/*
 * -----------------------------------------------------------------------------
 * Implement example usages to understand smart pointers better, based on the
 * material covered in this video, from CppCon 2019.
 *
 * Ref: https://www.youtube.com/watch?v=xGDLkt-jBJ4&t=2346s)
 *      Arthur O'Dwyer “Back to Basics: Smart Pointers”
 *
 * Overview:
 *  - auto_ptr  : C++98; Deprecated in C++11. removged in C++17.
 *
 *  - unique_ptr    : C++11 replacement for auto_ptr. C++14 adds make_unique
 *
 *  - shared_ptr    : C+=11. Adds ref-counting. C++17 adds shared_ptr<T[]>
 *  - weak_ptr      : C++11. "Weak" references.
 *
 * Usage: g++ -o template-program-cpp template-program.cpp
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

// Pointer-specific test case prototypes
void test_shared_ptrs_basic_string(void);
void test_shared_ptrs_basic_int(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
      { "test_this"                         , test_this }
    , { "test_that"                         , test_that }
    , { "test_shared_ptrs_basic_string"     , test_shared_ptrs_basic_string }
    , { "test_shared_ptrs_basic_int"        , test_shared_ptrs_basic_int }
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
        int found = 0;
        for (; tctr < ARRAYSIZE(Test_fns); tctr++) {
            if (!strncmp(Test_fns[tctr].tfn_name, argv[1], strlen(argv[1]))) {
                Test_fns[tctr].tfn();
                found++;
            }
        }
        if (!found) {
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
 * Basic usage of shared pointers is similar to other pointers.
 *
 *  - Usage of 'auto' to establish another shared pointer
 *  - Usage of pointer->length(), to get size of datum pointer points to.
 *  - Shared pointers are assignable like ordinary pointers are
 *  - They are copy-constructible.
 */
void
test_shared_ptrs_basic_string(void)
{
    TEST_START();

    std::shared_ptr<string> pSharedPtr2Str = make_shared<string>("Hello");

    // Allocate a new shared ptr 'q' pointing to data pointed to by 'p'
    auto qNowPointsTo_p = pSharedPtr2Str;
    assert(*qNowPointsTo_p == *pSharedPtr2Str);

    pSharedPtr2Str = nullptr;

    cout << "q.length()=" << qNowPointsTo_p->length() << ": '" << *qNowPointsTo_p << "'"
         << endl;
    TEST_END();
}

/*
 * Basic usage of shared pointers is similar to other pointers.
 *
 *  - Usage of 'auto' to establish another shared pointer
 *  - Usage of pointer->length(), to get size of datum pointer points to.
 */
void
test_shared_ptrs_basic_int(void)
{
    TEST_START();

    std::shared_ptr<int> pSharedPtr2Int = make_shared<int>(42);

    // Allocate a new shared ptr 'q' pointing to data pointed to by 'p'
    auto qNowPointsTo_p = pSharedPtr2Int;
    assert(*qNowPointsTo_p == *pSharedPtr2Int);

    pSharedPtr2Int = nullptr;

    // Can use them as implicit "!= nullptr" boolean check.
    if (qNowPointsTo_p) {
        cout << "q.sizeof()=" << sizeof(*qNowPointsTo_p) << ", Value=" << *qNowPointsTo_p
             << endl;
    }
    TEST_END();
}
