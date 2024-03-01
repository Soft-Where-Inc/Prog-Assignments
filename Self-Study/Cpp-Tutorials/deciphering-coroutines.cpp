/*
 * -----------------------------------------------------------------------------
 * Deciphering Coroutines: You Tube Talk
 *
 * Implement basic coroutines from simple examples.
 * Problem Statement: Implement Fibonacci series using coroutines.
 *
 * -----------------------------------------------------------------------------
 * Ref:
 *  [1] https://www.youtube.com/watch?v=J7fYddslH0Q, Andreas Weis
 *      CppCon 2022: Deciphering C++ Coroutines - A Diagrammatic Coroutine Cheat Sheet
        https://www.linkedin.com/in/andreas-weis-0459994b/
 *
 * Usage: g++ -o deciphering-coroutines deciphering-coroutines.cpp
 *        ./deciphering-coroutines [test_*]
 *        ./deciphering-coroutines [--help | test_<something> | test_<prefix> ]
 *
 * History:
 *  Thu 29.Feb.2024: Went for Chris Ramming's going-away lunch in Los Altos.
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <thread>

using namespace std;

typedef unsigned int uint32;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);
void test_runFiboGenerator_basic(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
          { "test_this"                       , test_this }
        , { "test_that"                       , test_that }
        , { "test_runFiboGenerator_basic"     , test_runFiboGenerator_basic }
};

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

/*
 * *****************************************************************************
 *      **** Fibonacci Sequence Definitions and Implementation begins here ****
 * *****************************************************************************
 */

/*
 * *****************************************************************************
 * Initial implementation of Fibonacci Generator.
 * Generate hard-coded 100 initial numbers in Fibonacci sequence.
 * *****************************************************************************
 */
void runFiboGenerator(void)
{
    uint32 ictr = 0;
    uint32 i1 = 1;
    uint32 i2 = 1;
    while (ictr++ < 100) {
        cout << i1 << " " << std::flush;
        i1 = std::exchange(i2, (i1 + i2));
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    cout << endl;
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

void
test_runFiboGenerator_basic(void)
{
    TEST_START();
    runFiboGenerator();
    TEST_END();
}

