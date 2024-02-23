/*
 * ProgAssignments/ATPs/BjarneTour.cpp:
 *
 * Exercises from Bjarne Stroustrup's Tour of C++ book, to get myself
 * warmed up on C++ idioms.
 *
 * History:
 *   12.Feb.2023 - Started at Green's library Stanford Univ.
 */

#include <iostream>
#include <vector>
// #include <string>

using namespace std;

// Prototypes:

// Two functions overloaded with diff signatures.
double square(double x);

double square(const char *x);

void
print_cmdline_args(vector<string> &argv);

// Function defined as a constant expression; Evaluated at compile-time.
constexpr double ce_square(double x) { return(x * x); }

void do_auto_print(void);
void do_incr(void);
void do_incr2(void);
void do_incr(vector<int>& items);

// Read-only; i.e. 'const' version of prev method.
void do_incr_ro(const vector<int>& items);

void test_null_ptr(void);

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

int
main(int argc, char *argv[])
{
   cout << "Hello World. argc=" << argc << ":" << argv[0]
        << endl;
   if (argc > 1) {
      double d = atof(argv[1]);
      cout << "Square of " << argv[1] << " is: " << square(d)
           << endl;
   }

   // -----
   // dstr is a variable whose type is deduced by r.h.s.
   // constexpr: 'dstr' will be a compile-time constant.
   constexpr auto dstr = "3.33";
   double dsquare = square(dstr);
   cout << "constexpr dstr: Square of " << dstr << " is: " << dsquare
        << endl;

   // ---- constexpr
   constexpr double dsq = ce_square(25.5);
   cout << "constexpr: double val=" << 25.5 << ", dsq=" << dsq << endl;

   // -----
   // Use {} for to initialize variable, to catch compile time errors.
   // 'i' will be truncated; 'd' will not be.
   // Prefer initializing using {} for variables declared with named type
   int i = 7.3;
   double d {7.3};
   cout << "int d using {}: i=" << i << ", d=" << d << endl;

   // -----
   // Same exercise using auto keyword
   // 'j' will be truncated; 'dd' will not be.
   auto dval = 8.3;
   int j = dval;
   auto dd {dval};   // dd's type is auto-determined from that of dval
   cout << "auto init using {}: j=" << j << ", dd=" << dd << endl;

   // -----
   // Extract params is an example of a method whose prototype is defined using
   // Vector and String classes, to extract command-line parameters.
   vector<string> argv_vector(argc);

   // -----
   // Convert array of char * args into vector of cmdline string values
   for (auto i = 0; i < argc; i++) {
      argv_vector[i] = argv[i];
   }
   print_cmdline_args(argv_vector);

   // ----
   do_auto_print();
   do_incr();
   do_incr2();

   test_null_ptr();
}

double
square(double x)
{
   return(x * x);
}

double
square(const char *x)
{
   double xval = atof(x);
   return(xval * xval);
}

/*
 * Example of a function defined to use vector of string values as its argument.
 */
 void
 print_cmdline_args(vector<string> &argv)
 {
   cout << "Print " << argv.size() << " command-line arguments" << endl;
   for (auto i = 0; i < argv.size(); i++) {
      cout << "Arg[" << i << "] argv: '" << argv[i] << "'" << endl;
   }
   cout << "Print " << argv.size() << " arguments using enumeration: ";
   for (auto arg : argv) {
      cout << arg << " ";
   }
   cout << endl;
 }

/*
 * Example of for()-loop variables w/o declarating type of counter.
 * Shows how-to iterate thru a list by enumeration: auto <var> : list
 */
void
do_auto_print(void)
{
   int intarray[] = {10, 21, 32, 43, 54};

   cout << __func__ << ": contents of array, vector etc. " << endl;
   for (auto i : intarray) {
      cout << i << " ";
   }
   cout << endl;

   vector<int> intvec = {6, 7, 8, 9, 10};
   for (auto intval : intvec) {
      cout << intval << " ";
   }
   cout << endl;
}

/*
 * In-place increment a vector of ints, using a reference to element.
 * Shows how-to print a vector using << overloading method.
 */
void
do_incr(void)
{
   vector<int> intvec = {6, 7, 8, 9, 10};

   cout << __func__ << ":Initial vector        : " << intvec << endl;
   for (auto& intref : intvec) {
      intref++;
   }

   cout << __func__ << ":Vector after increment: " << intvec << endl;
}

/*
 * Same as do_incr(), but we call a sub-method which receives an
 * int-vector, by reference, to do the in-place increment.
 */
void
do_incr2(void)
{
   vector<int> intvec = {16, 17, 18, 19, 20};

   cout << __func__ << ":Initial vector        : " << intvec << endl;

   do_incr(intvec);

   cout << __func__ << ":Vector after increment: " << intvec << endl;

   do_incr_ro(intvec);
}

/*
 * Workhorse method that receives a vector by reference. And increments each
 * item in the vector.
 */
void
do_incr(vector<int>& items)
{
   for (auto& item : items) { item++; }
}

void
do_incr_ro(const vector<int>& items)
{
   // Disallowed as 'items' is a reference to a 'const' vector.
   // for (auto& item : items) { item++; }

   cout << __func__ << ": Can only print contents: " << items << endl;
}

/*
 * Function to exercise use of 'nullptr' keyword to handle NULL ptrs.
 */
void
test_null_ptr()
{
   int   i;
   int  *ip = &i;
   int  *iNullp = nullptr;

   cout << __func__ << ": ip=" << ip
        << " is " << ((ip == nullptr) ? "" : "not")
        << " NULL ptr."
        << endl;

   cout << __func__ << ": iNullp=" << iNullp
        << " is " << ((iNullp == nullptr) ? "" : "not")
        << " NULL ptr."
        << endl;
}
