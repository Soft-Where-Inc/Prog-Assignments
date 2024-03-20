/*
 * -----------------------------------------------------------------------------
 * future-promise-async-basic.pp: Implement basic usages of future / promise
 * async execution paradigms from C++ standard.
 *
 * Ref:
 *  [1] C++ Threading #7: Future, Promise and async(), Bo Qian
 *      https://www.youtube.com/watch?v=SZQ6-pf-5Us
 *
 * Usage: g++ -o future-promise-async-basic future-promise-async-basic.pp
 *        ./future-promise-async-basic [test_*]
 *        ./future-promise-async-basic [--help | test_<something> | test_<prefix> ]
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <fmt/core.h>
#include <thread>
#include <future>

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
void test_factorial_thread(void);
void test_factorial_async(void);
void test_factorial_deferred(void);

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
    , { "test_factorial_thread"     , test_factorial_thread }
    , { "test_factorial_async"      , test_factorial_async }
    , { "test_factorial_deferred"   , test_factorial_deferred }

};

// Test start / end info-msg macros
#define TEST_START()  cout << __func__ << "() "
#define TEST_END()    cout << " ...OK" << endl
#define TEST_SKIP(msg)   do { cout << "... Unsupported: " << msg << endl; return; } while(0)

// Fabricate a string to track code-location of call-site.
#define __LOC__ \
    "[" + std::string{__func__} + "():" + std::to_string(__LINE__) + "] "

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

// **** Helper methods ****
/**
 * ends_with() : Similar to Python's str.endswith(substr)
 *
 * - Do length-check first. Fail if it does not satisfy.
 * - Then, do an interesting use of "reverse-iterator" rbegin(), rend().
 *
 *                        ┌──str.rbegin()
 *                        ▼
 *    str ──►[ ...... a b c ]
 *        substr ───►[a b c ]
 *                  ▲     ▲
 * substr.rend()────┘     └──substr.rbegin()
 *
 * Ref: https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
*/
inline bool ends_with(std::string const & str, std::string const & substr)
{
    if (substr.size() > str.size()) {
        return false;
    }
    // Walk backwards, using reverse iterators, from the end of the substr,
    // rbegin(), matching till just-prior-to-its begin-char, rend(), comparing
    // from the last-char of the input string 'str'.
    return std::equal(substr.rbegin(), substr.rend(), str.rbegin());
}

// **** Helper functions for test cases ****
/**
 * Basic definition of n! factorial(n), which will print the result as a message.
 */
void factorial(int n) {
    int res = 1;
    for (auto i = n; i > 1; --i) {
        res *= i;
    }
    std::thread::id this_id = std::this_thread::get_id();
    cout << "ThreadID=" << this_id;
    fmt::print(" Factorial {}! = {} ", n, res);
}

/**
 * Definition of n! factorial(n), which will be executed as an async function.
 * Returns n!
 */
int factorial_async_fn(int n) {
    int res = 1;
    for (auto i = n; i > 1; --i) {
        res *= i;
    }
    // Print messages to show that async-fn is being executed and sleeping ...
    std::thread::id this_id = std::this_thread::get_id();
    cout << "ThreadID=" << this_id;
    fmt::print("{} Inducing artifical sleep for {} seconds ...", __LOC__, n);
    std::cout << std::flush;
    std::this_thread::sleep_for(std::chrono::seconds(n));
    return res;
}

/**
 * Definition of n! factorial(n), which will be executed as an async function,
 * but this is invoked using std::launch::deferred by the caller. So, this
 * function will be executed in the same [ main() ] thread of the caller.
 *.
 * Returns n!
 */
int factorial_deferred_fn(int n, std::thread::id caller_tid) {
    int res = 1;
    for (auto i = n; i > 1; --i) {
        res *= i;
    }
    // Print messages to show that async-fn is being executed and sleeping ...
    std::thread::id this_id = std::this_thread::get_id();

    // Confirm that caller -did- invoke using std::launch::deferred interface.
    assert(caller_tid == this_id);

    cout << "ThreadID=" << this_id;
    fmt::print("{} Inducing artifical sleep for {} seconds ...", __LOC__, n);
    std::cout << std::flush;
    std::this_thread::sleep_for(std::chrono::seconds(n));
    return res;
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

    // Exercise this helper method for base cases
    string substr = "Hello World.";
    assert(ends_with("Hello",        substr) == false);
    assert(ends_with("Hello World",  substr) == false);
    assert(ends_with("Hello World.", substr) == true);

    test_msg(__LOC__ + "Hello World.");
    TEST_END();
}

void
test_msg(string msg)
{
    TEST_START();
    cout << msg;
    ends_with(msg, "Hello World.");
    TEST_END();
}

/* Exercise factorial() function executed by a class thread. */
void
test_factorial_thread(void)
{
    TEST_START();

    std::thread t1(factorial, 4);

    t1.join();

    TEST_END();
}

/* Exercise factorial() function executed by an async function */
void
test_factorial_async(void)
{
    TEST_START();

    int n{5};

    std::thread::id this_id = std::this_thread::get_id();
    cout << "Main ThreadID=" << this_id << " ";

    // Async function returns -very-important-thing: A Future!
    // async() method returns a FUTURE OBJECT!
    std::future<int> fu_res = std::async(factorial_async_fn, n);

    // Print messages to show that async-fn is being executed and sleeping ...
    fmt::print("{} Factorial {}! = ... ", __LOC__, n);
    std::cout << std::flush;

    // ------------------------------------------------------------------------
    // You can execute the function and get the result using .get() method
    // FUTURE is a "channel" to get result from child thread executing 'async'
    // fn. fu_res.get() will wait till child thread finishes and will receive
    // the result returned by the child thread.
    // ------------------------------------------------------------------------
    // NOTE: You can call .get() -ONLY-ONCE- to draw results!
    auto res = fu_res.get();

    fmt::print(" is {} ", res);
    TEST_END();
}

/**
 * Exercise factorial() function executed by deferred execution by the same
 * thread as this main(). Deferred function will be executed when .get() is
 * invoked below.
 */
void
test_factorial_deferred(void)
{
    TEST_START();

    int n{5};

    std::thread::id this_id = std::this_thread::get_id();
    cout << "Main ThreadID=" << this_id << " ";

    // Async function returns -very-important-thing: A Future!
    // async() method returns a FUTURE OBJECT!
    std::future<int> fu_res = std::async(std::launch::deferred,
                                         factorial_deferred_fn, n, this_id);

    // Print messages to show that async-fn is being executed and sleeping ...
    fmt::print("{} Factorial {}! = ... ", __LOC__, n);
    std::cout << std::flush;

    // ------------------------------------------------------------------------
    // You can execute the function and get the result using .get() method
    // FUTURE is a "channel" to get result from child thread executing 'async'
    // fn. fu_res.get() will wait till child thread finishes and will receive
    // the result returned by the child thread.
    // ------------------------------------------------------------------------
    // NOTE: You can call .get() -ONLY-ONCE- to draw results!
    auto res = 0;
    res = fu_res.get();

    fmt::print(" is {} ", res);
    TEST_END();
}

void
test_template(void)
{
    TEST_START();

    TEST_END();
}
