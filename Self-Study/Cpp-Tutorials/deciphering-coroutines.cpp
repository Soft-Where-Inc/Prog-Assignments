/*
 * -----------------------------------------------------------------------------
 * Deciphering Coroutines: You Tube Talk
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
 * -----------------------------------------------------------------------------
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

/*
 * AnyCoroReturnType{} - Definition of object returned by the coroutine.
 *
 * - promise_type{} is the object that remains "inside" the coroutine and is the
 *   interfacing object between the caller and the coroutine.
 *   - Is the "central intersection point" between caller & coroutine code.
 *   - This object is automatically generated -by-the-compiler. You do nothing.
 */
struct AnyCoroReturnType {

    // Minimal implementation to get compilation to work.
    struct promise_type {
        AnyCoroReturnType   get_return_object() { return {}; }

        // Specifies that the coroutine should never be initially suspended.
        std::experimental::suspend_never initial_suspend() { return {}; }

        void return_void() { }

        void unhandled_exception() { }

        // Specifies what happens when the coroutine termintes. Always use
        // 'noexcept' as it's difficult to deal with exceptions when terminating.
        std::experimental::suspend_always final_suspend() noexcept { return {}; }
    };
};

/*
 * Most basic, simplest coroutine which does basically nothing.
 */

AnyCoroReturnType
coro_hello_world(void)
{
    cout << __LOC__
         << "Hello World! [ Printed when we do: suspend_never initial_suspend() ]"
         << endl;
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
