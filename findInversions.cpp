/*
 * Stanford Coursera class on Algorithms:
 *
 * This file contains all of the 100,000 integers between 1 and 100,000
 * (inclusive) in some order, with no integer repeated.
 *
 * Your task is to compute the number of inversions in the file given,
 * where the i'th row of the file indicates the i'th entry of an array.
 *
 * Because of the large size of this array, you should implement the
 * fast divide-and-conquer algorithm covered in the video lectures.
 *
 * The numeric answer for the given input file should be typed in the
 * space below.
 *
 * History:
 *   5.Mar.2022   - At Stanford Old Union building
 */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define NUM_ITEMS (100 * 1000)   // 100K

class Inversions
{
   public:
      // Load data items from input file
      void
      load(const char *filename)
      {
         ifstream inpfile(filename);
         if (!inpfile.is_open()) {
            cout << "Unable to open input file: '" << filename << "'\n";
            return;
         }

         // Read till eof contents into int array.
         while (inpfile >> numbers[nelements]) {
            nelements++;
         }

         inpfile.close();
         cout << "Read " << nelements << " ints from input file " << filename << endl;
      }

      // Find # of inversions in input array
      int
      numInversions()
      {
         if (nelements <= 1) {
            return 0;
         }
         if (nelements == 2) {
            return numInvBase(0, nelements);
         }
         return -1;
      }

      // Debug: Dump contents of input array
      void
      dump(void)
      {
         cout << nelements << " ints loaded" << endl;
         for (auto ictr = 0; ictr < nelements; ictr++) {
            cout << "[" << ictr << "]: " << numbers[ictr] << endl;
         }
      }
   private:
      int nelements = 0;
      int numbers[NUM_ITEMS] = {0};

      // Implement the base case: Assert(n==2);
      int
      numInvBase(int start, int n)
      {
         int rv = 0;
         // Flip the pair if needed, and count 1 inversion.
         if (numbers[start] > numbers[start + 1]) {
            auto tmp = numbers[start];
            numbers[start] = numbers[start + 1];
            numbers[start + 1] = tmp;
            rv = 1;
         }
         return rv;
      }
};

int
main(int argc, const char *argv[])
{
   std::cout << "Hello World! argc=" << argc << "\n";
   if (argc <= 1) {
      return 0;
   }
   Inversions data;
   data.load(argv[1]);
   data.dump();

   cout << "# of inversions found: " << data.numInversions() << endl;
   data.dump();
}
