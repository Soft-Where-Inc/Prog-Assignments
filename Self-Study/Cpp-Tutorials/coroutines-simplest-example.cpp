/*
 * coroutines-simplest-example.cpp:
 * -----------------------------------------------------------------------------
 * Simplest example of using Co-routines in C++ downloaded from the net.
 * By Alexey Timin.
 *
 * Updated commentary and fixed compilation errors, and changes to get the
 * basic program to run on my Mac using these references:
 *
 * Ref:
 *  [1] https://dev.to/atimin/the-simplest-example-of-coroutines-in-c20-4l7a
 *
 *  [2] https://www.youtube.com/watch?v=8sEe-4tig_A&t=1983
 *      CppCon 2022: C++20's Coroutines for Beginners - Andreas Fertig
 *
 *  [3] https://www.chiark.greenend.org.uk/~sgtatham/quasiblog/coroutines-c++20/
 *      Simon Tatham, 2023-08-06
 *
 * Usage: g++ -std=c++20 -o coroutines-simplest-example coroutines-simplest-example.cpp
 *        ./coroutines-simplest-example [test_*]
 *        ./coroutines-simplest-example [--help | test_<something> | test_<prefix> ]
 *
 * History:
 *  Sun, 25.Feb.2024; 5pm: Deepu called from DC. Erin had some meetings with
 *  senators for his Non-profit work. Deepu is chaperoning him there for 3 days.
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <experimental/coroutine>
#include <thread>
#include <queue>
#include <functional>

using namespace std;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
                          { "test_this"     , test_this }
                        , { "test_that"     , test_that }
                      };

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

/*
 * *****************************************************************************
 * Begin Coroutines Sample code copied from above tutorial.
 * *****************************************************************************
 */
/*
 * *****************************************************************************
 * Task_queue: Global variable: Represents a queue of tasks, where each task
 * is a callable target (function) that takes no arguments and returns a bool
 * value.
 *
 * From Chat: std::function<bool()>: std::function is a polymorphic function
 * wrapper provided by the C++ Standard Library. It can hold any callable
 * target (functions, lambdas, function objects, etc.) that matches a specified
 * signature.
 *
 * Here, bool() specifies the return type in the signature of the callable
 * target that std::function can hold. The signature indicates that the target
 * callable should take no arguments and return a bool value.
 * *****************************************************************************
 */
std::queue<std::function<bool()>> Task_queue;

/*
 * *****************************************************************************
 * From ChatGPT: This code block implements the await_suspend() function,
 * which is used in coroutines to suspend execution until a certain condition
 * (in this case, a time delay) is met. It pushes a lambda function into a
 * task queue (Task_queue), which will be executed asynchronously.
 * The lambda checks if the elapsed time exceeds a specified delay and resumes
 * the coroutine handle accordingly.
 * *****************************************************************************
 */
struct sleep {

    // Define a constructor for the sleep struct that takes an integer
    // parameter n and initializes the member variable `delay` with the value
    // of n. The empty constructor body indicates that no additional operations
    // are performed during initialization.
    // However, we enhanced the interface to accept the calling fn's name,
    // which then gets printed in the body, for diagnostics.
    sleep(int n, string callerfn) : delay{n} {
        cout << callerfn << ":Sleep n=" << n << endl;
    }

    constexpr bool await_ready() const noexcept { return false; }

    // Mandatory to have a method named exactly 'await_suspend()' in this
    // object. The call to "co_await sleep()" to instantiate one such object
    // will go through the STLibrary's coroutine interfaces, which will look
    // for a method named exactly like so.
    // (If you change the name, it will raise this error:
    // error: no member named 'await_suspend' in 'sleep'.)
    void
    await_suspend(std::experimental::coroutine_handle<> corhdl) noexcept
    {
        // Record the start time before the async operation begins.
        auto start = std::chrono::steady_clock::now();

        cout << __func__ << ":" << __LINE__ << ": Task_queue.push()" << endl;
        // --------------------------------------------------------------------
        // From Chat: Push a lambda-function taking 3 variables captured
        // by-value, into the global Task_queue. This lambda represents the
        // asynchronous operation that needs to be suspended until completion.
        //
        // When this queued-task will [eventually] execute, the lambda-fn
        // will decide if the delay has elapsed and [may] resume the
        // coroutine. If the coroutine was resumed, the task is successful.
        // Otherwise, it's deemed a failure (still some more time has to pass).
        // --------------------------------------------------------------------
        Task_queue.push([start, corhdl, d = delay]
                        {
                            // Check if the elapsed-time is greater than the
                            // specified 'delay' ('d'). If so, the coroutine,
                            // corhdl, can be resumed.
                            if (decltype(start)::clock::now() - start > d) {
                                cout << __func__ << ":" << __LINE__
                                     << ": await_suspend(), delay d="
                                     << std::chrono::milliseconds(d).count()
                                     << ", returns; resume coroutine."
                                     << endl;

                                // This jugglery is to workaround following g++
                                // error: 'this' argument to member function 'resume' has type 'const std::experimental::coroutine_handle<>', but function is not marked const
                                //
                                std::experimental::coroutine_handle<> resumehdl = corhdl;
                                resumehdl.resume();

                                // Success: Asynchronous operation has completed
                                return true;
                            } else {
                                // Asynchronous operation is still pending
                                // and needs to wait.
                                return false;
                            }
                        });
    }

    // Again: Mandatory to have a method named exactly like this below.
    // Declares a coroutine operation that doesn't return any value and doesn't
    // perform any specific action when the coroutine is resumed. It's a
    // placeholder for a coroutine operation that might need to be defined in
    // certain coroutine implementations but doesn't require any specific
    // behavior in this context.
    // This operation doesn't modify the state of the object (const) and doesn't
    // throw exceptions.
    void await_resume() const noexcept {}

    std::chrono::milliseconds delay;
};


/*
 * *****************************************************************************
 * Task{} - This is the coroutine's wrapper type. This is also the return type
 * of the coroutine itself [See foo1() and foo2() below.]
 *
 * Technically, a coroutine is a Finite State machine that can be customized
 * by the members of the mandatory promise_type, embedded in this wrapper type.
 *
 * From ChatGPT, to debug this error:
 *
 *  error: the coroutine promise type 'promise_type' must declare either 'return_value' or 'return_void'
 *
 * In C++ coroutines, the promise type is a structure or class defined within
 * the coroutine type. It is responsible for managing the coroutine's state,
 * including the result value or the absence of a result. The promise type
 * typically contains methods that are used during the coroutine's execution,
 * such as return_value() or return_void().
 * *****************************************************************************
 */
struct Task {

    // ------------------------------------------------------------------------
    // NOTE: Mandatory part of the interface of std::experimental::coroutine_traits
    //       It requires a member defined with exactly 'promise_type' name.
    //       Otherwise, you will get a compiler error.
    //
    // Here's the mapping from the member fields of this promise_type() to the
    // coroutine 'action-verbs'. A specific instance of promise_type() may
    // choose to implement only some of these methods, leaving the rest to the
    // default behaviour: (From [2])
    //
    //  - co_yield  -> promise_type.yield_value();
    //  - co_await  -> promise_type.await_transform();
    //  - co_return -> promise_type.return_value() / return_void();
    // ------------------------------------------------------------------------
    struct promise_type {
        // Default constructor
        promise_type() = default;


        // ---------------------------------------------------------------------
        // Everything below here is "customization" [2] for our specific
        // use-case of a co-routine.
        // ---------------------------------------------------------------------

        // Compiler invokes this method when it first sets-up the coroutine.
        // Method responsible for creating the object that will represent the
        // result of the coroutine. In this case, it returns a default-constructed
        // Task object.
        Task get_return_object() { return {}; }

        // Specifies that the coroutine should be initially suspended.
        std::experimental::suspend_never initial_suspend() { return {}; }
        // std::experimental::suspend_always initial_suspend() { return {}; }

        std::experimental::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}

        // Define return_value() if coroutine returns a value
        // (See note above about diagnostics from Chat.)
        // void return_value(int value) {}
        // void return_value(void) {} // Modify return_value() to accept void

        // In this case, coroutine does not return anything. So define this.
        void return_void() {}
    };
};

/*
 * *****************************************************************************
 * You have to define these functions as returning Task{}, which is a struct
 * defined with a co-routine type. (You cannot change this to return void.)
 * These two functions are coroutines.
 * *****************************************************************************
 */
Task
foo1() noexcept {
    cout << "[" << __func__ << ":" << __LINE__ << "]: Hello! "
         << endl;

    auto i = 0;
    // Use diff ictrs, passed as delay param to sleep(), so we can see
    // in trace outputs which function is being called.
    for (; i < 10; i++) {
        cout << "[" << __func__ << ":" << __LINE__
             << "]: Hello ... Going to sleep "
             << endl;
        co_await sleep{i, __func__};
    }
    cout << "[" << __func__ << ":" << __LINE__ << "]:"
         << "Finished, i=" << i
         << endl;
    co_return; // Returns nothing.
}

Task
foo2() noexcept {
    cout << "[" << __func__ << ":" << __LINE__ << "]: Hello! "
         << endl;

    // Use diff jctrs, passed as delay param to sleep(), so we can see
    // in trace outputs which function is being called.
    for (int j = 10; j < 20; j++) {
        cout << "[" << __func__ << ":" << __LINE__
             << "]: Hello ... Going to sleep "
             << endl;
        co_await sleep{j, __func__};
    }
    // As coroutine returns void, we can skip this stmt as well.
    // co_return; // Returns nothing.
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

    // Sample code copied from above tutorial.
    foo1();
    foo2();

    while (!Task_queue.empty()) {
        auto task = Task_queue.front();
        if (!task()) {
            Task_queue.push(task);
        }
        Task_queue.pop();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;

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

