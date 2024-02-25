/*
 * -----------------------------------------------------------------------------
 * lamda-expressions-tutorial.cpp:
 *
 * Tutorial program to work through syntax & semantics of Lambda expressions.
 *
 * Ref:
 *  [1] MS https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170
 *
 *  [2] MS https://learn.microsoft.com/en-us/cpp/cpp/examples-of-lambda-expressions?view=msvc-170
 *
 * Usage: g++ -o lamda-expressions-tutorial lamda-expressions-tutorial.cpp
 *
 * History:
 *  RESOLVE: Started ... Incomplete.
 * -----------------------------------------------------------------------------
 */

#include <iostream>
#include <list>
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

void test_lambda_expr_basic(void);
void test_binding_rules_for_captured_variables(void);
void test_define_lambda_and_invoke(void);
void test_lambda_expr_as_fn_arg_to_api(void);
void test_use_lambda_expr_to_split_into_even_odd_lists(void);
void test_doSortFloats(void);
void test_doSortFloatsUsingLambdaFns(void);
void test_nested_lambda_exprs(void);
void test_lambda_expr_in_function(void);

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
                , { "test_lambda_expr_basic", test_lambda_expr_basic }
                , { "test_binding_rules_for_captured_variables"
                                            , test_binding_rules_for_captured_variables }
                , { "test_define_lambda_and_invoke"
                                            , test_define_lambda_and_invoke }
                , { "test_lambda_expr_as_fn_arg_to_api"
                                            , test_lambda_expr_as_fn_arg_to_api }
                , { "test_use_lambda_expr_to_split_into_even_odd_lists"
                                            , test_use_lambda_expr_to_split_into_even_odd_lists }
                , { "test_doSortFloats"     , test_doSortFloats }
                , { "test_doSortFloatsUsingLambdaFns"
                                            , test_doSortFloatsUsingLambdaFns }
                , { "test_nested_lambda_exprs"
                                            , test_nested_lambda_exprs }
                , { "test_lambda_expr_in_function"
                                            , test_lambda_expr_in_function }
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

// Overload << operator to print all elements in a list.
template <typename S>
ostream& operator<<(ostream& os, const list<S>& listl)
{
    // Printing all the elements using <<
    for (auto element : listl) {
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
 * Basic usage of lambda expressions (anonymous functions) to do simple math.
 * Based on [1] and [2].
 *
 * Parts of a lambda expression:
 *  []          : Capture clause; No captured variables; i.e. no local variables
 *                can be referenced in the body of the lambda expression/function.
 *  [&]         : All local variables visible in the scope of the lamda can be
 *                used in the body of the lambda by reference.
 *  [=]         : All local variables visible in the scope of the lamda can be
 *                used in the body of the lambda by value.
 *  ()          : Optional parameter list, in declaration; Reqd when invoking
 *  mutable     : Optional keyword
 *  throw()     : Optional exception-specification
 *  -> <type>   : Trailing return-type
 * -----------------------------------------------------------------------------
 */
void
test_lambda_expr_basic(void)
{
    TEST_START();

    // ----
    // Absolute minimum syntax needed to define a lambda expression.
    //  - You -NEED- at least the empty '[]' when declaring the expr
    //  - You -NEED- at least an empty '()' when invoking lambda expr/fn.
    auto lambdafnMin = [] { return (42); };
    cout << "lambdafnMin()=" << lambdafnMin() << endl;
    assert(lambdafnMin() == 42);

    // -----
    // Assign the lambda expression that adds two numbers to an auto variable.
    // NOTE That you don't need to declare the return type of the fn 'f1'.
    // Expression resulotion seems to resolve the return type.
    auto i1 = [](const int x, const int y) { return x + y; };

    cout << "Sum of (2, 3)=" << i1(2, 3) << endl;

    // -----
    // NOTE: You -cannot- declare 'const int y' as we do y++ inside the fn.
    auto f2_sum_x_incr_y = [](const int x, int y) { y++; return (x + y); };

    cout << "f2_sum_x_incr_y(2, 3)=" << f2_sum_x_incr_y(2, 3) << endl;

    // ----- Trailing return type usage ----
    // Define return type of the anonymous function using "-> <type>" clause
    // Even though the fn takes floats, and computes result as a float, the
    // value returned will be converted to 'int'.
    auto f3_sum_floats_return_int = [](const float x, const float y) -> int
                                        {
                                            auto rv = (x + y);
                                            cout << "[ Anon " << __func__ << ": rv="
                                                 << rv << " ] ";
                                            return rv;
                                        };

    const float f1{2.2};
    const float f2{3.3};
    cout << "f3_sum_floats_return_int(" << f1 << ", " << f2 << ")="
         << f3_sum_floats_return_int(f1, f2) << endl;

    // Assign the same lambda expression to a function object.
    function<int(int, int)> fnObj = [](int x, int y) { return x + y; };

    cout << "fnObj(): sum(3,4)=" << fnObj(3, 4) << endl;

    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * Test case to exercise different forms of captured variables; i.e. the stuff
 * mentioned enclosed in [], referencing to local variables declared in the
 * scope of the definition of the lambda expression. From [2].
 *
 * Per the standard, captured variables:
 *  - By value, are bound when the expression is declared. So, if this variable
 *    changes its value after the expression was created, the changed value
 *    will -not- be "seen" inside the lambda expression.
 *
 *  - By reference, are also bound when the expression is declared. As the
 *    binding is by reference, any change in value of this variable in the
 *    outer-scope, -will- be "seen" inside the lambda expression.
 * -----------------------------------------------------------------------------
 */
void
test_binding_rules_for_captured_variables(void)
{
    TEST_START();

    int i = 0;
    int j = 5;

    // The following lambda expression captures i by value and j by reference.
    function<int (void)> lambdaexpr = [i, &j] { return i + j; };

    auto rv = lambdaexpr();
    cout << "lambdaexpr()=" << rv << endl;
    assert(rv == j);

    // Change the values of i and j. Only new-value of 'j' will be reflected.
    i = 22;
    j = 42;

    // Call f and print its result.
    rv = lambdaexpr();
    cout << "lambdaexpr()=" << rv << endl;
    assert(rv == j);

    i = 0;  // Reset so we go back to initial value.
    j = 21;

    // ---------------------------------------------------------------------
    // The behaviour is slightly different if you have both parameters to
    // the lambda function and also captured variables. The same rules as
    // given above will apply to the captured variables. But if the lambda
    // function is invoked with a changed 'i', that will be reflected in the
    // result.
    // NOTE: This lambda-function is defined both with captured variables
    //        (i, j) that are in-scope of this declaration, -AND- also takes
    //        (param_i, param_j) that are passed when this fn will be invoked.
    // ---------------------------------------------------------------------
    auto lambdafn = [i, &j](const int param_i, const int param_j)
                           {
                                return (i + j + param_i + param_j);
                           };

    auto exp_rv = 42;
    rv = lambdafn(i, j);

    cout << "i=" << i << ", j=" << j
         << ", lambdafn(i, j)=" << rv << endl;
    assert(rv == exp_rv);

    // Change the value of 'i', which is also used as param_i. This will be
    // reflected in the computed sum.
    i = 2;
    exp_rv = 44;
    rv = lambdafn(i, j);

    cout << "i=" << i << ", j=" << j
         << ", lambdafn(i, j)=" << rv << endl;
    assert(rv == exp_rv);

    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * Test to define-and-invoke lambda expression with parameters immediately
 * supplied as part of the definition-invocation. From [2]
 * -----------------------------------------------------------------------------
 */
 void
 test_define_lambda_and_invoke(void)
 {
    TEST_START();

    auto rv = [] (int x, int y) { return x + y; }(5, 4);
    auto exp_rv = (5 + 4);
    cout << "Anon define-and-exec-SUM(5,4)=" << rv << endl;
    assert(rv == exp_rv);

    // NOTE: You cannot invoke this lambda expression with float params,
    // as that will need type conversion. You will get a compiler warning.
    // rv = [] (int x, int y) { return x + y; }(5.5, 4.4);

    TEST_END();
 }

/*
 * -----------------------------------------------------------------------------
 * Test case to demonstrate how-to use a lambda expression (function) as an
 * argument to an interface [here, std::find_if()] that needs a comparison
 * function to implement the semantic; here find_if().
 * (Developed based on example 2 from [2])
 * -----------------------------------------------------------------------------
 */
void
test_lambda_expr_as_fn_arg_to_api(void)
{
    TEST_START();

    list<int> numbers = {11, 42, 33, 5, 6, 9, 20, 10};
    const int exp_1st_even = 42;
    const int exp_1st_odd  = 11;

    cout << "List of numbers: " << numbers << endl;

    // Use the find_if() and a lambda expr to locate 1st even / odd number.
    const list<int>::const_iterator even_nos = find_if(numbers.begin(),
                                                       numbers.end(),
                                                       // Inline lambda expression.
                                                       [](int n) {return (n % 2) == 0;}
                                                       );

    // Print the result.
    if (even_nos != numbers.end()) {
        assert(*even_nos == exp_1st_even);
        cout << "The first even number in the list is " << *even_nos << "." << endl;
    } else {
        cout << "The list contains no even numbers." << endl;
    }

    const list<int>::const_iterator odd_nos = find_if(numbers.begin(),
                                                      numbers.end(),
                                                      // Inline lambda expression.
                                                       [](int n) {return (n % 2) == 1;}
                                                       );
    // Print the result.
    if (odd_nos != numbers.end()) {
        assert(*odd_nos == exp_1st_odd);
        cout << "The first odd number in the list is " << *odd_nos << "." << endl;
    } else {
        cout << "The list contains no odd numbers." << endl;
    }

    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * Test case to demonstrate use of partition() with a common lambda expression
 * split a list of numbers into even / odd lists.
 * (Developed based on example 2 from [2] and help from ChatGPT.)
 * -----------------------------------------------------------------------------
 */
void
test_use_lambda_expr_to_split_into_even_odd_lists(void)
{
    TEST_START();

    list<int> numbers = {11, 42, 33, 5, 6, 9, 20, 10};
    cout << "List of numbers: " << numbers << endl;

    // ---- Sub-case for even numbers list ----

    // Define lambda functions to decide if a number is odd / even.
    auto is_even = [](const int n) { return ((n % 2) == 0); };

    // Using std::partition to partition the numbers, with even #s at the head.
    auto partition_point = std::partition(numbers.begin(), numbers.end(), is_even);

    // Copying the even numbers to the even_nos list
    list<int> even_nos = {};
    std::copy_if(numbers.begin(), partition_point, std::back_inserter(even_nos),
                 is_even);

    // NOTE: Cannot use the following: Unlike std::vector, std::list does not
    // provide random access iterators, so you cannot directly use std::sort
    // with begin() and end() iterators from a std::list.
    // std::sort(even_nos.begin(), even_nos.end());
    // Instead do this:

    even_nos.sort();

    cout << "List of even numbers: " << even_nos << endl;

    list<int> exp_even_nos_list = {6, 10, 20, 42};
    assert(even_nos == exp_even_nos_list);

    // One way to create odd #s sub-list is to copy directly from the
    // partition point of the partitioned list. std::partition() has already
    // re-arranged numbers[] such that non-even #s are stacked up at the
    // rear-end of the list starting from ''partition_point'
    list<int> odd_nos_2 = {};   // Used later on for assertion checking
    std::copy(partition_point, numbers.end(), std::back_inserter(odd_nos_2));

    // ---- Sub-case for odd numbers list ----

    auto is_odd  = [](const int n) { return ((n % 2) == 1); };

    // Using std::partition to partition the numbers, with odd #s at the head.
    partition_point = std::partition(numbers.begin(), numbers.end(), is_odd);

    // Copying the odd numbers to the odd_nos list
    list<int> odd_nos = {};
    std::copy_if(numbers.begin(), partition_point, std::back_inserter(odd_nos),
                 is_odd);

    odd_nos.sort();

    cout << "List of odd numbers (Method-1): " << odd_nos << endl;

    list<int> exp_odd_nos_list = {5, 9, 11, 33};
    assert(odd_nos == exp_odd_nos_list);

    odd_nos_2.sort();

    cout << "List of odd numbers (Method-2): " << odd_nos_2 << endl;

    assert(odd_nos_2 == exp_odd_nos_list);

    TEST_END();
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

/*
 * -----------------------------------------------------------------------------
 * Demonstrate use of nested lambda expressions. Lifted from [2].
 * The inner lambda expression multiplies its argument by 2 and returns the
 * result. The outer lambda expression calls the inner lambda expression with
 * its argument and adds 3 to the result.
 *
 * The lambda expression is invoked-upon-definition with arg 5.
 * So, effectively it's calculating: (5 * 2) + 3 == 13.
 * -----------------------------------------------------------------------------
 */
void
test_nested_lambda_exprs(void)
{
    TEST_START();

    // The following lambda expression contains a nested lambda expression.
    // auto timestwoplusthree = [](int x) { return [](int y) { return y * 2; }(x) + 3; }(5);
                          // Start of outer lambda-fn
    auto timestwoplusthree =
                [](int x) {
                                  // Start of inner lambda-fn
                              return [](int y) {
                                                    return y * 2;
                                               } // Inner lambda-fn ends here.

                                                // Inner lambda is invoked here,
                                                // on the arg received by outer
                                                // lambda-fn; i.e. 5
                                                (x) + 3;
                          }(5);

    // Print the result.
    cout << "Value returned by nested-lambda-fns=" << timestwoplusthree << endl;
    assert(timestwoplusthree == 13);

    TEST_END();
}

/*
 * -----------------------------------------------------------------------------
 * Demonstrate use of lambda expressions in functions and use of for_each()
 * syntax to iterate through each element in a vector. Lifted from [2].
 * -----------------------------------------------------------------------------
 */

// Define a Scale class, to specify scaling factor to be applied to a number.
class Scale
{
  public:
    // The constructor.
    explicit Scale(int scale) : _scale(scale) {}

    // Prints the product of each element in a vector object and the scale factor
    void applyScale(const vector<int>& v) const
    {
        // Define lambda function that captures the _scale value by reference.
        // Apply it to each term of the input vector.
        for_each(v.begin(), v.end(), [=](int n) { cout << n * _scale << " "; });
    }

  private:
    int _scale;
};

void
test_lambda_expr_in_function(void)
{
    TEST_START();

    vector<int> numbers = {};
    numbers.push_back(3);
    numbers.push_back(2);
    numbers.push_back(1);
    numbers.push_back(20);

    Scale scale(5);
    cout << "Numbers=[ " << numbers << "] Scaled by 5: ";
    scale.applyScale(numbers);

    TEST_END();
}
