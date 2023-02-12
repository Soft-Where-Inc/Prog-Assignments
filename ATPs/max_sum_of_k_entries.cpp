/*
 * max_sum_of_k_entries.cpp:
 *
 * Compute the max-sum of k-consecutive entries in an array.
 * Basic program implementing Sliding Window technique to compute avg. of
 * k-contiguous entries in an array.
 */
#include <iostream>

using namespace std;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))
#define MAX(a, b) ( ((a) >= (b)) ? (a) : (b))

/* Define a class to compute avg of k-entries in an array */
class maxSumKEntriesArray
{
   private:
      int  intArray[20]; 
      int  arraySize;

   public:
      // Constructor
      maxSumKEntriesArray(int *input, int nentries) {
         for (int i = 0; i < nentries; i++, input++) {
            intArray[i] = *input;
         }
         arraySize = nentries;
      }

      /*
       * Find sum of n-entries in sub-array.
       */
      int
      findSumOfN(int *start, int n)
      {
         int result = 0;
         for (int i = 0; i < n; i++) {
            result += *(start + i);
         }
         return result;
      }

      /*
       * Find the avg of k-contiguous entries of array of ints.
       * This is a brute-force implementation.
       *
       * Returns:
       *  # of such k-contiguous runs.
       *  Each k'th run's avg returned by avgs[] array.
      int
      findMaxSumK_BruteForce(int k)
      {
         if (k >= arraySize) {
            return findSumOfN(intArray, arraySize);
         }

         // Do brute-force walk of k-contiguous entries till we exhaust
         // the # of entries in the array.
         int *start = intArray;
         int *end   = (intArray + k);
         int *arrayEnd = (intArray + arraySize);
         int  runid = 0;
         int  maxSum = 0;

         while (end <= arrayEnd) {
            int thisMaxSum = findSumOfN(start, k);
            maxSum = MAX(maxSum, thisMaxSum);

            // Move to next contiguous chunk
            runid++;
            start++;
            end++;
         }
         return maxSum;
      }
       */

      /*
       * Find the avg of k-contiguous entries of array of ints.
       * Returns:
       *  # of such k-contiguous runs.
       *  Each k'th run's avg returned by avgs[] array.
      int
      findSmartAvgK(int k, float *avgs) {
         if (k >= arraySize) {
            *avgs = findAvgOfSubArray(intArray, arraySize);
            return 1;
         }

         // Do brute-force walk of k-contiguous entries till we exhaust
         // the # of entries in the array.
         **
          * Find the avg of 1st k-entries. When moving to the next batch, re-compute
          * the new sum by dropping the 0'th value from old sum and add
          * the next entry's value.
          **
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
       */

      void
      printArray(void)
      {
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

    maxSumKEntriesArray my_array(data, ARRAY_SIZE(data));
    my_array.printArray();

    int k = 4;
    int result = 0;
    // int result = my_array.findMaxSumK_BruteForce(k);

    // Print resulting k-average values.
    cout << "\n" << "Brute-force k-running max-sum result: k="
         << k
         << ", max SUM()=" << result
         << endl;

    return 0;
}
