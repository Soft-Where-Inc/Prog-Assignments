// Compute the average of k-consecutive entries in an array.

#include <iostream>

using namespace std;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

// create a class
class Room {

   public:
    double length;
    double breadth;
    double height;

    // Default constructor
    Room() {
       length  = 10;
       breadth = 10;
       height  = 12;
    }
    double calculateArea() {
        return length * breadth;
    }

    double calculateVolume() {
        return length * breadth * height;
    }

    // Is this room bigger than the other room by floor area?
    bool
    isBigger(Room other) {
      return (this->calculateArea() > other.calculateArea());
    }
};

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
         for (int i = 0; i < arraySize; i++) {
            cout << i << ": " << intArray[i] << endl;
         }
      }
};

/*
 * main() begins here.
 */
int main() {

    // create object of Room class
    Room room1;

    // assign values to data members
    room1.length = 42.5;
    room1.breadth = 30.8;
    room1.height = 19.2;

    // calculate and display the area and volume of the room
    cout << "Area of Room =  " << room1.calculateArea() << endl;
    cout << "Volume of Room =  " << room1.calculateVolume() << endl;

    Room basic_room;
    cout << "Area of default Room =  " << basic_room.calculateArea() << endl;
    cout << "Volume of default Room =  " << basic_room.calculateVolume() << endl;

    cout << "basic_room is bigger than room1: "
         << basic_room.isBigger(room1) << endl;


    int data[] = {2, 3, 4, 55, 6, 3, 2};

    avgKEntriesArray my_array(data, ARRAY_SIZE(data));
    my_array.printArray();

    float avgs[ARRAY_SIZE(data)];
    int nruns = my_array.findAvgK(3, avgs);

    // Print resulting k-average values.
    cout << "\n" << "Brute-force k-running averages result:" << endl;
    float *avgp = avgs;
    for (int i = 0; i < nruns; i++, avgp++) {
      cout << i << ": avg=" << *avgp << endl;
    }

    nruns = my_array.findSmartAvgK(3, avgs);
    cout << "\n" << "Optimized k-running averages result:" << endl;
    avgp = avgs;
    for (int i = 0; i < nruns; i++, avgp++) {
      cout << i << ": avg=" << *avgp << endl;
    }

    return 0;
}
