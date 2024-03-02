/*
 * -----------------------------------------------------------------------------
 * b2b-iterators.cpp: Back to Basics C++ Iterators
 * Talk by Nicolai Josuttis - CppCon 2023
 *
 * Ref:
 *  [1] https://www.youtube.com/watch?v=26aW6aBVpk0&t=394s
 *
 * Usage: g++ -o b2b-iterators b2b-iterators.cpp
 *        ./b2b-iterators [test_*]
 *        ./b2b-iterators [--help | test_<something> | test_<prefix> ]
 *
 * Key-Points: ---- Iterators work through "Collections" ----
 *
 *  - Iterators through [begin(), end() ) - work on a half-open-range, where
 *    end() is "past-the-last-item-in-the-vector".
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <vector>

using namespace std;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);
void test_iter_basic_empty_vec(void);
void test_iter_basic_int_vec(void);
void test_iter_basic_string(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
      { "test_this"                     , test_this }
    , { "test_that"                     , test_that }
    , { "test_iter_basic_empty_vec"     , test_iter_basic_empty_vec }
    , { "test_iter_basic_int_vec"       , test_iter_basic_int_vec }
    , { "test_iter_basic_string"       , test_iter_basic_string }
};

// Test start / end info-msg macros
#define TEST_START()  cout << __func__ << ": "
#define TEST_END()    cout << " ...OK" << endl

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


/*
 * Basic usage of begin() / end() limits to print an empty vector.
 */
void
test_iter_basic_empty_vec()
{
    TEST_START();

    vector<int> v{};

    cout << "size=" << v.size() << " [ ";

    // Declare an iterator for current position of element in vector
    for (vector<int>::iterator pos = v.begin(); pos < v.end(); pos++) {
        cout << *pos << " ";
    }
    cout << "]";

    // For an empty collection: Size is 0, and begin will be same as end.
    assert(v.size() == 0);
    assert(v.begin() == v.end());

    TEST_END();
}

/*
 * Basic usage of begin() / end() limits to iterate thru vector of ints.
 */
void
test_iter_basic_int_vec()
{
    TEST_START();

    vector<int> v{0, -93, 42, 22, 16, 2000};

    cout << "size=" << v.size() << " [ ";

    // Declare 'last' as an iterator thru elements in a vector-of-ints.
    // Keep track of "last" seen element through iteration.
    vector<int>::iterator last = v.begin();

    // Use 'auto' to "infer" that we are declaring an iterator on vector<int>
    for (auto pos = v.begin(); pos < v.end(); pos++) {
        cout << *pos << " ";

        last = pos;
    }
    cout << "]";

    auto exp_size = 6;
    assert(v.size() == exp_size);
    assert((v.begin() + exp_size) == v.end());

    // When we finish iterating, "last" should be positioned to last element
    assert((v.begin() + exp_size - 1) == last);

    // Can dereference an iterator's current position with *<iter> notation
    assert(*last == 2000);

    TEST_END();
}

/*
 * Basic usage of begin() / end() limits to iterate thru a string, treating
 * it like a vector of chars.
 */
void
test_iter_basic_string()
{
    TEST_START();

    string s{"Hello World."};

    cout << "strlen=" << s.size() << " [ ";

    for (auto curr = s.begin(); curr < s.end(); curr++) {
        cout << *curr << " ";
    }
    cout << "]";

    // Size of vector-of-chars; i.e. string's length == C-strlen()
    assert(s.size() == strlen(s.c_str()));

    TEST_END();
}
