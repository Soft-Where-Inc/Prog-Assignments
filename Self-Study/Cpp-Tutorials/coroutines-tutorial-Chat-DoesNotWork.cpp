/*
 * -----------------------------------------------------------------------------
 * Implement sample usages of co-routines, a C++20 feature.
 *
 * Ref:
 *  [1] ChatGPT
 *
 * Usage: g++ -std=c++20 -o coroutines-tutorial coroutines-tutorial.cpp
 *        ./coroutines-tutorial [test_*]
 *        ./coroutines-tutorial [--help | test_<something> | test_<prefix> ]
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <experimental/coroutine>

using namespace std;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

/*
struct MyAwaitable {
    bool await_ready() { return false; }
    void await_suspend(std::experimental::coroutine_handle<>) {
        std::cout << "Awaiting..." << std::endl;
    }
    void await_resume() { std::cout << "Resuming..." << std::endl; }
};

struct MyCoroutine {
    MyAwaitable operator co_await() { return {}; }
};
*/

/*
struct MyAwaitable {
    bool await_ready() { return false; }
    void await_suspend(std::experimental::coroutine_handle<> handle) {
        std::cout << "Awaiting..." << std::endl;
        handle.resume(); // Resume the coroutine
    }
    void await_resume() { std::cout << "Resuming..." << std::endl; }
};

struct MyCoroutine {
    struct promise_type {
        MyAwaitable get_return_object() { return {}; }
        std::experimental::suspend_never initial_suspend() { return {}; }
        std::experimental::suspend_never final_suspend() noexcept { return {}; }

        // Add return_void here, to address this compilation error:
        // error: the coroutine promise type 'promise_type' must declare either 'return_value' or 'return_void'
        void return_void() {}

        void unhandled_exception() {}
    };
};

MyCoroutine coroutine() {
    // co_await MyAwaitable{};
    co_return;
}
*/

/*
struct MyAwaitable {
    bool await_ready() { return false; }
    void await_suspend(std::experimental::coroutine_handle<> handle) {
        std::cout << "Awaiting..." << std::endl;
        handle.resume(); // Resume the coroutine
    }
    void await_resume() { std::cout << "Resuming..." << std::endl; }
};

struct MyCoroutine {
    struct promise_type {
        MyAwaitable get_return_object() { return {}; }
        std::experimental::suspend_never initial_suspend() { return {}; }
        std::experimental::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {} // Add return_void here
        void unhandled_exception() {}
    };
};

MyCoroutine coroutine() {
    co_return; // co_return with no value to return a MyCoroutine object
}
*/

struct MyAwaitable {
    bool await_ready() { return false; }
    void await_suspend(std::experimental::coroutine_handle<> handle) {
        std::cout << "Awaiting..." << std::endl;
        handle.resume(); // Resume the coroutine
    }
    void await_resume() { std::cout << "Resuming..." << std::endl; }
};

struct MyCoroutine {
    struct promise_type {
        MyCoroutine get_return_object() { return {}; } // Return a MyCoroutine object
        std::experimental::suspend_never initial_suspend() { return {}; }
        std::experimental::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {} // Add return_void here
        void unhandled_exception() {}
    };
};

MyCoroutine coroutine() {
    co_return; // co_return with no value to return a MyCoroutine object
}




// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);
void test_coroutine_basic(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
                          { "test_this"                 , test_this }
                        , { "test_that"                 , test_that }
                        , { "test_coroutine_basic"      , test_coroutine_basic }
                      };

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

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

    // This code-chunk is as suggested by ChatGPT; Doesn't compile w/gcc:
    // error: 'co_await' cannot be used in the 'main' function
    /// MyCoroutine coroutine;
    /// co_await coroutine;
    /// std::cout << "Coroutine completed." << std::endl;

    coroutine();
    std::cout << "Coroutine completed." << std::endl;
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

// **** Co-Routines related test cases ****
void
test_coroutine_template(void)
{
    TEST_START();

    TEST_END();
}

void
test_coroutine_basic(void)
{
    TEST_START();


    TEST_END();
}
