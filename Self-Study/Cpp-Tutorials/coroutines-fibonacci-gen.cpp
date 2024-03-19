/*
 * -----------------------------------------------------------------------------
 * Implement Fibonacci Generator as a co-routine.
 *
 * Ref: Bjarne Stroutstrup's talk at CppCon 2021, Slide ~1:06:18
 * https://www.youtube.com/watch?v=15QF2q66NhU&list=PLHTh1InhhwT6vjwMy3RG5Tnahw0G9qIx6
 *
 * Usage: g++ -o coroutines-fibonacci-gen coroutines-fibonacci-gen.cpp
 *        ./coroutines-fibonacci-gen [test_*]
 *        ./coroutines-fibonacci-gen [--help | test_<something> | test_<prefix> ]
 *
 * RESOLVE: This does not compile both on Linux or Mac. The unresolved reference
 * is to generator<int> fibonacci() definition. This templated `generator` class
 * is supposed to be available as per https://en.cppreference.com/w/cpp/coroutine/generator
 * but I did not find the headers on:
 * Mac: /Library/Developer/CommandLineTools/SDKs/MacOSX12.1.sdk/usr/include/c++/v1/
 *      Or under .../c++/V1/experimental
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>

#include <iterator>
#include <coroutine>
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
void test_coro_fibonacci(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
          { "test_this"             , test_this }
        , { "test_that"             , test_that }
        , { "test_coro_fibonacci"   , test_coro_fibonacci }
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

/*
 * Simple Fibonacci sequence generator implemented as a coroutine.
 */
generator<int> fibonacci()
{
    int a = 0;
    int b = 1;  // Initial state
    while (true) {
        int next = a + b;
        co_yield a; // Return next Fibonacci number
        a = b;
        b = next;   // Update state
    }
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

void
test_coro_fibonacci(void)
{
    TEST_START();

    for (auto v: fibonacci()) { // Use co-routine
        cout << v << " ";
    }

    TEST_END();
}
