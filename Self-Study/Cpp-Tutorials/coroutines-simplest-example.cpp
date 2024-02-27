/*
 * coroutines-simplest-example.cpp:
 * -----------------------------------------------------------------------------
 * Simplest example of using Co-routines in C++ downloaded from the net.
 * By Alexey Timin.
 *
 * Updated commentary and fixed compilation errors, and changes to get the
 * basic program to run on my Mac using these references:
 *
 * Usage: g++ -std=c++20 -o coroutines-simplest-example coroutines-simplest-example.cpp
 *        ./coroutines-simplest-example [test_*]
 *        ./coroutines-simplest-example [--help | test_<something> | test_<prefix> ]
 *        ./coroutines-simplest-example test_coroutines
 *
 * -----------------------------------------------------------------------------
 *          **** Description of coroutines workflow: ****
 * -----------------------------------------------------------------------------
 *
 * Although [1] claims this to be a "simplest example", it is quite complex.
 *
 * - There are 2 coroutine functions: cor_fn1(), cor_fn2(), which have a
 *   suspend / resume interplay, through the test case method,
 *   test_coroutines(), that invokes this whole machinery.
 *
 * - There is a global Task_queue<>, which is a handle to an anonymous
 *   lambda function that will be enqueued by 2 coroutine functions.
 *
 * - Each coroutine function loops around n-times enqueueing tasks, which
 *   will add an instance of the lambda-function to the queue. Then, the
 *   coroutine will be suspended.
 *
 *   To trace which coroutine is enqueueing which task, we use distinct
 *   sleep delay values for each coroutine. cor_fn1() uses [0..5] and
 *   cor_fn2() uses [6..10]. NIters_coro_1, NIters_coro_2 control the loop
 *   parameters.
 *
 * - When execution continues in the test case, test_coroutines(), we
 *   execute the 1st task in the queue. "Executing the task" means the lambda
 *   function that was enqueued will be executed.
 *
 *   The _lambda_function()_ will check for sufficient elapsed time, and if
 *   so, the issuing coroutine will be resumed.
 *
 * Execution flow in the test_coroutines() test looks like:
 *
 *  - cor_fn1() - enqueue a task. Suspended.
 *  - cor_fn2() - enqueue a task. Suspended.
 *  - Control flow continues to test_coroutines(), which will execute the
 *    task at the head of the queue. And then will dequeue the task.
 *
 *    Lock-step coroutine execution begins here:
 *
 *      - Exec task enqueued by cor_fn1, which will resume cor_fn1()
 *      - cor_fn1() enqueues another task. cor_fn1() is suspended.
 *
 *      - Exec task enqueued by cor_fn2, which will resume cor_fn2()
 *      - cor_fn2() enqueues another task. cor_fn2() is suspended.
 *
 *      - Exec task enqueued by cor_fn1, which will resume cor_fn1()
 *      - cor_fn1() enqueues another task. cor_fn1() is suspended.
 *
 *      - Exec task enqueued by cor_fn2, which will resume cor_fn2()
 *      - cor_fn1() enqueues another task. cor_fn2() is suspended.
 *      ....
 *
 * -----------------------------------------------------------------------------
 * Ref:
 *  [1] https://dev.to/atimin/the-simplest-example-of-coroutines-in-c20-4l7a
 *
 *  [2] https://www.youtube.com/watch?v=8sEe-4tig_A&t=1983
 *      CppCon 2022: C++20's Coroutines for Beginners - Andreas Fertig
 *
 *  [3] https://www.chiark.greenend.org.uk/~sgtatham/quasiblog/coroutines-c++20/
 *      Simon Tatham, 2023-08-06
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

void test_coroutines(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
                          { "test_this"         , test_this }
                        , { "test_that"         , test_that }
                        , { "test_coroutines"   , test_coroutines }
                      };

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

// Fabricate a string to track code-location of call-site.
#define LOC() \
    "[" + std::string{__func__} + ":" + std::to_string(__LINE__) + "] "

/*
 * *****************************************************************************
 * Begin Coroutines Sample code copied from above tutorial.
 * *****************************************************************************
 */

constexpr int NIters_coro_1{5};
constexpr int NIters_coro_2{5};

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
        cout << "Constructor for sleep{} called from '"
             << callerfn << "', n=" << n << endl;
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

        cout << LOC() << "Perform Task_queue.push()" << endl;
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
                                cout << LOC()
                                     << "Lambda-fn(), delay d="
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
    //  - co_yield  -> promise_type.yield_value(); Specify a value you want to
    //                 deliver to someone.
    //  - co_await  -> promise_type.await_transform(); Indicate a "thing" you
    //                 want to wait-for. "Wait-for" input value.
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
        // std::experimental::suspend_always initial_suspend() { return {}; }

        // Specifies that the coroutine should not be suspended on startup.
        // (i.e. before returning to the caller / main() )
        std::experimental::suspend_never initial_suspend() { return {}; }

        // Specifies what happens when the coroutine termintes. Always use
        // 'noexcept' as it's difficult to deal with exceptions when terminating.
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
cor_fn1() noexcept {
    // cout << "[" << __func__ << ":" << __LINE__ << "]: Hello! "
    cout << endl << LOC() + "**** Hello! Starting co-routine 1 ..." << endl;

    // Use diff ictrs, passed as delay param to sleep(), so we can see
    // in trace outputs which function is being called.
    for (auto i = 0; i < NIters_coro_1; i++) {
        cout << endl << LOC() << "Hello ictr=" << i << "... Going to sleep\n";

        // NOTE: Compiler converts operant to `co_await` into an awaiter object,
        // which will tell it (the coroutine[?]) how to manage the suspension.
        // The await_suspend() method defined in the sleep{} object dictates
        // what action is tken when await is invoked.
        co_await sleep{i, __func__};
    }
    co_return; // Returns nothing.
}

Task
cor_fn2() noexcept {
    cout << endl << LOC() << "**** Hello! Starting co-routine 2 ..." << endl;

    // Use diff jctrs, passed as delay param to sleep(), so we can see
    // in trace outputs which function is being called.
    for (int j = NIters_coro_1; j < (NIters_coro_1 + NIters_coro_2); j++) {
        cout << endl << LOC() << "Hello jctr=" << j << "... Going to sleep\n";
        co_await sleep{j, __func__};
    }
    // As coroutine returns void, we can skip this stmt as well.
    // co_return; // Returns nothing.
}

/*
 * Test case to exercise execution of two co-routines, cor_fn1() and cor_fn2().
 * Sample code copied from above tutorial.
 */
void
test_coroutines(void)
{
    TEST_START();

    cor_fn1();
    cor_fn2();

    cout << endl
         << LOC() << "Process all tasks till Task_queue() is not empty ..."
         << "\n\n";
    while (!Task_queue.empty()) {

        // 'task' is now a handle to the enqueue lambda-fn
        auto task = Task_queue.front();
        cout << endl << LOC() << "Executing enqueued task's lambda-fn() ...\n";

        // Execute the lambda function, which will check for elapsed-time
        // v/s delay required. If sufficient time has elapsed, the lambda-fn
        // will resume the owning coroutine's handle.
        if (!task()) {
            cout << LOC() << "Task is completed.\n";
            Task_queue.push(task);
        }
        Task_queue.pop();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    TEST_END();
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
