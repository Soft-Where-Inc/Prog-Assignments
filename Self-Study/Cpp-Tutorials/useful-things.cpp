/*
 * -----------------------------------------------------------------------------
 * useful-things.cpp: Simple program to test out simple things, to figure how
 *  the syntax & semantics works.
 *
 * Ref:
 *
 * Usage: g++ -o useful-things useful-things.cpp
 *        ./useful-things [test_*]
 *        ./useful-things [--help | test_<something> | test_<prefix> ]
 *
 * History:
 * -----------------------------------------------------------------------------
 */

#include <iostream>

using namespace std;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);
void test_find(void);

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
                        , { "test_find"     , test_find }
                      };

// Test start / end info-msg macros
#define TEST_START()  cout << __func__ << " "
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

/*
 * test_find(): Exercise str.find() and str.substr() to see how they work.
 */
void
test_find(void)
{
    TEST_START();

    // Even though string has 2 ':'-s, find() returns 1st instance.
    std::string s{"This:is a:separated string"};

    int colon_pos = s.find(":");
    cout << "colon pos=" << colon_pos << endl;
    assert(colon_pos == 4);

    // Characters not found, find() will return < 0;
    assert(s.find('?') == -1);
    int bang_pos = s.find("!");
    cout << "find(!) returns " << s.find("!") << ", bang_pos=" << bang_pos << endl;
    assert(s.find("!") == -1);

    std::string prefix = s.substr(0, colon_pos);
    cout << "Prefix is: '" << prefix << "'" << endl;
    assert(prefix == "This");

    // Suffix will include the 1st location of separator ':'
    std::string suffix = s.substr(colon_pos, s.size());
    cout << "Suffix is: '" << suffix << "'" << endl;
    assert(suffix == ":is a:separated string");

    TEST_END();
}

void
test_template(void)
{
    TEST_START();

    TEST_END();
}
