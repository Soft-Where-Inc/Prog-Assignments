/*
 * -----------------------------------------------------------------------------
 * lamda-expressions-tutorial.cpp:
 *
 * Tutorial program to work through syntax & semantics of Lambda expressions.
 *
 * Ref:
 *  [1] MS https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170
 *
 * Usage: g++ -o lamda-expressions-tutorial lamda-expressions-tutorial.cpp
 *
 * History:
 *  RESOLVE: Started ... Incomplete.
 * -----------------------------------------------------------------------------
 */

#include <iostream>
#include <vector>

using namespace std;

string Usage = " [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Code Function Prototypes
void doSortFloats(vector<float>&);

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(string);

void test_doSortFloats(void);
void test_doSortFloatsUsingLambdaFns(void);

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
                , { "test_doSortFloats"     , test_doSortFloats }
                , { "test_doSortFloatsUsingLambdaFns"
                                            , test_doSortFloatsUsingLambdaFns }
    };

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

/*
 * -----------------------------------------------------------------------------
 * Function to overload << operator to generically print all elements in a
 * vector. Uses template of generic typename 'S'.
 *
 * Note: The arg name 'vector' is allowed even though 'vector' is a keyword.
 *
 * Ref:  https://www.geeksforgeeks.org/different-ways-to-print-elements-of-vector/
 * -----------------------------------------------------------------------------
 */
template <typename S>
ostream& operator<<(ostream& os, const vector<S>& vector)
{
    // Printing all the elements using <<
    for (auto element : vector) {
        os << element << " ";
    }
    return os;
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

// **** Sample Tutorial Code ****

/*
 * Receive a reference to a vector of floats. Sort in-place in ascending order.
 */
void
doSortFloats(vector<float>& floats)
{
    std::sort(floats.begin(), floats.end());
}

/*
 * Classical Compare-function, that will sort floats in ascending order.
 * Ref: https://en.cppreference.com/w/cpp/algorithm/sort
 *
 * Comparison function object (i.e. an object that satisfies the requirements
 * of Compare) which returns true if the first argument is less than (i.e., is
 * ordered before) the second.
 */
bool
floatCmp(float f1, float f2)
{
    return (f1 < f2);
}

void
doSortFloatsCmpfn(vector<float>& floats)
{
    std::sort(floats.begin(), floats.end(), floatCmp);
}

/*
 * Descending sort Compare-function, that will sort floats in descending order.
 */
bool
floatDescCmp(float f1, float f2)
{
    // Returns reverse of normal comparison, which is: (f1 < f2) ? True : False;
    return (f1 >= f2);
}

void
doDescSortFloatsCmpfn(vector<float>& floats)
{
    std::sort(floats.begin(), floats.end(), floatDescCmp);
}

/*
 * *****************************************************************************
 * Implement std::sort() using ascending sort-compare function defined
 * as a lambda function. Based on [1]
 * *****************************************************************************
 */
void
doSortFloatsLambdaCmpfn(vector<float>& floats)
{
    std::sort(floats.begin(), floats.end(),
               // Lambda expression begins here
               [](float f1, float f2) {
                    return (f1 < f2);
               }
               // Lambda expression ends here
            );
}

/*
 * *****************************************************************************
 * Implement std::sort() using descending sort-compare function defined
 * as a lambda function. Based on [1]
 * *****************************************************************************
 */
void
doDescSortFloatsLambdaCmpfn(vector<float>& floats)
{
    std::sort(floats.begin(), floats.end(),
               // Lambda expression begins here
               [](float f1, float f2) {
                    return (f1 >= f2);
               }
               // Lambda expression ends here
            );
}

/*
 * *****************************************************************************
 * Implement std::sort() using ascending sort-compare function on absolute float
 * value, defined as a lambda function. Based on [1]
 * *****************************************************************************
 */
void
doSortFloatsAbsValueLambdaCmpfn(vector<float>& floats)
{
    std::sort(floats.begin(), floats.end(),
               // Lambda expression begins here
               [](float f1, float f2) {
                    return (std::abs(f1) < std::abs(f2));
               }
               // Lambda expression ends here
            );
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
 * -----------------------------------------------------------------------------
 * Test case to exercise different flavours of interfaces to sort floats.
 * Use an initial unsorted vector of arrays. Declare an expected sorted array
 * of floats. Exercise different interfaces to sort the vector, using std::sort().
 *
 * Verify that the resultant array matches the expected sorted array of floats.
 * -----------------------------------------------------------------------------
 */
void
test_doSortFloats(void)
{
    TEST_START();
    cout << endl;

    vector<float> floatsUnsorted   = { 3.123, -3.123, 1.9, -1.9, 2, 0 };
    vector<float> floatsSorted     = {-3.123, -1.9, 0, 1.9, 2, 3.123 }; // expected
    vector<float> floatsSortedDesc = { 3.123, 2, 1.9, 0, -1.9, -3.123 };

    // -- Sorting vector of floats using default sortcmp() function.
    vector<float> floats = floatsUnsorted;

    cout << "doSortFloats(): Unsorted: " << floats;

    doSortFloats(floats);

    cout << " Sorted: " << floats << endl;
    assert(floats == floatsSorted);

    // -- Sorting vector of floats using user-specified floatCmp() fn
    floats = floatsUnsorted;

    cout << "doSortFloatsCmpfn(): Unsorted: " << floats;

    doSortFloatsCmpfn(floats);

    cout << " Sorted: " << floats << endl;
    assert(floats == floatsSorted);

    // -- Descending sort vector of floats using user-specified floatDescCmp()
    floats = floatsUnsorted;

    cout << "doDescSortFloatsCmpfn(): Unsorted: " << floats;

    doDescSortFloatsCmpfn(floats);

    cout << " DescSorted: " << floats << endl;
    assert(floats == floatsSortedDesc);

    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * Test case to exercise different flavours of interfaces to sort floats,
 * using lambda functions to specify the sort-comparison functions.
 * Use an initial unsorted vector of arrays. Declare an expected sorted array
 * of floats. Exercise different interfaces to sort the vector, using std::sort().
 *
 * Verify that the resultant array matches the expected sorted array of floats.
 * -----------------------------------------------------------------------------
 */
void
test_doSortFloatsUsingLambdaFns(void)
{
    TEST_START();
    cout << endl;

    vector<float> floatsUnsorted   = { 3.123, -3.123, -1.9, 1.9, 2, 0 };
    vector<float> floatsSorted     = {-3.123, -1.9, 0, 1.9, 2, 3.123 }; // expected

    // Sorting by absolute-values will only rearrange the items as they
    // appear in the unsorted input-list. The floatsUnsorted is intentionally
    // setup so that we have a pair of (3.123, -3.123) and another pair (-1.9, 1.9)
    // where the +ve / -ve terms come in diff orders. In the sorted list, we
    // should expect to see the order retained, as shown below:
    vector<float> floatsSortAbsVal = { 0, -1.9, 1.9, 2, 3.123, -3.123 }; // expected

    vector<float> floatsSortedDesc = { 3.123, 2, 1.9, 0, -1.9, -3.123 };

    // -- Sorting vector of floats using ascending sort lambda function.
    vector<float> floats = floatsUnsorted;

    cout << "doSortFloatsLambdaCmpfn(): Unsorted: " << floats;

    doSortFloatsLambdaCmpfn(floats);

    cout << " Sorted: " << floats << endl;
    assert(floats == floatsSorted);

    // -- Sorting vector of floats using descending sort lambda function.
    floats = floatsUnsorted;

    cout << "doDescSortFloatsLambdaCmpfn(): Unsorted: " << floats;

    doDescSortFloatsLambdaCmpfn(floats);

    cout << " DescSorted: " << floats << endl;
    assert(floats == floatsSortedDesc);

    // -- Ascending sort of vector of floats using absolute value in lambda fn
    floats = floatsUnsorted;

    cout << "doSortFloatsAbsValueLambdaCmpfn(): Unsorted: " << floats;

    doSortFloatsAbsValueLambdaCmpfn(floats);

    cout << " Sorted: " << floats << endl;
    assert(floats == floatsSortAbsVal);

    TEST_END();
}
