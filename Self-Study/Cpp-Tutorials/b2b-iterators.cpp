/*
 * -----------------------------------------------------------------------------
 * b2b-iterators.cpp: Back to Basics C++ Iterators
 * Talk by Nicolai Josuttis - CppCon 2023
 * -----------------------------------------------------------------------------
 *
 * Ref:
 *  [1] https://www.youtube.com/watch?v=26aW6aBVpk0&t=394s
 *
 *  [2] Good discussion of support for std::format() and alternatives.
 *      https://stackoverflow.com/questions/63724059/does-gcc-support-c20-stdformat
 *
 *  [3] A Tour of C++, Bjarne Stroustrup, 2nd Ed.
 *
 *  [4] https://devblogs.microsoft.com/cppblog/documentation-for-cpp20-ranges/
 *
 *  [5] https://learn.microsoft.com/en-gb/cpp/standard-library/ranges?view=msvc-170
 *
 *  [6] https://learn.microsoft.com/en-us/cpp/standard-library/range-adaptors?view=msvc-170
 *  	RESOLVE: Also see examples on range-adaptors from above article.
 *
 * Usage: g++ -std=c++20 -o b2b-iterators b2b-iterators.cpp
 *        ./b2b-iterators [test_*]
 *        ./b2b-iterators [--help | test_<something> | test_<prefix> ]
 *
 * On Mac, to detect memory leaks, do: $ leaks -atExit -- ./b2b-iterators ...
 *
 * ----------------------------------------------------------------------------
 * Key-Points: ---- Iterators work through "Collections" ----
 * ----------------------------------------------------------------------------
 *
 *  - Iterators through [begin(), end() ) - work on a half-open-range, where
 *    end() is "past-the-last-item-in-the-vector".
 *
 * Good summary of various iterators in C++20:
 * [1] From slide no. 21, ~27.42 into the talk.
 * [3] See also Sec 11.6. Container Overview, pg. 146 ]
 *
 * - Contiguous range/iterator:
 *      - Can jump to and compare with any other position.
 *      - Support: =, *, ++, ==, !=, --, +=, -=, <, <=, ... [], -
 *      - Iterator may be raw pointer, range has std::ranges::data() accessor.
 *
 * - Random access range/iterator: (Almost similar to Contiguous range/iterator)
 *      - Can jump to and compare with any other position.
 *      - Support: =, *, ++, ==, !=, --, +=, -=, <, <=, ... [], -
 *      - Only supported for deque<T> (Internally maintained as array of arrays.)
 *      - So, can do deque_items[1000].
 *
 * - Bidirectional range/iterator:
 *      - Can jump forward and backward.
 *      - Support: =, *, ++, ==, !=, --
 *      - list<>, associative containers (set<T>, map<K,V>, multimap<K,V>)
 *
 * - Forward range/iterator:
 *      - Can iterate forward multiple times.
 *      - Support: =, *, ++, ==, !=
 *      - forward_list<>, unordered containers (hash tables, unordered_map<K,V>,
 *        unordered_multimap<K,V>, unordered_set<T>, unordered_multiset<T>
 *
 * -- Key Points on Ranges from [4]
 *
 * A range is represented by an iterator that marks the beginning of the
 * range, and a sentinel that marks the end of the range. The sentinel may
 * be the same type as the begin iterator, or it may be different, which lets
 * Ranges support operations which simple iterator pairs can’t.
 *
 * -- Generic programming, using templates (no need to be OO-always!)
 *    See test_accumulate_doubles().
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>         // std::cout()
#include <vector>
#include <set>
#include <list>
#include <array>
#include <deque>
#include <map>
#include <unordered_map>
#include <algorithm>
// #include <experimental/ranges>
#include <numeric>          // std::accumulate()

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
void test_prRandomAccessContainer(void);
void test_prBiDirIterateBackwards(void);
void test_prContainerIterateForwards(void);
void test_sort(void);
void test_const_iterators(void);
void test_accumulate_doubles(void);
void test_vector_size_capacity_gotcha(void);
void test_memory_allocation();

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
    , { "test_prRandomAccessContainer"  , test_prRandomAccessContainer }
    , { "test_prBiDirIterateBackwards"  , test_prBiDirIterateBackwards }
    , { "test_prContainerIterateForwards"
                                        , test_prContainerIterateForwards }
    , { "test_sort"                     , test_sort }
    , { "test_const_iterators"          , test_const_iterators }
    , { "test_accumulate_doubles"       , test_accumulate_doubles }
    , { "test_vector_size_capacity_gotcha"
                                        , test_vector_size_capacity_gotcha }
    , { "test_memory_allocation"        , test_memory_allocation }
};

// Test start / end info-msg macros
#define TEST_START()  cout << __func__ << ": "
#define TEST_END()    cout << " ...OK" << endl

// Fabricate a string to track code-location of call-site.
#define __LOC__ \
    "[" + std::string{__func__} + "():" + std::to_string(__LINE__) + "] "

/**
 * Some class to play around with objects, memory allocation etc.
 */
class Person
{
  public:
    Person() {};

    Person(uint32_t age, string first_name, string last_name) {
        age_ = age;
        first_name_ = first_name;
        last_name_ = last_name;
    }

  private:
    uint32_t    age_;
    string      first_name_;
    string      last_name_;

    friend ostream& operator<<(ostream& os, const Person& person) {
        os << "[Name=" << person.first_name_
           << " " << person.last_name_
           << ", Age=" << person.age_ << "]";
        return os;
    }
};

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
 * -----------------------------------------------------------------------------
 * Function to overload << operator to generically print all elements in a
 * pair. Uses template of generic typenames 'T1' and 'T2'.
 *
 * Note: The arg name 'vector' is allowed even though 'vector' is a keyword.
 *
 * Ref:  https://www.geeksforgeeks.org/different-ways-to-print-elements-of-vector/
 * -----------------------------------------------------------------------------
 */
template <typename T1, typename T2>
ostream& operator<<(ostream& os, const pair<T1,T2>& apair)
{
    char f_sqc{}; // First's  single-quote character
    char s_sqc{}; // Second's single-quote character

    char ch;
    string s;
    if (   (typeid(apair.first) == typeid(ch))
        || (typeid(apair.first) == typeid(s))) {
        f_sqc = '\'';
    }
    if (   (typeid(apair.second) == typeid(ch))
        || (typeid(apair.second) == typeid(s))) {
        s_sqc = '\'';
    }

    // Printing the pair of items using <<
    os << "(" << f_sqc << apair.first  << f_sqc
       << "," << s_sqc << apair.second << s_sqc
       << ")";
    return os;
}

/*
 * **************************************************************************
 * Simple function to check if a given address is from stack / heap.
 *
 * As a refresher, here's a pictorial of how memory layouts can be for a
 * stack based execution. On most h/w stack addresses go from high to low mem.
 *
 * In fig. below, stkf1_a & stk2_b are variables declared on-stack in
 * adjacent stack frames. On the left is an example of the stack frames
 * going from high to low addresses, while on the right is an example showing
 * stack frames going from low to high addresses.
 *
 *                              (    (&stkf1_a < &stkf2_b)
 *                               && ((&stkf1_a + GUARDSIZE) > &stkf2_b) )
 *     low──►                         low──►
 *           ▲                             │
 *           │                             │
 *           │                             │
 *           ├ ─ ─ ──[Stack Guard Size]─ ─ ┤
 *           │                             │
 *           │   stkf2_b           stkf1_a │
 *           │                             │
 *           ├───────[Frame boundary]──────┤
 *           │                             │
 *           │   stkf1_a           stkf2_b │
 *           │                             │
 *           │                             │
 *           ├ ─ ─ ──[Stack Guard Size]─ ─ ┤
 *           │                             │
 *           │                             │
 *           │                             ▼
 *     high──►                       high──►
 *
 *   (    (&stkf1_a > &stkf2_b)
 *    && ((&stkf1_a - GUARDSIZE) < &stkf2_b) )
 *
 * In code below, (&stkf1_a is addr) and (&stkf2_b is &dummy1)
 * **************************************************************************
 */
bool
addrOnStack(const char * const addr)
{
    // Define a local variable and check if its address matches the input addr
    char dummy1;
    char dummy2;

    // True => fig on the right; False => fig on the left (common case in h/w)
    bool stack_is_growing_up_in_addr = (&dummy2 > &dummy1);
    constexpr auto stack_guard = (1024 * 1024); // bytes

    printf(" [stack_is_growing_up_in_addr=%d, addr=%p, &dummy1=%p ]",
            stack_is_growing_up_in_addr, addr, &dummy1);
    if (stack_is_growing_up_in_addr) {
        return ((addr < &dummy1) && (addr + stack_guard > &dummy1));
    } else {
        return ((addr > &dummy1) && ((addr - stack_guard) < &dummy1));
    }
}

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
    char sqc{}; // single-quote character

    // Enclose chars and strings in '' for readability.
    if (elements.begin() != elements.end()) {
        char ch;
        string s;
        if (   (typeid(*elements.begin()) == typeid(ch))
            || (typeid(*elements.begin()) == typeid(s))) {
            sqc = '\'';
        }
    }
    cout << "size=" << elements.size() << " [ ";
    for (auto pos = elements.begin(); pos != elements.end(); pos++) {
        cout << sqc << *pos << sqc << " ";
    }
    cout << "]\n";
    return elements.size();
}

/*
 * **************************************************************************
 * prContainerIterateForwards() - Generic method to print items in an iterable
 * container that only supports iterating forward. E.g. singly-linked lists,
 * hash tables.
 */
template<typename T>
size_t prContainerIterateForwards(const T& elements)
{
    return prContainer(elements);
}

/*
 * **************************************************************************
 * prRandomAccessContainer() - Generic method to print items in an iterable
 *  container, that can be randomly accessed.
 *
 * Vector, Deque (double-ended queue), arrays qualify as such containers.
 *
 * Return size of container; i.e., # of elements in it, so it can be used
 * for assertion checking in tests.
 *
 * NOTE: We go back to using (begin() < end()) which -ONLY- works for those
 *       containers whose elements are laid out sequentially
 * **************************************************************************
 */
template<typename T>
size_t prRandomAccessContainer(const T& elements)
{
    char sqc{}; // single-quote character

    // Enclose chars and strings in '' for readability.
    if (elements.begin() < elements.end()) {    // NOTE: Use of < check.
        char ch;
        string s;
        if (   (typeid(*elements.begin()) == typeid(ch))
            || (typeid(*elements.begin()) == typeid(s))) {
            sqc = '\'';
        }
    }
    cout << "size=" << elements.size() << " [ ";
    for (auto pos = elements.begin(); pos < elements.end(); pos++) {
        cout << sqc << *pos << sqc << " ";
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
/*
 * NOTE: With C++17 and prior, you would define this function using templates.
 *
template<typename T>
size_t printContainer(const T& elements)
*/
// NOTE: C++20 supports syntactic sugar to define without templates.
size_t printContainer(const auto& elements)
{
    char sqc{}; // single-quote character

    // Enclose chars and strings in '' for readability.
    if (elements.begin() != elements.end()) {
        char ch;
        string s;
        if (   (typeid(*elements.begin()) == typeid(ch))
            || (typeid(*elements.begin()) == typeid(s))) {
            sqc = '\'';
        }
    }
    cout << "size=" << elements.size() << " [ ";
    for (const auto& elem : elements) {
        cout << sqc << elem << sqc << " ";
    }
    cout << "]\n";
    return elements.size();
}

/*
 * **************************************************************************
 * prBiDirIterateBackwards() - Generic method to print items in a container
 * that supports bi-directional access. Print elements in backwards order.
 *
 * Return size of container; i.e., # of elements in it, so it can be used
 * for assertion checking in tests.
 *
 * NOTE: Cannot use begin() < end() as it won't work for LISTs. Need to
 *       just check that begin() != end().
 * **************************************************************************
 */
template<typename T>
size_t prBiDirIterateBackwards(const T& elements)
{
    char sqc{}; // single-quote character

    // Enclose chars and strings in '' for readability.
    if (elements.begin() != elements.end()) {
        char ch;
        string s;
        if (   (typeid(*elements.begin()) == typeid(ch))
            || (typeid(*elements.begin()) == typeid(s))) {
            sqc = '\'';
        }
    }
    cout << "size=" << elements.size() << " [ ";

    // Iterate starting from the end, but 'end' is not a valid item.
    auto pos = elements.end();
    do {
        --pos;
        cout << sqc << *pos << sqc << " ";
    } while (pos != elements.begin());
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

/*
 * Test print method for containers whose elements are laid out sequentially.
 *  Vector, Array, Deque ..., Strings ...
 */
void
test_prRandomAccessContainer(void)
{
    TEST_START();

    constexpr auto array_size = 4;

    // --- Test out arrays ----
    cout << endl << "Test arrays:" << endl;

    // Have to specify size of array when declaring it.
    array<float, array_size> floats{1.234, 3.14, -42.42, 88.88};
    auto nitems = prRandomAccessContainer(floats);
    assert(nitems == array_size);

    // RESOLVE: Why does this fail?
    // assert(floats[1] == 3.14);

    // Based on [3], pg. 141, Sec. 11.2.2 Use of Vec[] with bounds checking
    try {
        assert(floats.at(nitems) == 99);
    } catch (out_of_range&) {
        cerr << __LOC__
             << "Out-of-range exception raised at index=" << nitems
             << ", array size=" << floats.size()
             << endl;
    }

    // --- Test out Deque's ----
    cout << endl << "Test deques (Double-ended queues):" << endl;

    auto exp_items = 0;
    deque<int> dq_ints;
    assert(dq_ints.size() == exp_items);

    auto index{-1};
    try {
		assert(dq_ints.at(index) == 99);
    } catch (out_of_range&) {
    	cerr << __LOC__
             << "Out-of-range exception raised at index=" << index
             << ", double-ended queue size=" << dq_ints.size()
             << endl;
    }

    dq_ints.push_front(42);
    exp_items++;
    nitems = prRandomAccessContainer(dq_ints);
    assert(nitems == exp_items);

    dq_ints.push_front(41);
    exp_items++;
    nitems = prRandomAccessContainer(dq_ints);
    assert(nitems == exp_items);

    dq_ints.push_back(51);
    exp_items++;
    nitems = prRandomAccessContainer(dq_ints);
    assert(nitems == exp_items);

    // Exercise interfaces to verify the data we loaded
    assert(nitems == dq_ints.size());
    assert(dq_ints.front() == 41);
    assert(dq_ints.back() == 51);

    TEST_END();
}

/*
 * Test print methods on bi-directional iterators. Print elements in backwards
 * scan order, to show that the -- iteration is working.
 *
 * list<>, Associative containers, like map<>, set<> ... qualify)
 */
void
test_prBiDirIterateBackwards(void)
{
    TEST_START();

    cout << endl;

    auto exp_items = 6;
    set<string> strings{ "this", "that", "and", "the", "other", "items"};

    // Note that the lists should be printed in exactly reverse order.
    prContainer(strings);

    auto nitems = prBiDirIterateBackwards(strings);
    assert(nitems == exp_items);

    // ---- Test with map: Key-value pair

    map<string, int> kv_map{  {"one"	, 1}
                            , {"two"	, 2}
                            , {"three"	, 3}
                            , {"four"	, 4}
                            , {"five"	, 5}
                            , {"six"	, 6}
                            , {"seven"	, 7}
                            , {"eight"	, 8}
                            , {"nine"	, 9}
                            , {"ten"	, 10}
                            };

    cout << "Map{}: ";
    exp_items = 10;
    nitems = prBiDirIterateBackwards(kv_map);
    assert(nitems == exp_items);

    // ---- Test with a multi-map: Key-value pair, with duplicate keys

    multimap<string, int> multi_kv_map{  {"one"     , 1}
                                       , {"two"     , 2}
                                       , {"three"   , 3}
                                       , {"four"    , 4}
                                       , {"five"    , 5}
                                       , {"two"     , 22}
                                       , {"six"     , 6}
                                       , {"seven"   , 7}
                                       , {"eight"   , 8}
                                       , {"nine"    , 9}
                                       , {"seven"   , 77}
                                       , {"ten"     , 10}
                                };

    cout << "Multi-map{}: ";
    exp_items = 12;
    nitems = prBiDirIterateBackwards(multi_kv_map);
    assert(nitems == exp_items);

    // ---- Test with an unordered map: Unordered Key-value pair

    unordered_map<string, int> ukv_map{  {"one"     , 1}
                                       , {"two"     , 2}
                                       , {"three"   , 3}
                                       , {"four"    , 4}
                                       , {"five"    , 5}
                                       , {"six"     , 6}
                                       , {"seven"   , 7}
                                       , {"eight"   , 8}
                                       , {"nine"    , 9}
                                       , {"ten"     , 10}
                                };

    exp_items = 10;

    // Unordered maps are stored as hash tables, which do not support moving
    // reverse in the iteration. Hence, following call will result in a
    // compiler error.
    // nitems = prBiDirIterateBackwards(ukv_map);

    cout << "Unordered-map{}: ";
    // Print using usual generic container print method.
    nitems = prContainer(ukv_map);
    assert(nitems == exp_items);

    TEST_END();
}

void
test_prContainerIterateForwards(void)
{
    TEST_START();

    // ---- Test with an unordered map: Unordered Key-value pair
    unordered_map<string, int> ukv_map{  {"one"     , 1}
                                       , {"two"     , 2}
                                       , {"three"   , 3}
                                       , {"four"    , 4}
                                       , {"five"    , 5}
                                       , {"six"     , 6}
                                       , {"seven"   , 7}
                                       , {"eight"   , 8}
                                       , {"nine"    , 9}
                                       , {"ten"     , 10}
                                };
    auto exp_items = 10;

    // Unordered maps are stored as hash tables, which do not support moving
    // reverse in the iteration. Hence, following call will result in a
    // compiler error.
    // nitems = prBiDirIterateBackwards(ukv_map);

    cout << "Unordered-map{}: ";
    auto nitems = prContainerIterateForwards(ukv_map);
    assert(nitems == exp_items);

    TEST_END();
}

/*
 * 	**** Test cases for different C++ STL Algorithms ****
 */

// Basic sort() algorithm.
void
test_sort(void)
{
    TEST_START();

    vector<int> v{0, -93, 42, 22, 16, 2000};
    std::sort(v.begin(), v.end());

    cout << endl << "Sorted vector<int>:";
    prContainer(v);

    vector<string> strings{ "this", "that", "and", "the", "other", "items"};
    std::sort(strings.begin(), strings.end());

    cout << endl << "Sorted vector<string>:";
    prContainer(strings);

    TEST_END();
}

/*
 * Basic example of using std::range() to specify begin() / end() of vector to sort().
 */
/*
void
test_sort_range_basic(void)
{
    TEST_START();

    vector<int> v{0, -93, 42, 22, 16, 2000};
    std::ranges::sort(v);

    cout << endl << "Sorted vector<int>:";
    prContainer(v);

    TEST_END();
}
*/

void
test_const_iterators(void)
{
    TEST_START();

    vector<int> v{0, -93, 42, 22, 16, 2000};

    /*
     * Use normal iterator and increment each item.
     * BUT -BE-careful about pos++ (which increments iterator) v/s *pos++, which
     * only dereference member, but will again only increment iterator.
     * You need to do: (*pos)++ to increment the value itself!
     */
    for (auto pos = v.begin(); pos != v.end(); pos++) {
        (*pos)++;
    }
    prContainer(v);

    // Will fail with compilation error, due to use of constant iterator.
    // for (auto pos = v.cbegin(); pos < v.cend(); pos++) { (*pos)++; }

    TEST_END();
}

/*
 * Test use of std::accumulate() to do some aggregate SUM() across a range
 * of iterable items.
 */
void
test_accumulate_doubles(void)
{
    TEST_START();

    vector<double> vd{-20.22, -33.33, -5000, 42, 40.40, 16.16, 2000};
    prContainer(vd);

    // Find SUM() items in entire vector
    auto vd_sum = 0.0;
    vd_sum = std::accumulate(vd.begin(), vd.end(), vd_sum);
    cout << "Sum=" << vd_sum;

    // Find SUM() items in entire vector, w/o including far outliers on each end
    vd_sum = 0.0;
    std::sort(vd.begin(), vd.end());
    vd_sum = std::accumulate(vd.begin() + 1, vd.end() - 1, vd_sum);
    cout << "  Sum=" << vd_sum << " (without including outliers)" << endl;

    TEST_END();
}

/*
 * Test behaviour that vectors may get re-sized upon new insert which will
 * throw-off existing position location iterators.
 */
void
test_vector_size_capacity_gotcha(void)
{
    TEST_START();

    size_t vcap = 5;

    vector<double> vd;  // {-20.22, -33.33, -5000, 42, 40.40, 16.16, 2000};

    // You can reserve a capacity by this interface
    vd.reserve(vcap);
    cout << endl << "Vector vd size=" << vd.size() << ", capacity=" << vd.capacity();

    vd.push_back(-20.22);
    vd.push_back(-33.33);
    vd.push_back(-5000);
    vd.push_back(-42);
    vd.push_back(42);
    cout << endl << "Vector vd size=" << vd.size() << ", capacity=" << vd.capacity();

    auto found42 = std::find(vd.begin(), vd.end(), 42);
    cout << endl << "found42=" << *found42 << endl;

    size_t old_index = (found42 - vd.begin());

    vd.push_back(-42);
    vd.push_back(-41);
    vd.push_back(-40);
    cout << endl << "Vector vd size=" << vd.size() << ", capacity=" << vd.capacity();
    cout << endl << "found42=" << *found42 << endl;

    // Attempt to use the stale `found42` iterator to walk to end of vector
    // You will likely see garbage being printed.
    // If you don't have the check for "old_index < vd.size()", this loop will
    // never terminate, as vd.end() has completely changed to an end-iterator
    // position, in newly allocated memory, whereas `found42` is an iterator in
    // the old vector's memory.
    for (auto pos = found42; (pos != vd.end() && old_index < vd.size());
         pos++, old_index++) {
        cout << "new Item=" << *pos << endl;
    }

    TEST_END();
}

/**
 * Simple test case to verify how memory allocation works by examining addresses
 * of objects declared on-stack and on the heap.
 */
void
test_memory_allocation(void)
{
    TEST_START();

    int i_on_stack;
    int j_on_stack;

    bool addr_is_on_stack = addrOnStack((const char *) &i_on_stack);
    assert(addr_is_on_stack == true);

    cout << endl << "addr of i=" << &i_on_stack << ", i=" << i_on_stack
                 << ", onstack=" << addr_is_on_stack;

    cout << endl << "addr of j=" << &j_on_stack << ", j=" << j_on_stack;

    Person emplA(42, "Jerry", "Brown");
    cout << endl << "addr of empA=" << &emplA << ", size=" << sizeof(emplA)
                 // << ", onstack=" << addr_is_on_stack
                 << " " << emplA;

    int diff_addr = (int) ((size_t) &i_on_stack - (size_t) &j_on_stack);
    cout << endl << "diff (&j - &i)=" << diff_addr << " bytes";

    diff_addr = (int) ((size_t) &j_on_stack - (size_t) &emplA);
    cout << endl << "diff (&e - &j)=" << diff_addr << " bytes";

    // Use the NEW operator to dynamically allocate a Person object,
    // which returns a pointer to the allocated memory.
    Person * emplNew = new Person(44, "New", "Employee");
    addr_is_on_stack = addrOnStack((const char *) emplNew);
    assert(addr_is_on_stack == false);

    cout << endl << "addr of empNew=" << emplNew
                 << ", size=" << sizeof(*emplNew)
                 << ", onstack=" << addr_is_on_stack
                 << " " << *emplNew;

    delete emplNew; // Otherwise, there will be a memory leak
    TEST_END();
}

void
test_template(void)
{
    TEST_START();

    TEST_END();
}
