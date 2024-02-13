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
#include <random>

using namespace std;

const int Num_items = (100 * 1000);   // 100K
const int One_M     = (1000 * 1000);

// Function prototypes
int run_random_tests(void);
int run_test(int nitems);

// ----------------------------------------------------------------------------
// Random-generator class.
// Ref: Stroustrup's C++ book, 2nd Ed., Sec. 14.5, Pgs. 191.
// ----------------------------------------------------------------------------
class Rand_int
{
  public:
    Rand_int(int low, int high): dist{low, high} { }

    int operator()() { return dist(rand_gen); }

    void seed(int s) { rand_gen.seed(s); }

  private:
    default_random_engine rand_gen;
    uniform_int_distribution<> dist;
};

// ----------------------------------------------------------------------------
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

      // Load random # of values in input array, within an arbitrary chosen range
      // of -1M to 1M
      void
      loadRand(const int nitems) {
        Rand_int rnd{0, One_M};

        // NOTE: Uncomment this to get predictable random data for debugging.
        // rnd.seed(nitems);

        for (auto ictr = 0; ictr < nitems; ictr++) {
            numbers[ictr] = rnd();
        }
        nelements = nitems;
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

         // cout << __func__ << ": start=0" << ", nelements=" << nelements << endl;
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
      verify(int& error_index)
      {
         for (auto ictr = 0; ictr < (nelements - 1); ictr++) {
            if (numbers[ictr] > numbers[ictr + 1]) {
                error_index = ictr + 1;
                return false;
            }
         }
         error_index = 0;
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
      numInvMerge(const int lo, const int hi, const int nitems_lo,
                  const int nitems_hi)
      {
         assert((lo + nitems_lo) == hi);
         // [l0, l1, l2, ..., li]  [h0, h1, h2, ..., hj]
         // Deal with simple case when lower-list is <= higher-list
         // Case: li <= h0
         if (numbers[lo + nitems_lo - 1] <= numbers[hi]) {
            return 0;
         }

         // Deal with reverse case when all items in sorted upper-half are <= all
         // items in sorted lower-half
         // Case: hj <= l0
         if (   (nitems_lo == nitems_hi)
             && (numbers[hi + nitems_lo - 1] <= numbers[lo])) {
            // cout << __func__ << ":" << __LINE__ << ": do swapChunk()" << endl;

            // Flip both sub-halves as a chunk using memory move
            swapChunk(lo, hi, nitems_lo);
            return (nitems_lo * nitems_lo);
         }

         // Process all items in lower-half. When this loop terminates, lop will
         // be pointing to 1st element -after- end of lower-half; I.e., it will
         // be positioned exactly at the 1st element of the upper-half.
         auto nlower_half_items = nitems_lo;

         // Merge the two sub-lists, counting # inversions along the way
         int *lop = &numbers[lo];
         int *hip = &numbers[hi];

         // Create 2-source sub-lists; 'lo', 'hi' is just for our understanding
         int src_lo[nitems_lo];
         int src_hi[nitems_hi];
         memmove(src_lo, lop, (nitems_lo * sizeof(*lop)));
         memmove(src_hi, hip, (nitems_hi * sizeof(*hip)));

         // Establish terminating pointers
         int *loend = (lop + nitems_lo);
         int *hiend = (hip + nitems_hi);

         auto rv = 0;
         auto curr = lo;
         while ((lop < loend) && (hip < hiend)) {
            if (*lop <= *hip) {
                numbers[curr] = *lop;
                lop++;
            } else {
                numbers[curr] = *hip;
                hip++;
                rv += (loend - lop);
            }
            curr++;
         }

         // If any items left over from 'lo' list, copy them over as a chunk.
         // No need to check for left-over items from 'hi' list as those are
         // already in the output 'numbers' array.
         if (lop < loend) {

            auto leftover_lo_items = (loend - lop);
            memmove(&numbers[curr], lop, (leftover_lo_items * sizeof(*lop)));

            // These many left-over items from 'lo' list were all > each item
            // of the initial 'hi' list. Account for that many # of inversions.
            rv += (leftover_lo_items * nitems_hi);
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
      swap(const int i, const int j) {
        assert(i < nelements);
        assert(j < nelements);

        auto tmp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = tmp;
      }

      // Swap chunk of 'nitems' from i'th and j'th index in numbers[] array
      void
      swapChunk(const int i, const int j, const int nitems) {
        assert((i + nitems) <= nelements);
        assert((j + nitems) <= nelements);
        int tmp[nitems];
        memmove(tmp, &numbers[i], (nitems * sizeof(*tmp)));
        memmove(&numbers[i], &numbers[j], (nitems * sizeof(*tmp)));
        memmove(&numbers[j], tmp, (nitems * sizeof(*tmp)));
      }
};

int
main(int argc, const char *argv[])
{
   std::cout << "Hello World! argc=" << argc << "\n";
   // Load test-data from a file if it's provided.
   if (argc == 2) {
       Inversions data;
       data.load(argv[1]);
       data.dump();

       int nInvFound = data.numInversions();
       cout << "# of inversions found: " << nInvFound << endl;
       auto error_at = 0;
       if (!data.verify(error_at)) {
           cout << "Error! Output array is unsorted: " << endl;
           data.dump();
           return 1;
        }
        return 0;
    }

    int rc = run_random_tests();

    return rc;
}

// ----------------------------------------------------------------------------
int
run_random_tests(void) {
    auto max_items = 100000;
    cout << __func__ << ": Running " << max_items
         << " random data tests for finding inversions." << endl;

    auto nfailed = 0;
    for (auto nitems = 0; nitems < max_items; nitems++) {
        if ((nitems % 10000) == 0) {
            cout << "Random test nitems=" << nitems << endl;
        }
        if (run_test(nitems)) {
            nfailed++;
        }
    }
    return nfailed;
}

// ----------------------------------------------------------------------------
// Returns index (>0) at which sortedness was first found to be broken.
// 0 return => output is sorted correctly. Non-zero => failure in sorting.
int
run_test(int nitems = 10) {

    Inversions data;
    data.loadRand(nitems);
    // if (nitems <= 20) { data.dump(); }

    int nInvFound = data.numInversions();
    // cout << "# of inversions found: " << nInvFound << endl;
    auto error_at = 0;
    if (!data.verify(error_at)) {
        cout << "Error! Output array of " << nitems
             << " items is unsorted at index=" << error_at << endl;
        data.dump();
    }

    return 0;
}
