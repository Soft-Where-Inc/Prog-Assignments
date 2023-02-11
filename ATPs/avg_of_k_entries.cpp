/*
 * Compute the average of k-consecutive entries in an array.
 * Basic program implementing Sliding Window technique to compute avg. of
 * k-contiguous entries in an array.
 */
#include <iostream>

using namespace std;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

/* Define a class to compute avg of k-entries in an array */
class avgKEntriesArray
{
   private:
      int  intArray[20]; 
      int  arraySize;
   public:
      // Constructor
      avgKEntriesArray(int *input, int nentries) {
         for (int i = 0; i < nentries; i++, input++) {
            intArray[i] = *input;
         }
         arraySize = nentries;
      }

      /*
       * Find the avg of a sub-array of n-elements, from start position.
       */
      float
      findAvgOfSubArray(int *start, int n) {
         float result = 0;
         for (int i = 0; i < n; i++, start++) {
            result += *start;
         }
         return (result / n);
      }

      /*
       * Find the avg of k-contiguous entries of array of ints.
       * This is a brute-force implementation.
       *
       * Returns:
       *  # of such k-contiguous runs.
       *  Each k'th run's avg returned by avgs[] array.
       */
      int
      findAvgK(int k, float *avgs) {
         if (k >= arraySize) {
            *avgs = findAvgOfSubArray(intArray, arraySize);
            return 1;
         }

         // Do brute-force walk of k-contiguous entries till we exhaust
         // the # of entries in the array.
         int *start = intArray;
         int *end   = (intArray + k);
         int *arrayEnd = (intArray + arraySize);
         int  runid = 0;

         while (end <= arrayEnd) {
            avgs[runid] = findAvgOfSubArray(start, k);

            // Move to next contiguous chunk
            runid++;
            start++;
            end++;
         }
         return runid;
      }

      /*
       * Find the avg of k-contiguous entries of array of ints.
       * Returns:
       *  # of such k-contiguous runs.
       *  Each k'th run's avg returned by avgs[] array.
       */
      int
      findSmartAvgK(int k, float *avgs) {
         if (k >= arraySize) {
            *avgs = findAvgOfSubArray(intArray, arraySize);
            return 1;
         }

         // Do brute-force walk of k-contiguous entries till we exhaust
         // the # of entries in the array.
         /*
          * Find the avg of 1st k-entries. When moving to the next batch, re-compute
          * the new sum by dropping the 0'th value from old sum and add
          * the next entry's value.
          */
         int *start = intArray;
         int *end   = (intArray + k - 1);
         int *arrayEnd = (intArray + arraySize);
         int  runid = 0;
         int  currSum = 0;

         for (int i = 0; i < k; i++) {
            currSum += *(start + i);
         }

         do {
            avgs[runid] = (currSum * 1.0 / k);

            // Recompute sum of next k-items
            currSum -= *start;

            // Move to next contiguous chunk
            start++;
            end++;
            currSum += *end;

            runid++;
         } while (end < arrayEnd);
         return runid;
      }

      void
      printArray(void) {
         cout << "[ ";
         for (int i = 0; i < arraySize; i++) {
            cout << intArray[i] << ",";
         }
         cout << "] " << endl;
      }
};

/*
 * main() begins here.
 */
int main()
{
    int data[] = {2, 3, 4, 55, 6, 3, 2, 44, 232, 344, -1, 333};

    avgKEntriesArray my_array(data, ARRAY_SIZE(data));
    my_array.printArray();

    int k = 4;
    float avgs[ARRAY_SIZE(data)];
    int nruns = my_array.findAvgK(k, avgs);

    // Print resulting k-average values.
    cout << "\n" << "Brute-force k-running averages result: k="
         << k
         << endl;
    float *avgp = avgs;
    for (int i = 0; i < nruns; i++, avgp++) {
      cout << i << ": avg=" << *avgp << endl;
    }

    nruns = my_array.findSmartAvgK(k, avgs);
    cout << "\n" << "Optimized k-running averages result:" << endl;
    avgp = avgs;
    for (int i = 0; i < nruns; i++, avgp++) {
      cout << i << ": avg=" << *avgp << endl;
    }

    return 0;
}
