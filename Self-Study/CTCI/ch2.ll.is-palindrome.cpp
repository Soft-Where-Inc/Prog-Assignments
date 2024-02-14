/*
 * ch2.ll.is-palindrome.cpp
 *
 * Check if a singly-linked list is a palindrome.
 *
 * Ref: Chap 2, Linked Lists
 *
 * History:
 *  Feb-2024    Started
 */

#include <iostream>
#include <random>

using namespace std;

const int One_M     = (1000 * 1000);

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
// Definition of node in a singly-linked list
// ----------------------------------------------------------------------------
class Node
{
  public:
    Node *next = NULL;
    int   data;

    // Default constructor
    Node() {
        data = 0;
        next = NULL;
    }

    // Constructor
    Node(const int d) {
        data = d;
    }

    // Print contents of just this node
    void
    prNode() {
        cout << "Node=" << this
             << ", next=" << this->next
             << ", data=" << this->data
             << endl;
    }

  private:

};

// ----------------------------------------------------------------------------
// Definition of a singly linked-list class
// ----------------------------------------------------------------------------
class LinkedList
{
  public:
    Node * head;

    // Default constructor
    LinkedList() { head = NULL; }

    // Append new item to the end of the list.
    Node * appendToTail(const int d);

    // Append new randomly generated item to the end of the list.
    Node * appendRandomToTail();

    // Print the linked list.
    void printList();
};

// ----------------------------------------------------
Node *
LinkedList::appendToTail(const int d) {
    Node *newNode = new Node(d);
    Node **nextp = &this->head;

    while (*nextp) {
        nextp = &((*nextp)->next);
    }
    *nextp = newNode;
    return newNode;
}

// ----------------------------------------------------
Node *
LinkedList::appendRandomToTail() {
    Rand_int rnd{-One_M, One_M};    // Include -ve #s also in random range
    return appendToTail(rnd());
}

// ----------------------------------------------------
void
LinkedList::printList() {
    if (this->head == NULL) {
        cout << "Empty list." << endl;
        return;
    }
    Node **nextp = &this->head;
    while (*nextp) {
        Node *nodep = *nextp;
        cout << "Node: " << nodep
             << " { next=" << nodep->next
             << ", data=" << nodep->data
             << " }" << endl;

        nextp = &(nodep->next);
    }
}

// Function prototypes

void test_print_empty_list(void);
void test_appendToTail(void);
void test_appendRandomToTail(void);

/*
 * main() and test cases begin here ...
 */
int
main(int argc, char *argv[])
{
    cout << argv[0] << ": Hello World." << endl;

    test_print_empty_list();
    test_appendToTail();
    test_appendRandomToTail();
}

void
test_print_empty_list(void)
{
    cout << __func__ << endl;
    LinkedList list;
    list.printList();
}

void
test_appendToTail(void)
{
    cout << __func__ << endl;
    LinkedList list;
    list.appendToTail(2);
    list.printList();
}

void
test_appendRandomToTail(void)
{
    cout << __func__ << endl;
    LinkedList list;
    list.appendRandomToTail();
    list.printList();
}
