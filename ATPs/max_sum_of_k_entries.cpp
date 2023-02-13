/*
 * max_sum_of_k_entries.cpp:
 *
 * Compute the max-sum of k-consecutive entries in an array.
 * Basic program implementing Sliding Window technique to compute max(SUM) of
 * k-contiguous entries in an array.
 */
#include <iostream>
#include <vector>

using namespace std;

#define MAX(a, b) ( ((a) >= (b)) ? (a) : (b))

// Function prototypes
void run_test(void);

/* Define a class to compute max(SUM) of k-entries in an array */
class maxSumKEntriesArray
{
   private:
      int  intArray[20]; 
      int  arraySize;

   public:
      // Constructor
      maxSumKEntriesArray(const vector<int>& input) {
         // for (auto i = 0; i < nentries; i++, input++) {
         auto i {0};
         for (auto v : input) {
            intArray[i] = v;
            i++;
         }
         arraySize = input.size();
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
       * Find the max(SUM) of k-contiguous entries of array of ints.
       * This is a brute-force implementation.
       *
       * Returns:
       *  Max SUM() across all k-contiguous values.
       */
      int
      findMaxSumK_BruteForce(int k, int *maxStartIndex)
      {
         if (k >= arraySize) {
            *maxStartIndex = 0;
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
            if (thisMaxSum > maxSum) {
               maxSum = thisMaxSum;
               *maxStartIndex = (start - intArray);
            }

            // Move to next contiguous chunk
            runid++;
            start++;
            end++;
         }
         return maxSum;
      }

      /*
       * Find the max SUM() of k-contiguous entries of array of ints using
       * sliding window technique.
       *
       *  [ - - - - - - - - - - - - - - - - - - - - - - ... n ... - ]
       *    x x x x x                   (k entries)
       *
       * - Find SUM of 1st k-entries.
       * - If (k+1)'th value < 0th value of prev-k'th run, retain current sum.
       * - Otherwise, recompute new SUM() as (currSum - 0th-value + (k+1)'st-value
       *
       * Returns:
       *  # of such k-contiguous runs.
       */
      int
      findMaxSum(int k, int *maxStartIndex) {
         if (k >= arraySize) {
            *maxStartIndex = 0;
            return findSumOfN(intArray, arraySize);
         }

         /*
          * Find the SUM of 1st k-entries. When moving to the next batch,
          * re-compute the new sum by dropping the 0'th value from old sum and
          * add the next entry's value.
          */
         int *start = intArray;
         int *end   = (intArray + k - 1);
         int *arrayEnd = (intArray + arraySize - 1);
         int  runid = 0;
         int  currSum = 0;
         int  result = 0;

         for (int i = 0; i < k; i++) {
            currSum += *(start + i);
         }
         *maxStartIndex = 0;
         result = currSum;

         // Sliding window loop begins here.
         while (end < arrayEnd) {
            // Recompute sum of next k-items
            currSum -= *start;

            // Move to next contiguous chunk
            start++;
            end++;
            currSum += *end;
            if (currSum > result) {
               result = currSum;
               *maxStartIndex = (start - intArray);
            }
         }
         return result;
      }

      // Verify the result by the two methods.
      bool
      verify(int k)
      {
         int startIndex_slow = -1;
         int startIndex_opt  = -1;

         return (    findMaxSumK_BruteForce(k, &startIndex_slow)
                     == findMaxSum(k, &startIndex_opt));
      }

      void
      print(void)
      {
         cout << "[ ";
         for (int i = 0; i < arraySize; i++) {
            cout << intArray[i] << " ";
         }
         cout << "] " << endl;
      }

      void
      print(int startIndex, int numItems) {
         cout << "[ ";
         for (int i = startIndex; i < (startIndex + numItems); i++) {
            cout << intArray[i] << " ";
         }
         cout << "] " << endl;
      }
};

/*
 * main() begins here.
 */
int main()
{
    vector<int> data {2, 3, 4, 55, 6, 3, 2, 44, 232, 344, 101, 333};

    maxSumKEntriesArray my_array(data);
    my_array.print();

    int k = 4;
    int startOfRun = 0;
    int result = my_array.findMaxSumK_BruteForce(k, &startOfRun);

    // Print resulting max(SUM-k-values).
    cout << "\n" << "Brute-force k-running max-sum result: k="
         << k
         << ", max SUM()=" << result
         << ", starting from index=" << startOfRun
         << endl;

    result = my_array.findMaxSum(k, &startOfRun);

    // Print resulting max(SUM-k-values).
    cout << "\n" << "Sliding Window k-running max-sum result: k="
         << k
         << ", max SUM()=" << result
         << ", starting from index=" << startOfRun
         << endl;
    my_array.print(startOfRun, k);

    cout << "Verification of two methods: " << my_array.verify(k) << endl;

    run_test();

    return 0;
}

/*
 * Run a bunch of test cases. Verify that the result by the brute-force
 * approach matches the optimized approach.
 */
void
run_test(void)
{
   vector<vector <int>> test_data
         {  { 2, 3, 4, 5, 4, 3, 2, 1 }
          , { 1, 3, 9, 4, 3, 22, 11, 3, 4, 55 }
         };

   auto k = 3;
   for (auto data : test_data) {
      auto startIndex = 0;

      maxSumKEntriesArray test_array(data);

      auto result = test_array.findMaxSum(k, &startIndex);

      cout << "# of elements: " << data.size()
           << ", max SUM=" << result
           << ", starts at index=" << startIndex
           << ", verification=" << test_array.verify(k)
           << endl;
   }
}
