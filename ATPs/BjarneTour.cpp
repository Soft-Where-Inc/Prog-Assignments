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

constexpr double ce_square(double x) { return(x * x); }

void do_auto_print(void);

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
 }

/*
 * Example of for()-loop variables w/o declarating type of counter.
 * Iterate thru list by enumeration.
 */
void
do_auto_print(void)
{
   int intarray[] = {10, 21, 32, 43, 54};

   cout << "Auto-print contents of array, vector etc. " << endl;
   for (auto i:intarray) {
      cout << i << " ";
   }
   cout << endl;

   vector<int> intvec = {6, 7, 8, 9, 10};
   for (auto intval:intvec) {
      cout << intval << " ";
   }
   cout << endl;
}

