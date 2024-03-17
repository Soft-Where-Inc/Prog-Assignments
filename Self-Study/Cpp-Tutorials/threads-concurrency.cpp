/*
 * -----------------------------------------------------------------------------
 * threads-concurrency.cpp: Simple test-cases to exercise C++ std::thread
 * concurrency primitives.
 *
 * Ref: Based on following CppCon talk
 *  [1] C++ Memory Model: from C++11 to C++23 - Alex Dathskovsky - CppCon 2023
 *      https://www.youtube.com/watch?v=SVEYNEWZLo4 ; Starting ~29m.10s
 *
 * Usage: g++ -o threads-concurrency threads-concurrency.cpp
 *        ./threads-concurrency [test_*]
 *        ./threads-concurrency [--help | test_<something> | test_<prefix> ]
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>

#include <thread>   // For std::thread, std::this_thread

#if __linux__
#include <cstring>
#include <cassert>
#endif // __linux__

using namespace std;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);
void test_threads_basic(void);

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
    , { "test_threads_basic"        , test_threads_basic }
};

// Test start / end info-msg macros
#define TEST_START()  cout << __func__ << " "
#define TEST_END()    cout << " ...OK" << endl
#define TEST_SKIP(msg)   do { cout << "... Unsupported: " << msg << endl; return; } while(0)

// Fabricate a string to track code-location of call-site.
#define __LOC__ \
    "[" + std::string{__func__} + ":" + std::to_string(__LINE__) + "] "

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
            cout << "Warning: Named test-function '"
                 << argv[1] << "' not found." << endl;
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

/**
 * The class thread represents a single thread of execution. Threads allow
 * multiple functions to execute concurrently.
 *
 * Most basic invocation of a thread, with the thread-handler-function
 * defined by an inline lambda-function. All that this thread does is to
 * sleep for a small interval, and then exits.
 */
void
test_threads_basic(void)
{
    TEST_START();

    std::thread([](){
                auto nms = 500;
                cout << __LOC__ << "Sleeping for " << nms << " milliseconds ...";
                std::this_thread::sleep_for(std::chrono::milliseconds(nms));
            }).join();

    TEST_END();
}

void
test_template(void)
{
    TEST_START();

    TEST_END();
}
