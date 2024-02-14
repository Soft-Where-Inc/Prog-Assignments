/*
 * ch2.ll.rm-dups-unsorted-ll.cpp
 *
 * Remove duplicates from unsorted linked list.
 *
 * Ref: Chap 2, Linked Lists
 *      https://www.geeksforgeeks.org/program-to-implement-singly-linked-list-in-c-using-class/
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

    // Delete the 1st node found given its value.
    bool deleteNode(const int d);

    // Eliminate duplicate items, return # of dup-items eliminated
    int dupEliminate(void);

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
bool
LinkedList::deleteNode(const int d) {
    Node **nextp = &this->head;

    while (*nextp) {
        Node *nodep = *nextp;
        if (nodep->data == d) {
            // Relink past the node being deleted
            *nextp = nodep->next;

            // Detach node from linked list
            nodep->next = NULL;
            delete nodep;
            return true;

        } else {
            nextp = &(nodep->next);
        }
    }
    // Did not find node for requested 'd' to delete.
    return false;
}

// ----------------------------------------------------
// Eliminate duplicate values from linked list.
// Returns: The # of duplicates items eliminated.
int
LinkedList::dupEliminate() {

    int neliminated = 0;
    set<int> unique_items;

    Node **nextp = &this->head;

    while (*nextp) {
        Node *nodep = *nextp;

        // Insert new item into set if it's not found
        if (unique_items.find(nodep->data) == unique_items.end()) {
            unique_items.insert(nodep->data);

            nextp = &nodep->next;
        } else {
            // Relink past the node being deleted
            *nextp = nodep->next;

            // Detach node from linked list
            nodep->next = NULL;
            delete nodep;
            neliminated++;
        }
    }
    return neliminated;
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
void test_append_n_entries(void);
void test_delete_1st_node(void);
void test_delete_inner_node(void);
void test_delete_last_node(void);
void test_eliminate_one_dup(void);
void test_eliminate_all_but_one_dups(void);
void test_random_inserts_eliminate_dups(const int nitems);
void test_inserts_with_half_dups_eliminate_dups(const int nitems);

/*
 * main() and test cases begin here ...
 */
int
main(int argc, char *argv[])
{
    cout << "Hello World." << endl;

    test_print_empty_list();
    test_append_n_entries();
    test_delete_inner_node();

    test_eliminate_one_dup();
    test_eliminate_all_but_one_dups();

    test_random_inserts_eliminate_dups(100);
    test_random_inserts_eliminate_dups(   1 * 1000);
    test_random_inserts_eliminate_dups(  10 * 1000);
    test_random_inserts_eliminate_dups( 100 * 1000);

    // For very large # of inserts, generate half # of items as dups.
    test_inserts_with_half_dups_eliminate_dups(100 * 1000);
}

void
test_print_empty_list(void)
{
    LinkedList list;
    list.printList();
}

void
test_append_n_entries(void)
{
    cout << __func__ << endl;
    LinkedList list;
    list.appendToTail(1);
    list.appendToTail(4);
    list.appendToTail(5);
    list.appendToTail(3);
    list.printList();
}

void
test_delete_inner_node(void)
{
    cout << __func__ << endl;
    LinkedList list;
    list.appendToTail(1);
    list.appendToTail(4);
    list.appendToTail(5);

    // Delete should find node and delete it.
    bool rv = list.deleteNode(4);
    assert(rv == true);

    // Node should not be found.
    rv = list.deleteNode(4);
    assert(rv == false);
    list.printList();
}

void
test_eliminate_one_dup(void)
{
    cout << __func__ << endl;
    LinkedList list;
    list.appendToTail(1);
    list.appendToTail(4);
    list.appendToTail(1);
    list.appendToTail(3);

    int num_eliminated = list.dupEliminate();

    assert(num_eliminated == 1);
    list.printList();
}

void
test_eliminate_all_but_one_dups(void)
{
    cout << __func__ << endl;
    LinkedList list;
    const int nentries = 5;
    for (auto ictr = 0; ictr < nentries; ictr++) {
        list.appendToTail(1);
    }
    int num_eliminated = list.dupEliminate();

    assert(num_eliminated == (nentries - 1));
    list.printList();
}

/*
 * Stress test: Insert large # of randomly generated data. Count the # of
 * unique values inserted, while inserting. Eliminate duplicates and verify
 * counts.
 */
void
test_random_inserts_eliminate_dups(const int nitems_to_insert)
{
    int nunique_items = 0;
    set<int> unique_items;
    Rand_int rnd{-One_M, One_M};    // Include -ve #s also in random range

    cout << __func__ << ": nitems_to_insert=" << nitems_to_insert;
    LinkedList list;
    for (auto ictr = 0; ictr < nitems_to_insert; ictr++) {
        const int newval = rnd();
        list.appendToTail(newval);

        if (unique_items.find(newval) == unique_items.end()) {
            unique_items.insert(newval);
            nunique_items++;
        }
    }

    cout << ", unique=" << nunique_items << endl;

    const int ndeleted = list.dupEliminate();
    assert(ndeleted == (nitems_to_insert - nunique_items));
    cout << " ... Test deleted=" << ndeleted << " items." << endl;
}

/*
 * Stress test: Insert large # of sequentially generated data. Count the # of
 * unique values inserted, while inserting. Eliminate duplicates and verify
 * counts.
 */
void
test_inserts_with_half_dups_eliminate_dups(const int nitems_to_insert)
{

    cout << __func__ << ": nitems_to_insert=" << nitems_to_insert;
    LinkedList list;

    const int nunique_items = (nitems_to_insert/2);
    // First insert half the # of items, sequentially generated data.
    for (auto ictr = 0; ictr < nunique_items; ictr++) {
        list.appendToTail(ictr);
    }

    // Insert another half, in decreasing sequential order.
    for (auto ictr = (nunique_items - 1); ictr >= 0; ictr--) {
        list.appendToTail(ictr);
    }

    cout << ", unique=" << nunique_items << endl;

    const int ndeleted = list.dupEliminate();
    cout << " ... Test deleted=" << ndeleted << " items." << endl;
    assert(ndeleted == (nitems_to_insert - nunique_items));
}
