/*
 * -----------------------------------------------------------------------------
 * b2b-iterators.cpp: Back to Basics C++ Iterators
 * Talk by Nicolai Josuttis - CppCon 2023
 *
 * Ref:
 *  [1] https://www.youtube.com/watch?v=26aW6aBVpk0&t=394s
 *
 *  [2] Good discussion of support for std::format() and alternatives.
 *      https://stackoverflow.com/questions/63724059/does-gcc-support-c20-stdformat
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
#include <set>
#include <list>

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
void test_prContainer_int_vector(void);
void test_prContainer_string(void);
void test_prContainer_strings(void);
void test_prContainer_set_of_strings(void);
void test_printContainer_list_of_names(void);

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
    , { "test_iter_basic_string"        , test_iter_basic_string }
    , { "test_prContainer_int_vector"   , test_prContainer_int_vector }
    , { "test_prContainer_string"       , test_prContainer_string }
    , { "test_prContainer_strings"      , test_prContainer_strings }
    , { "test_prContainer_set_of_strings"
                                        , test_prContainer_set_of_strings }
    , { "test_printContainer_list_of_names"
                                        , test_printContainer_list_of_names }
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


// **** Helper methods ****

/*
 * **************************************************************************
 * prContainer() - Generic method to print items in an iterable container.
 *
 * Return size of container; i.e., # of elements in it, so it can be used
 * for assertion checking in tests.
 *
 * NOTE: Cannot use begin() < end() as it won't work for SETs. Need to
 *       just check that begin() != end().
 * **************************************************************************
 */
template<typename T>
size_t prContainer(const T& elements)
{
    char sep{};

    // Enclose chars and strings in '' for readability.
    if (elements.begin() != elements.end()) {
        char ch;
        string s;
        if (   (typeid(*elements.begin()) == typeid(ch))
            || (typeid(*elements.begin()) == typeid(s))) {
            sep = '\'';
        }
    }
    cout << "size=" << elements.size() << " [ ";
    for (auto pos = elements.begin(); pos != elements.end(); pos++) {
        cout << sep << *pos << sep << " ";
    }
    cout << "]\n";
    return elements.size();
}

/*
 * **************************************************************************
 * printContainer() - Print using range-based for()-loop
 *
 * Generic method to print items in an iterable container. Exactly similar to
 * prContainer() but we use a range-based for()-loop here. Under the covers,
 * this does the same thing as checking for begin(), pos != end()
 *
 * Return size of container; i.e., # of elements in it, so it can be used
 * for assertion checking in tests.
 *
 * NOTE: The iteration returns a 'const element' itself, not a reference to
 *       the element (like prContainer() does).
 *       So -NO-NEED- to dereference the `pos`. Just print `elem` directly!
 * **************************************************************************
 */
template<typename T>
size_t printContainer(const T& elements)
{
    char sep{};

    // Enclose chars and strings in '' for readability.
    if (elements.begin() != elements.end()) {
        char ch;
        string s;
        if (   (typeid(*elements.begin()) == typeid(ch))
            || (typeid(*elements.begin()) == typeid(s))) {
            sep = '\'';
        }
    }
    cout << "size=" << elements.size() << " [ ";
    for (const auto& elem : elements) {
        cout << sep << elem << sep << " ";
    }
    cout << "]\n";
    return elements.size();
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

void
test_prContainer_int_vector(void)
{
    TEST_START();

    vector<int> v{};
    auto nitems = prContainer(v);
    assert(nitems == 0);

    v.push_back(1);
    v.push_back(-1);
    v.push_back(42);
    v.push_back(-42);
    v.push_back(0);
    nitems = prContainer(v);
    assert(nitems == 5);

    TEST_END();
}

void
test_prContainer_string(void)
{
    TEST_START();

    string s{};
    auto nitems = prContainer(s);
    assert(nitems == 0);

    s = "Hello World.";
    nitems = prContainer(s);
    assert(nitems == 12);

    TEST_END();
}

void
test_prContainer_strings(void)
{
    TEST_START();

    vector<string> strings{};
    auto nitems = prContainer(strings);
    assert(nitems == 0);

    // Load 5 strings to this vector
    auto loadn = 5;
    for (auto i = 0; i < loadn; i++) {
        strings.push_back("String-" + std::to_string(i));
    }
    nitems = prContainer(strings);
    assert(nitems == loadn);

    TEST_END();
}

/*
 * Construct a SET() of strings, and print the container, using generic
 * container printing method.
 */
void
test_prContainer_set_of_strings(void)
{
    TEST_START();

    set<string> strings{ "this", "that", "and", "the", "other", "items"};

    auto nitems = prContainer(strings);
    assert(nitems == 6);

    // Add some more elements to this set.
    strings.insert("Here is a new one.");
    strings.insert("Awkward string");
    strings.insert("Just installed Eclipse, vim plugin is not working.");
    nitems = prContainer(strings);
    assert(nitems == 9);
    TEST_END();
}

/*
 * Construct a LIST() of strings, and print the container, using generic
 * container printing method. Items in a list are unordered. They appear in
 * the way they get inserted, unlike a SET{}, where they are rearranged.
 *
 * This test case also exercises some basic list() methods, for learning.
 */
void
test_printContainer_list_of_names(void)
{
    TEST_START();

    list<string> names{ "Joe Biden", "Eric Hoffmann", "Michael Schwartz"};
    auto exp_items = 3;
    auto nitems = printContainer(names);
    assert(nitems == exp_items);

    assert(names.front() == "Joe Biden");
    assert(names.back() == "Michael Schwartz");

    names.emplace_back("Papa Johns");
    exp_items++;
    nitems = printContainer(names);
    assert(nitems == exp_items);

    assert(names.back() == "Papa Johns");

    names.emplace_front("Papa Murphys");
    exp_items++;
    nitems = printContainer(names);
    assert(nitems == exp_items);

    assert(names.front() == "Papa Murphys");

    TEST_END();
}

void
test_template(void)
{
    TEST_START();

    TEST_END();
}
