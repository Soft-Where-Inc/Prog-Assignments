/*
 * -----------------------------------------------------------------------------
 * threads-concurrency.cpp: Simple test-cases to exercise C++ std::thread
 * concurrency primitives.
 *
 * The class `thread` represents a single thread of execution. Threads allow
 * multiple functions to execute concurrently.
 *
 * Ref: Based on following CppCon talk
 *
 *  [1] C++ Memory Model: from C++11 to C++23 - Alex Dathskovsky - CppCon 2023
 *      https://www.youtube.com/watch?v=SVEYNEWZLo4 ; Starting ~29m.10s
 *
 * Pre-requisites:
 *  - {fmt} printing library: https://fmt.dev/latest/index.html
 *
 * ---- Usage: ----
 *
 *  Linux> $ sudo apt remove libfmt8/jammy
 *  Linux> $ sudo apt-get install -y libfmt8/jammy
 *  Linux> $ sudo apt-get install -y libfmt-dev/jammy
 *
 *  # ------------------------------------------------------------------------
 *  # Make sure to pass -l<library> name -after- the source file that uses it
 * https://stackoverflow.com/questions/61594396/linking-errors-with-fmt-undefined-reference-to-stdstring-fmtv6internal
 *  # ------------------------------------------------------------------------
 *
 *  Linux> $ g++ -std=c++20 -o threads-concurrency threads-concurrency.cpp -lfmt
 *
 *  # To find out search-path used by linked to load libraries:
 *  Linux> $ ld --verbose | grep SEARCH_DIR | tr -s ' ;' \\012
 *
 *    Mac> $ brew reinstall fmt
 *    Mac> $ g++ -std=c++20 -o threads-concurrency -lfmt threads-concurrency.cpp
 *
 *  $ ./threads-concurrency [test_*]
 *  $ ./threads-concurrency [--help | test_<something> | test_<prefix> ]
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>

#include <thread>   // For std::thread, std::this_thread
#include <fmt/core.h>

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
void test_thread_local(void);

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
    , { "test_thread_local"         , test_thread_local }
};

// Test start / end info-msg macros
#define TEST_START()  cout << __func__ << "() "
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
 * ****************************************************************************
 * Most basic invocation of a thread, with the thread-handler-function
 * defined by an inline lambda-function. All that this thread does is to
 * sleep for a small interval, and then exits.
 *
 * The advantage of using std::thread library is that you do -NOT- need
 * to further link with -pthread (for Posix threads library). C++ STL
 * takes care of this library by default.
 * *****************************************************************************
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

/**
 * ****************************************************************************
 * Exercise the use of thread-local variables.
 */
constexpr uint64_t      tlocal_ctr_initial = 0;
thread_local uint64_t   tlocal_ctr = tlocal_ctr_initial;

// Define a thread-function handler to increment this thread-local counter
// Return the final value via an output reference parameter
void
do_count(const string tname, int ntimes, uint64_t& ret)
{
    // Increment thread-local counter ntimes
    for (auto i = 0; i < ntimes; i++) {
        tlocal_ctr++;
    }
    fmt::print("Excuted ThreadID='{}', new tlocal_ctr={}\n", tname, tlocal_ctr);

    // Return final value of thread-local counter for this thread
    ret = tlocal_ctr;
}

/**
 * ****************************************************************************
 * Spawn 2 concurrent threads, each increment its own thread-local counter.
 * Verify the output returned by thread-handler is the expected counter value.
 *
 * Key Points:
 *  - The main(), here test_thread_local() fn run by main(), is also a thread.
 *  - When you execute a thread, you copy everything inside it. So, each thread
 *    has its own thread-local copy of `tlocal_ctr`. To return this incremented
 *    value from each thread to the main() [caller], we pass-in the reta / retb
 *    variables via a std:ref reference.
 *
 * Results: The output value returned by each thread will have updated the
 * local counter, but the main thread's local counter should remain uninit'ed.
 * ****************************************************************************
 */
void
test_thread_local(void)
{
    TEST_START();

    cout << endl;

    uint64_t    reta{};
    uint64_t    retb{};

    // Start two threads, invoking the common incrementer function, returning
    // the final result of incremented thread-local counter via output param
    auto ntimes_a = 10;
    std::thread ta(do_count, "tA-ntimes=10", 10, std::ref(reta));

    auto ntimes_b = 20;
    std::thread tb(do_count, "tB-ntimes=20", 20, std::ref(retb));

    ta.join();
    tb.join();

    fmt::print("reta={}, retb={}, main tlocal_ctr={}\n", reta, retb, tlocal_ctr);
    
    // Verify test results
    assert(reta == ntimes_a);   // each thread's local counter should have
    assert(retb == ntimes_b);   // been incremented

    // Main thread's counter should have remained unchanged at its initial value.
    assert(tlocal_ctr == tlocal_ctr_initial);

    TEST_END();
}

void
test_template(void)
{
    TEST_START();

    TEST_END();
}
