/*
 * coroutines-simplest-example.cpp:
 * -----------------------------------------------------------------------------
 * Simplest example of using Co-routines in C++ downloaded from the net.
 * By Alexey Timin.
 *
 * Ref:
 *  [1] https://dev.to/atimin/the-simplest-example-of-coroutines-in-c20-4l7a
 *
 * Usage: g++ -o coroutines-simplest-example coroutines-simplest-example.cpp
 *        ./coroutines-simplest-example [test_*]
 *        ./coroutines-simplest-example [--help | test_<something> | test_<prefix> ]
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <coroutine>
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
 * Sample code copied from above tutorial.
 * *****************************************************************************
 */
std::queue<std::function<bool()>> task_queue;

struct sleep {
    sleep(int n) : delay{n} {}

    constexpr bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) const noexcept {
        auto start = std::chrono::steady_clock::now();
        task_queue.push([start, h, d = delay] {
            if (decltype(start)::clock::now() - start > d) {
                h.resume();
                return true;
            } else {
                return false;
            }
        });
    }

    void await_resume() const noexcept {}

    std::chrono::milliseconds delay;
};


struct Task {
    struct promise_type {
        promise_type() = default;
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
    };
};

Task foo1() noexcept {
    std::cout << "1. hello from foo1" << std::endl;
    for (int i = 0; i < 10; ++i) {
        co_await sleep{10};
        std::cout << "2. hello from foo1" << std::endl;
    }
}

Task foo2() noexcept {
    std::cout << "1. hello from foo2" << std::endl;
    for (int i = 0; i < 10; ++i) {
        co_await sleep{10};
        std::cout << "2. hello from foo2" << std::endl;
    }
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

    while (!task_queue.empty()) {
        auto task = task_queue.front();
        if (!task()) {
            task_queue.push(task);
        }
        task_queue.pop();

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

