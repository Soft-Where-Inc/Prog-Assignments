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

const int Num_items = (100 * 1000);   // 100K

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

      // Find # of inversions in input array loaded by the load() method.
      // The input data will be sorted in-place upon return.
      int
      numInversions()
      {
         if (nelements <= 1) {
            return 0;
         }
         if (nelements == 2) {
            return numInvBase(0, nelements);
         }

         cout << __func__ << ": start=0" << ", nelements=" << nelements << endl;
         int rv = 0;
         int nitems_lo = (nelements / 2);

         // Recursive on left / right; sorting each half to count # inversions.
         rv += numInvSort(0, nitems_lo);

         auto nitems_hi = (nelements - nitems_lo);
         rv += numInvSort(nitems_lo, nitems_hi);

         // Merge the two sorted sub-lists
         rv += numInvMerge(0, nitems_lo, nitems_lo, nitems_hi);
         return rv;
      }

      // Verify that the contents are in sorted order
      bool
      verify(void)
      {
         for (auto ictr = 0; ictr < (nelements - 1); ictr++) {
            if (numbers[ictr] > numbers[ictr + 1]) {
                return false;
            }
         }
         return true;
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
      int numbers[Num_items] = {0};

      // Given a [sub-]array of 'n' items, implement merge-sort to count # of
      // inversions in this set starting at index 'start', with 'nitems' in the
      // set. Input data will be sorted upon return.
      int
      numInvSort(int start, int nitems)
      {
         if (nitems <= 1) {
            return 0;
         }
         if (nitems == 2) {
            return numInvBase(start, nitems);
         }

         // cout << __func__ << ": start=" << start << ", nitems=" << nitems << endl;
         int rv = 0;
         int nitems_lo = (nitems / 2);    // Same as # items in low-list, nitems_lo
         rv += numInvSort(start, nitems_lo);

         auto nitems_hi = (nitems - nitems_lo);
         rv += numInvSort((start + nitems_lo), nitems_hi);

         rv += numInvMerge(start, (start + nitems_lo), nitems_lo, nitems_hi);
         return rv;
      }

      // -----------------------------------------------------------------------
      // Implement merge-sort of 2 sub-lists, each of nitems length.
      // Count and return # of inversions found. Sort in-place.
      // 'lo', 'hi' are start indexes of lower / higher sub-list
      // 'nitems_lo' is # of items in lower  'lo' sub-list.
      // 'nitems_hi' is # of items in higher 'hi' sub-list.
      int
      numInvMerge(const int lo, const int hi, const int nitems_lo, const int nitems_hi)
      {
         assert((lo + nitems_lo) == hi);
         // Deal with simple case when lower-list is <= higher-list
         if (numbers[lo + nitems_lo - 1] <= numbers[hi]) {
            return 0;
         }

         // Deal with reverse case when all items in sorted upper-half are <= all
         // items in sorted lower-half
         if (   (nitems_lo == nitems_hi)
             && (numbers[hi + nitems_lo - 1] <= numbers[lo])) {
            // Flip both sub-halves, in-place.
            for (auto ictr = 0; ictr < nitems_lo; ictr++) {
               swap(lo + ictr, hi + ictr);
            }
            return (nitems_lo * nitems_lo);
         }

         // Merge the two sub-lists, counting # inversions along the way
         int rv = 0;
         int *lop = &numbers[lo];
         int *hip = &numbers[hi];

         // Process all items in lower-half. When this loop terminates, lop will
         // be pointing to 1st element -after- end of lower-half; I.e., it will
         // be positioned exactly at the 1st element of the upper-half.
         auto nlower_half_items = nitems_lo;
         while (nlower_half_items) {
            if (*lop <= *hip) {
               lop++;
               nlower_half_items--;
            } else {
               // Value in upper-half is < value in lower-half. Flip them
               auto tmp = *lop;
               *lop = *hip;
               *hip = tmp;
               lop++;
               hip++;
               nlower_half_items--;
               rv++;
            }
         }

         // Verify loop termination condition
         assert(lop == &(numbers[hi]));

         // In case input # of elements was odd, upper-half should have one more
         // item. Verify that and deal with remaining singleton item.
         if (nitems_lo != nitems_hi) {
            // cout << "nitems_lo=" << nitems_lo << ", nitems_hi=" << nitems_hi << endl;
            assert(nitems_hi == (nitems_lo + 1));

            auto lastp = &numbers[hi + nitems_hi - 1];

            // If prev-item is larger than current-item, flip the two items.
            while ((lastp > lop) && (*(lastp - 1) > *lastp)) {
                auto tmp = *lastp;
                *lastp = *(lastp - 1);
                *(lastp - 1) = tmp;
                rv++;
                lastp--;
            }
         }

         return rv;
      }

      // Implement the base case: Assert(n==2);
      int
      numInvBase(int start, int nitems)
      {
         assert(nitems == 2);

         int rv = 0;
         // Flip the pair if needed, and count 1 inversion.
         if (numbers[start] > numbers[start + 1]) {
            swap(start, start + 1);
            rv = 1;
         }
         return rv;
      }

      // Swap i'th item with j'th item in numbers[] array
      void
      swap(int i, int j) {
        assert(i < nelements);
        assert(j < nelements);

        auto tmp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = tmp;
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

   int nInvFound = data.numInversions();
   cout << "# of inversions found: " << nInvFound << endl;
   if (!data.verify()) {
       cout << "Error! Output array is unsorted: " << endl;
       data.dump();
       return 1;
    }
    return 0;
}
