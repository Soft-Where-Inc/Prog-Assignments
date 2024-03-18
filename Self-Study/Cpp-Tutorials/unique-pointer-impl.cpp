/*
 * -----------------------------------------------------------------------------
 * unique-pointer-impl.cpp: Implement UNIQUE_PTR from scratch.
 *
 * Ref:
 *  [1] Smart Pointers Explained with Code Implementation
 *      https://www.youtube.com/watch?v=gGL_Q0TpukY&t=1679s
 *
 *  [2] A Tour of C++, 2nd Edition, Bjarne Stoustrup.
 *
 * This is an excellent step-by-step tutorial on how-to build a unique_ptr
 * class from the basics, adding different types of constructors.
 *
 * Usage: g++ -o unique-pointer-impl unique-pointer-impl.cpp
 *        ./unique-pointer-impl [test_*]
 *        ././unique-pointer-impl [--help | test_<something> | test_<prefix> ]
 *
 * NOTES:
 * - Languages like Java, Python have built-in Garbage collectors, which causes
 *   performance overhead. In C++, if you use NEW(), you -must- do a DELETE()
 *   otherwise, it will cause a memory leak. NEW() allocates memory from the
 *   heap, similar to C-style malloc().
 *
 * - SMART pointers are a way to simplify the code so as a programmer you do
 *   not need to worry about a DELETE. Once the smart ptr goes out of scope
 *   C++ constructs will automatically invoke DELETE behind-the-scenes,
 *   avoiding a memory leak.
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <iostream>

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
void test_UniqueIntPtr_ctor_dtor_default(void);
void test_UniqueIntPtr_ctor_dtor_basic(void);
void test_UniquePtr_string_ctor_dtor_basic(void);
void test_UniquePtr_string_default_ctor_dtor(void);
void test_UniquePtr_string_copy_ctor(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
      { "test_this"                          , test_this }
    , { "test_that"                          , test_that }
    , { "test_UniqueIntPtr_ctor_dtor_default", test_UniqueIntPtr_ctor_dtor_default }
    , { "test_UniqueIntPtr_ctor_dtor_basic"  , test_UniqueIntPtr_ctor_dtor_basic }
    , { "test_UniquePtr_string_ctor_dtor_basic"
                                            , test_UniquePtr_string_ctor_dtor_basic }
    , { "test_UniquePtr_string_default_ctor_dtor"
                                            , test_UniquePtr_string_default_ctor_dtor }
    , { "test_UniquePtr_string_copy_ctor"   , test_UniquePtr_string_copy_ctor }
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
 * Definition of Class UniqueIntPtr(), which is identical to unique_ptr(), but
 * implemented just for basic int type.
 *
 * Class is a "resource handle" as it is managing an object for which memory
 * is allocated and the object is accessed through a pointer. [2] Sec. 5.2.1
 * *****************************************************************************
 */
class UniqueIntPtr {
  public:
    // Default constructor
    UniqueIntPtr() {
        val_ = new int(0);
        cout << __LOC__ << "Default ctor\n";
    }

    // Add constructor with user-specified value
    UniqueIntPtr(int *newint): val_(newint) {
        cout << __LOC__ << "Execute ctor\n";
    }

    // Default destructor
    ~UniqueIntPtr() {
        cout << __LOC__ << "Invoke dtor\n";
        delete val_;
    }

    int data() { return *val_; }

  private:
    int *   val_;
};

/*
 * *****************************************************************************
 * Definition of generic Class UniquePtr(), which is identical to unique_ptr(),
 * implemented for generic types using templates.
 *
 * Class is a "resource handle" as it is managing an object for which memory
 * is allocated and the object is accessed through a pointer. [2] Sec. 5.2.1
 * *****************************************************************************
 */
template<typename T>
class UniquePtr {
  public:
    // Default constructor, initializing to nullptr as default value
    // Add constructor with user-specified type and value
    UniquePtr(T *newval = nullptr): val_(newval) {
        cout << __LOC__ << "Execute "
             << ((val_ == nullptr) ? "default " : "") << "ctor\n";
    }

    // Copy constructor: Need to relinquish ownership from src - undefined
    UniquePtr(const UniquePtr<T>& src) = delete;

    // Copy assignment: Need to relinquish ownership from src - undefined
    UniquePtr& operator=(const UniquePtr<T>& src) = delete;

    // Default destructor
    ~UniquePtr() {
        if (val_) {
            cout << __LOC__ << "Invoke dtor\n";
            delete val_;
        }
    }

    // Return the value
    T data() {
        if (val_ == nullptr) {
            throw std::logic_error{__LOC__ ": Val ptr is null" };
        }
        return *val_;
    }

  private:
    T *   val_;
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


/**
 * *****************************************************************************
 * Exercise default usage of constructor & destructor of UniqueIntPtr() class,
 * and check that there is no memory leak.
 * *****************************************************************************
 */
void
test_UniqueIntPtr_ctor_dtor_default(void)
{
    TEST_START();

    int val = 42;
    UniqueIntPtr intptr = UniqueIntPtr();

    cout << "intptr.val=" << intptr.data();

    // Default constructor initializes value to 0; not to 'val'.
    assert(intptr.data() == 0);

    TEST_END();
}
/**
 * *****************************************************************************
 * Exercise basic usage of constructor & destructor of UniqueIntPtr() class,
 * and check that there is no memory leak.
 * *****************************************************************************
 */
void
test_UniqueIntPtr_ctor_dtor_basic(void)
{
    TEST_START();

    int val = 42;
    UniqueIntPtr intptr = UniqueIntPtr(new int(val));

    cout << "intptr.val=" << intptr.data();
    assert(intptr.data() == val);

    TEST_END();
}

/**
 * *****************************************************************************
 * Test UniquePtr for string type, using default-constructor for data type.
 * *****************************************************************************
 */
void
test_UniquePtr_string_default_ctor_dtor(void)
{
    TEST_START();

    UniquePtr pString = UniquePtr<string>();

    try {
        auto value = pString.data();
    } catch (std::logic_error& ex) {
        cerr << __LOC__ << "Logic exception raised: " << ex.what() << endl;
    }

    TEST_END();
}

/**
 * *****************************************************************************
 * Test UniquePtr for string type, using value-constructor.
 * *****************************************************************************
 */
void
test_UniquePtr_string_ctor_dtor_basic(void)
{
    TEST_START();

    UniquePtr pString = UniquePtr(new string("Hello"));

    auto value = pString.data();
    cout << __LOC__ << "String is: '" << value << "'";
    assert(value == "Hello");

    TEST_END();
}

/**
 * *****************************************************************************
 * Test UniquePtr for string type, using copy-constructor.
 * *****************************************************************************
 */
void
test_UniquePtr_string_copy_ctor(void)
{
    TEST_START();

    UniquePtr pString = UniquePtr(new string("COPY constructor unsupported!"));

    // COPY-constructor is unsupported for this class as the semantics of COPY
    // followed by relinquishing ownership from 'src' is undefined.
    // This will raise a compilation error.
    // UniquePtr pString2 = UniquePtr(pString);

    // UniquePtr pString2 = pString;

    cout << pString.data();

    TEST_END();
}

void
test_template(void)
{
    TEST_START();

    TEST_END();
}
