/*
 * -----------------------------------------------------------------------------
 * Deciphering Coroutines: By Andreas Weis, CppCon2022 You Tube Talk
 *
 * Implement basic coroutines from simple examples.
 * Problem Statement: Implement Fibonacci series using coroutines.
 *
 * Salient points about coroutines:
 *
 * - Think of coroutines -not- as a function that executes but as a factory
 *   function that returns a coroutine "object", which the caller can use
 *   to resume the coroutine. Coroutine object holds the state needed to
 *   resume the coroutine where it was suspended / left-off during execn.
 *
 * - C++20, coroutines are "stack-less". I.e., when suspended, control flow
 *   always returns to immediate caller.
 *
 * - You cannot tell from the outside, by just looking at the interface of a
 *   function if it's a coroutine. They look-and-behave just like a normal fn.
 *   It's the presence of `co_await`, `co_yield` or `co_return` that tells the
 *   compiler to treat this function as a coroutine.
 *
 * - Can convert a promise to a coroutine_handle and vice-versa
 *
 * - 'Awaitable' is the type of the argument that I can call the `co_await`
 *    operator with inside the coroutine.
 *    - Usually, you do co_await to stall for a result, which may be sending
 *      a result (of some computation) back to the caller, via the promise_type.
 *    - co_await are the -only- code points where there is an opportunity
 *      for execution to be suspended, and control handed back to the caller.
 *    - Not every co_await call will _result_ in suspension, but it _can_.
 *
 * Ref:
 *  [1] https://www.youtube.com/watch?v=J7fYddslH0Q, Andreas Weis
 *      CppCon 2022: Deciphering C++ Coroutines - A Diagrammatic Coroutine Cheat Sheet
        https://www.linkedin.com/in/andreas-weis-0459994b/
 *
 * Usage: g++ -o deciphering-coroutines deciphering-coroutines.cpp
 *        ./deciphering-coroutines [test_*]
 *        ./deciphering-coroutines [--help | test_<something> | test_<prefix> ]
 *
 * History:
 *  Thu 29.Feb.2024: Went for Chris Ramming's going-away lunch in Los Altos.
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <thread>
#include <experimental/coroutine>

using namespace std;

typedef unsigned int uint32;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);
void test_runFiboGenerator_basic(void);
void test_coro_hello_world_basic(void);
void test_resume_coro_hello_world(void);
void test_coro_hello_world_await_42(void);
void test_coro_FiboGenerator(void);
void test_coro_FiboGenerator2(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
          { "test_this"                       , test_this }
        , { "test_that"                       , test_that }
        , { "test_runFiboGenerator_basic"     , test_runFiboGenerator_basic }
        , { "test_coro_hello_world_basic"     , test_coro_hello_world_basic }
        , { "test_resume_coro_hello_world"    , test_resume_coro_hello_world }
        , { "test_coro_hello_world_await_42"  , test_coro_hello_world_await_42 }
        , { "test_coro_FiboGenerator"         , test_coro_FiboGenerator }
        , { "test_coro_FiboGenerator2"        , test_coro_FiboGenerator2 }
};

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

// Fabricate a string to track code-location of call-site.
#define __LOC__ \
    "[" + std::string{__func__} + ":" + std::to_string(__LINE__) + "] "

/*
 * *****************************************************************************
 *      **** Coroutines Definitions and Implementation begins here ****
 * *****************************************************************************
 */

constexpr uint32 AnyCoroReturnType_Initial_Value{999999};

constexpr uint32 AnyCoroReturnType_Answer{42};

/*
 * -----------------------------------------------------------------------------
 * AnyCoroReturnType{} - Definition of object returned by the coroutine.
 *
 * - promise_type{} is the object that remains "inside" the coroutine and is the
 *   interfacing object between the caller and the coroutine.
 *   - Is the "central intersection point" between caller & coroutine code.
 *   - This object is automatically generated -by-the-compiler. You do nothing.
 *
 * The code construction below (of coroutine_handle being passed as a
 * constructor arg and stored as a member variable of ReturnType) is the most
 * typical code form. (There are other ways to store this handle elsewhere,
 * but the video did not discuss those.)
 */
struct AnyCoroReturnType {

    // Minimal implementation to get compilation to work.
    struct promise_type {

        // Member item to exchange data thru coroutine handle and await event
        uint32  value_{AnyCoroReturnType_Initial_Value};

        // Create the coroutine_handle from the promise (as they are both
        // interchangeable), and pass that handle as a constructor arg to the
        // return type.
        AnyCoroReturnType   get_return_object() {
            return AnyCoroReturnType {
                std::experimental::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        // Specifies that the coroutine should always be initially suspended.
        std::experimental::suspend_always initial_suspend() { return {}; }

        void return_void() { }

        void unhandled_exception() { }

        // Specifies what happens when the coroutine termintes. Always use
        // 'noexcept' as it's difficult to deal with exceptions when terminating.
        std::experimental::suspend_always final_suspend() noexcept { return {}; }
    };

    // ------------------
    // Member fields
    // ------------------
    std::experimental::coroutine_handle<promise_type> corhdl;

    // Constructor: Takes coroutine_handle<promise_type> as an arg in its
    // constructor and store it in member variable, `corhdl`.
    AnyCoroReturnType(std::experimental::coroutine_handle<promise_type> h)
        : corhdl(h) { }

    // ------------------
    // Methods
    // ------------------
    // Resume uses this handle to resume coroutine.
    void resume() { corhdl.resume(); }

    uint32 getAnswer() { return corhdl.promise().value_; }
};

/*
 * -----------------------------------------------------------------------------
 * Type defined which is a parameter to the co_await() call in the coroutine.
 * -----------------------------------------------------------------------------
 */
struct Awaitable {
    // Member item that coroutine will await-on
    uint32  value_;

    // Should we resume after pushing value_ to promise_type?
    bool    do_resume_;

    // Constructor: Initialize value & resume flag from user-specific input.
    Awaitable(uint32 v) : value_{v} {
        // We expect that the more common case is that the client that wants
        // to initialize the await() structure will push a new value to the
        // promise, and would like to resume executing the coroutine.
        do_resume_ = true;
        cout << __LOC__ << "Constructor Awaitable{}: Use default do_resume_=" << do_resume_
             << endl;
    };

    // Caller that wishes to control the resume semantics can use this interface.
    Awaitable(uint32 v, bool resume) : value_{v} {
        // cout << __LOC__ << "Constructor Awaitable{}: Set do_resume_=" << resume
        //      << endl;

        do_resume_ = resume;
    };

    bool    await_ready() { return false; }

    // Customization point that will be executed shortly before
    // the coroutine goes to sleep (get suspended). This function takes as
    // its argument the coroutine_handle to the coroutine function that is
    // about to be suspended.
    void    await_suspend(std::experimental::coroutine_handle<AnyCoroReturnType::promise_type> corhdl) {

        // Poke the value_ we are awaiting-ON back to the coroutine handle's promise
        corhdl.promise().value_ = value_;

        if (do_resume_) {
            corhdl.resume();
        }
    }

    // Function that gets executed right before coroutine wakes up again.
    void    await_resume() { }
};

/*
 * -----------------------------------------------------------------------------
 * Most basic, simplest coroutine which does basically nothing.
 * -----------------------------------------------------------------------------
 */
AnyCoroReturnType
coro_hello_world(void)
{
    cout << __LOC__
         << "Hello World! [ Printed when we do: "
            "suspend_never initial_suspend(), or coroutine is resume()'ed ]"
         << endl;

    co_return;
}

/*
 * -----------------------------------------------------------------------------
 * Next version of a simple coroutine which invokes co_await interface to
 * promise a value to the caller. The ReturnType is still defined as:
 * `suspend_always initial_suspend` ...
 * -----------------------------------------------------------------------------
 */
AnyCoroReturnType
coro_hello_world_await_42(void)
{
    cout << __LOC__
         << "Hello World! [ Printed when coroutine is resume()'ed"
            ", as we are in suspend_always initial_suspend() ... ]"
         << endl;

    cout << __LOC__
         <<  "Coroutine will be suspended immediately after setting"
             " the answer to " << AnyCoroReturnType_Answer << endl;

    co_await Awaitable{AnyCoroReturnType_Answer};

    cout << __LOC__
         <<  "Coroutine executing after co_await completes." << endl;

    co_return;
}


/*
 * *****************************************************************************
 *      **** Fibonacci Sequence Definitions and Implementation begins here ****
 * *****************************************************************************
 */

/*
 * *****************************************************************************
 * Initial implementation of Fibonacci Generator.
 * Generate hard-coded 100 initial numbers in Fibonacci sequence.
 * *****************************************************************************
 */
void runFiboGenerator(void)
{
    uint32 ictr = 0;
    uint32 i1 = 1;
    uint32 i2 = 1;
    while (ictr++ < 100) {
        cout << i1 << " " << std::flush;
        i1 = std::exchange(i2, (i1 + i2));
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    cout << endl;
}

/*
 * *****************************************************************************
 * Fibonacci Generator implemented as a coroutine.
 * Here we basically loop endlessly, sending one output value to the caller and
 * then await. A resume() issued by the caller will step us to the next
 * number, send it out, and then await.
 * *****************************************************************************
 */
AnyCoroReturnType
coro_FiboGenerator(void)
{
    uint32 ictr = 0;

    uint32 i1 = 1;
    uint32 i2 = 1;

    // Return initial value of i1, and wait. (For initial value of i2, we will
    // perform the await below, on the 1st iteration of the loop.)
    // false => Do -not- resume this coroutine after every await call. Let
    //          the caller invoke resume() to resume control-flow.
    co_await Awaitable{i1, false};
    while (true) {

        // Perform: i1 = i2; i2 = (i1 + i2);
        i1 = std::exchange(i2, (i1 + i2));

        // cout << __LOC__
        //      << "await(): ictr=" << ictr << ", i1=" << i1 << ", i2=" << i2
        //      << endl;

        // Return the next Fibonacci value, i1, and await.
        co_await Awaitable{i1, false};
    }
    co_return;
}

/*
 * *****************************************************************************
 * Variation of Fibonacci Generator implemented as a coroutine.
 * Change the suspend / resume logic slightly here, so that there is less of
 * a mental-strain in tracking `await()` calls. There is only point where we
 * return current (next) Fibonacci number, and then do an await. Then, upon
 * resumption by the caller, we will compute the next number, go back into the
 * loop and then again await().
 * *****************************************************************************
 */
AnyCoroReturnType
coro_FiboGenerator2(void)
{
    uint32 ictr = 0;

    uint32 i1 = 1;
    uint32 i2 = 1;

    while (true) {

        // cout << __LOC__ << "Call co_await(): number=" << i1 << "... ";
        printf("[%s():%d] Call co_await: number=%4u ...",
               __func__, __LINE__, i1);

        // Return current value of i1, and wait. (For initial value of i2, we will
        // perform the await below, on the 1st iteration of the loop.)
        // false => Do -not- resume this coroutine after every await call. Let
        //          the caller invoke resume() to resume control-flow.
        co_await Awaitable{i1, false};

        // When coroutine is resumed Perform: i1 = i2; i2 = (i1 + i2);
        i1 = std::exchange(i2, (i1 + i2));

    }
    co_return;
}

/*
 * *****************************************************************************
 * main()
 * *****************************************************************************
 */
int
main(const int argc, const char *argv[])
{
    string hello_msg = "Hello World.";
    cout << argv[0] << ": " << hello_msg << " (argc=" << argc << ")" << endl;

    int rv = 0;

    // Run all test cases if no args are provided.
    if (argc == 1) {
        for (int tctr = 0; tctr < ARRAYSIZE(Test_fns); tctr++) {
            Test_fns[tctr].tfn();
        }
    } else if (strncmp("--help", argv[1], strlen("--help")) == 0) {
        cout << argv[0] << Usage << endl;
        return 0;
    } else if (strncmp("test_", argv[1], strlen("test_")) == 0) {
        // Execute the named test-function, if it's a supported test-function
        int tctr = 0;
        int ntests = 0;
        for (; tctr < ARRAYSIZE(Test_fns); tctr++) {
            if (!strncmp(Test_fns[tctr].tfn_name, argv[1], strlen(argv[1]))) {
                Test_fns[tctr].tfn();
                ntests++;
            }
        }
        if (tctr == ARRAYSIZE(Test_fns) && !ntests) {
            printf("Warning: Named test-function '%s' not found.\n", argv[1]);
            rv = 1;
        }
    } else {
        cout << "Unknown argument: " << argv[1] << endl;
    }
    return rv;
}

// **** Test cases ****

void
test_this(void)
{
    TEST_START();

    assert(1 == 1);
    TEST_END();
}

void
test_that(void)
{
    TEST_START();
    test_msg("Hello World.");
    TEST_END();
}

void
test_msg(string msg)
{
    TEST_START();
    assert(msg == "Hello World.");
}

void
test_runFiboGenerator_basic(void)
{
    TEST_START();
    runFiboGenerator();
    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * With suspend_always initial_suspend() defined in promise_type{}, the
 * coroutine will always be suspended. It will not print the 'Hello world' msg.
 * -----------------------------------------------------------------------------
 */
void
test_coro_hello_world_basic(void)
{
    TEST_START();

    cout << endl;
    cout << __LOC__ << "Executing coro_hello_world() [ Prints nothing with suspend_always initial_suspend() ] ..." << endl;
    coro_hello_world();
    cout << __LOC__ << "Returned from coro_hello_world() ..." << endl;
    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * With suspend_always initial_suspend() defined in promise_type{}, the
 * coroutine will be suspended. It will not print the 'Hello world' msg.
 *
 * However, in this test case, we grab and store the coroutine handle,
 * returned by `cor = coro_hello_world()`. And perform cor.resume() to
 * resume the coroutine.
 *
 * You should see the `Hello World` message from the coroutine inteleaved
 * between the messages emitted by this function.
 * -----------------------------------------------------------------------------
 */
void
test_resume_coro_hello_world(void)
{
    TEST_START();

    cout << endl;
    cout << __LOC__ << "Executing coro_hello_world() [ Prints nothing"
                       " with suspend_always initial_suspend() ] ..."
         << endl;

    AnyCoroReturnType cor = coro_hello_world();

    cor.resume();

    cout << __LOC__ << "Returned from coro_hello_world() ..." << endl;
    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * This test case exercises a slightly different coroutine,
 * coro_hello_world_await_42(), which deploys the `co_await` metaphor to
 * exchange data via the coroutine handle to the caller. Coroutine scaffolding
 * has been enhanced to support this data-exchange between the coroutine
 * handle and the promise_type.
 *
 * The return type is still defined so that the coroutine will be suspended
 * on entry. At this time, if we enquire for the return value, we will still
 * get an uninitialized value (-1), indicating that the coroutine is still
 * sleeping and has not performed the 'action' that will be triggered when
 * it is suspended -during-execution- where it will save the value in the
 * promise. [ See the work done in Awaitable.await_suspend() method. ]
 * -----------------------------------------------------------------------------
 */
void
test_coro_hello_world_await_42(void)
{
    TEST_START();

    cout << endl;
    cout << __LOC__ << "Executing coro_hello_world() [ Prints nothing"
                       " with suspend_always initial_suspend() ] ..."
         << endl;

    uint got_value = 0;

    AnyCoroReturnType cor = coro_hello_world_await_42();

    got_value = cor.getAnswer();
    cout << __LOC__ << "Returned value=" << got_value
         << ". Coroutine is still suspended due to suspend_always of type."
         << endl;

    // Verify that this is still the initial value as coroutine is still
    // suspended, and has not -YET- got to the `co_await` line of execution
    assert(got_value == AnyCoroReturnType_Initial_Value);

    cor.resume();

    got_value = cor.getAnswer();

    cout << __LOC__ << "Returned " << got_value
                    << " from coro_hello_world_await_42() ..." << endl;
    assert(got_value == AnyCoroReturnType_Answer);
    TEST_END();
}

void
test_coro_FiboGenerator(void)
{
    TEST_START();

    cout << endl;
    AnyCoroReturnType cor = coro_FiboGenerator();

    // Coroutine is suspended on entry, so do an initial resume, first.
    cor.resume();

    // Keep pulling next Fibonacci number from the generator, till we get tired.
    uint32 ictr = 0;
    while (ictr < 20) {

        uint32 next_num = cor.getAnswer();

        // This is not compiling in my version of g++. Resort to printf().
        // cout << __LOC__ << std::format("Fibo[{} ]={}", ictr, next_num) << endl;
        printf("[%s():%d] Fibo[%2d] = %4u\n",
               __func__, __LINE__, ictr, next_num);

        cor.resume();
        ictr++;
    }
    TEST_END();
}

void
test_coro_FiboGenerator2(void)
{
    TEST_START();

    cout << endl;
    AnyCoroReturnType cor = coro_FiboGenerator2();

    // Coroutine is suspended on entry, so do an initial resume, first.
    cor.resume();

    // Keep pulling next Fibonacci number from the generator, till we get tired.
    uint32 ictr = 0;
    while (ictr < 20) {

        uint32 next_num = cor.getAnswer();

        // This is not compiling in my version of g++. Resort to printf().
        // cout << __LOC__ << std::format("Fibo[{} ]={}", ictr, next_num) << endl;
        printf("[%s():%d] Fibo[%2d] = %4u ... resume ...\n",
               __func__, __LINE__, ictr, next_num);

        cor.resume();
        ictr++;
    }
    TEST_END();
}
