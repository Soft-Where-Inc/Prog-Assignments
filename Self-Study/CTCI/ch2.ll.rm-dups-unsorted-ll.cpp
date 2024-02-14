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

using namespace std;

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

void test_eliminate_one_dup(void)
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

void test_eliminate_all_but_one_dups(void)
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
