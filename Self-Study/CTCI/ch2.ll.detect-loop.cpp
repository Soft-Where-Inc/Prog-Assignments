/*
 * ch2.ll.detect-loop.cpp
 *
 * Detect a loop in a singly linked-list (caused by list corruption)
 *
 * Ref: Chap 2, Linked Lists. Prob 2.8
 *
 * History:
 *  Feb-2024    Started
 */

#include <iostream>
#include <set>
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
        cout << " Node=" << this
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

    // Find out if there is a loop in this linked-list. Return the ptr to the
    // node to which some other node is pointing to, causing the loop
    Node * findLoop();

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
/*
 * Algorithm to detect a loop in a singly linked-list:
 *
 * Assume that we cannot change the Node contents; i.e., while traversing the list
 * we cannot dirty the node, by setting a bit, which will dirty cache-lines etc.
 * We need an ancilliary struct to track 'visited nodes'. As we have to return the
 * ptr to the node where the loop ends, create a SET to track addresses as we
 * traverse them. If the addr of the next found is found in the SET, that's a loop.
 */
Node *
LinkedList::findLoop() {

    Node **nextp = &this->head;
    Node *rnode = NULL; // Return node ptr, which is where the loop ends.

    set<void *> visited = {};

    // Traverse through all the nodes
    while (*nextp) {
        Node *nodep = *nextp;
        if (visited.find(nodep) == visited.end()) {
            visited.insert(nodep);
        } else {
            rnode = nodep;
            break;
        }
        nextp = &(nodep->next);
    }
    return rnode;
}

// ----------------------------------------------------
void
LinkedList::printList() {
    if (this->head == NULL) {
        // cout << "Empty list." << endl;
        return;
    }
    Node **nextp = &this->head;
    while (*nextp) {
        Node *nodep = *nextp;
        cout << " Node: " << nodep
             << " { next=" << nodep->next
             << ", data=" << nodep->data
             << " }" << endl;

        nextp = &(nodep->next);
    }
}

// Function prototypes

void test_print_empty_list(void);
void test_appendToTail(void);
void test_findLoop_empty_list(void);
void test_findLoop_1node(void);
void test_findLoop_1node_corrupted(void);
void test_findLoop_2nodes(void);
void test_findLoop_2nodes_corrupted_n2(void);
void test_findLoop_2nodes_corrupted_n2_points_to_n1(void);

/*
 * main() and test cases begin here ...
 */
int
main(int argc, char *argv[])
{
    cout << argv[0] << ": Hello World." << endl;

    test_print_empty_list();
    test_appendToTail();
    test_findLoop_empty_list();
    test_findLoop_1node();
    test_findLoop_1node_corrupted();
    test_findLoop_2nodes();
    test_findLoop_2nodes_corrupted_n2();
    test_findLoop_2nodes_corrupted_n2_points_to_n1();
}

void
test_print_empty_list(void)
{
    cout << __func__;
    LinkedList list;
    list.printList();
    cout << " ... OK" << endl;
}

void
test_appendToTail(void)
{
    cout << __func__;
    LinkedList list;
    Node *n2 = list.appendToTail(2);
    list.printList();
    cout << " ... OK" << endl;
}

void
test_findLoop_empty_list(void)
{
    cout << __func__;
    LinkedList list;
    Node *loopNode = list.findLoop();
    assert(loopNode == NULL);
    cout << " ... OK" << endl;
}

void
test_findLoop_1node(void)
{
    cout << __func__;
    LinkedList list;
    Node *n1 = list.appendToTail(1);
    Node *loopNode = list.findLoop();
    assert(loopNode == NULL);
    cout << " ... OK" << endl;
}

void
test_findLoop_1node_corrupted(void)
{
    cout << __func__;
    LinkedList list;
    Node *n1 = list.appendToTail(1);

    // cause loop corruption by poking next ptr
    assert(n1->next == NULL);
    n1->next = n1;

    Node *loopNode = list.findLoop();
    assert(loopNode == n1);
    cout << " ... OK" << endl;
}

void
test_findLoop_2nodes(void)
{
    cout << __func__;
    LinkedList list;
    Node *n1 = list.appendToTail(1);
    Node *n2 = list.appendToTail(2);

    Node *loopNode = list.findLoop();
    assert(loopNode == NULL);
    cout << " ... OK" << endl;
}

void
test_findLoop_2nodes_corrupted_n2(void)
{
    cout << __func__;
    LinkedList list;
    Node *n1 = list.appendToTail(1);
    Node *n2 = list.appendToTail(2);

    // cause loop corruption: n2 points back to itself.
    assert(n2->next == NULL);
    n2->next = n2;

    Node *loopNode = list.findLoop();
    assert(loopNode == n2);
    cout << " ... OK" << endl;
}

void
test_findLoop_2nodes_corrupted_n2_points_to_n1(void)
{
    cout << __func__;
    LinkedList list;
    Node *n1 = list.appendToTail(1);
    Node *n2 = list.appendToTail(2);

    // cause loop corruption: n2 points back to n1.
    assert(n1->next == n2);
    assert(n2->next == NULL);
    n2->next = n1;

    Node *loopNode = list.findLoop();
    assert(loopNode == n1);
    cout << " ... OK" << endl;
}
