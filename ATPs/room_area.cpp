// Program to illustrate the working of
// objects and class in C++ Programming
// Ref: https://www.programiz.com/cpp-programming/object-class
// 11.2.2023: Going to UCB to meet Aamani & Rasana. Vandana's visiting

#include <iostream>
using namespace std;

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


    return 0;
}
