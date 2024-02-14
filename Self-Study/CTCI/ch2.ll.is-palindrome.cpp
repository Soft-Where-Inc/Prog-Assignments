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

    // Is linked-list empty?
    bool isEmpty() { return (head == NULL); }

    int capacity() { return nitems; }

    // Check if data in linked list is a palindrome.
    bool isPalindrome();

    // Check if data in linked list is a palindrome, without relying on 'nitems'.
    bool isPalindrome2();

    // Print the linked list.
    void printList();

  private:
    int nitems = 0;
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
    nitems++;
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
 * Algorithm: Detect palindrome using a singly linked list.
 *
 *  - Maintain count of # of items in this list when inserting
 *  - Walk forward to half-way mark of the list.
 *      - Walk up to (nitems/2) items in list.
 *      - If nitems is even, we are done for now.
 *      - If nitems is odd, walk up to next 'middle' item.
 *  - Push each item to a stack till you reach 'middle' item
 *  - Continue walking from (mid + 1)'th item to end-of-list
 *  - Check if each new item seen == item at top-of-the-stack
 *  - Return False, if they don't match; Else continue till eolist.
 *  - Return True, if all matches in 2nd half match as above.
 */
bool
LinkedList::isPalindrome() {
    if (this->isEmpty()) {
        return true;
    }
    if (this->capacity() == 1) {
        return true;
    }
    const int nitems = this->capacity();
    int is_odd = (nitems % 2);
    int found_stack[(nitems / 2)];
    int tos = 0;    // top-of-stack index
    bool rv = true;

    Node **nextp = &this->head;
    Node *nodep = NULL;
    while (*nextp && (tos < (nitems / 2))) {
        nodep = *nextp;
        found_stack[tos] = nodep->data;
        tos++;
        nextp = &(nodep->next);
    }
    // Skip past the middle item if capacity is odd.
    if (is_odd) {
        nodep = *nextp;
        nextp = &(nodep->next);
    }

    // Walk forward, checking for palindrome match with top-of-stack item
    tos--;
    while (tos >= 0) {
        nodep = *nextp;
        if (nodep->data != found_stack[tos]) {
            rv = false;
            break;
        }
        nextp = &(nodep->next);
        tos--;
    }

    return rv;
}

// ----------------------------------------------------
/*
 * Algorithm: Detect palindrome using a singly linked list.
 *
 * We do not maintain count of # of items in the list. Instead, determine the
 * mid-way marker by using slow / fast pointer technique.
 * Ref. Based on Soln #2, Pg. 164, CTCI.
 *
 *  - Walk forward to half-way mark of the list. 'slowp' moves forward one node
 *    at a time, while 'fastp' moves forward 2 nodes at a time.
 *  - Push each item to a stack till you reach 'middle' item
 *  - Special-case handling to recognize if 'slowp' is mid-way of list with
 *    odd or even number of items.
 *  - Continue walking from (mid + 1)'th item to end-of-list
 *  - Check if each new item seen == item at top-of-the-stack
 *  - Return False, if they don't match; Else continue till eolist.
 *  - Return True, if all matches in 2nd half match as above.
 */
bool
LinkedList::isPalindrome2() {
    if (this->isEmpty()) {
        return true;
    }
    if (this->capacity() == 1) {
        return true;
    }
    const int nitems = this->capacity();
    int is_odd = (nitems % 2);
    int found_stack[(nitems / 2)];
    int tos = 0;    // top-of-stack index
    bool rv = true;

    // slowp moves one node ahead at a time, while fastp will move 2 nodes at a time.
    // Intially both pointers start from the 1st node of the linked list.
    Node *slowp = this->head;
    Node *fastp = this->head;

    // While there is more nodes to traverse 2-nodes-at-a-time, move ahead ...
    while (slowp && fastp && fastp->next) {
        found_stack[tos] = slowp->data;
        tos++;
        slowp = slowp->next;

        // Check if we can skip a node from the current node that fastp is pointing to.
        fastp = fastp->next;
        fastp = (fastp ? fastp->next : (Node *) NULL);
    }
    // State of fastp tells us if there are odd # of items in the list..
    // slowp is positioned at the the middle item if list capacity is odd.
    if (fastp) {
        assert(fastp->next == NULL);
        slowp = slowp->next;
    }

    // Walk forward, checking for palindrome match with top-of-stack item
    tos--;
    while (tos >= 0) {
        if (slowp->data != found_stack[tos]) {
            rv = false;
            break;
        }
        tos--;
        slowp = slowp->next;
    }

    return rv;
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
        cout << "Node: " << nodep
             << " { next=" << nodep->next
             << ", data=" << nodep->data
             << " }" << endl;

        nextp = &(nodep->next);
    }
}

// Function prototypes

void test_isEmpty_list(void);
void test_print_empty_list(void);
void test_appendToTail(void);
void test_capacity(void);
void test_capacity_random5(void);
void test_appendRandomToTail(void);
void test_isPalindrome_empty_list(void);
void test_isPalindrome_one_item(void);
void test_isPalindrome_two_equal_items(void);
void test_isPalindrome_two_diff_items(void);
void test_isPalindrome_three_equal_items(void);
void test_isPalindrome_three_diff_items(void);

// Re-test with new implementation of palindrome checks.
void test_isPalindrome2_two_equal_items(void);
void test_isPalindrome2_two_diff_items(void);
void test_isPalindrome2_three_equal_items(void);
void test_isPalindrome2_three_diff_items(void);
void test_isPalindrome2_four_equal_items(void);
void test_isPalindrome2_four_diff_items(void);
void test_isPalindrome2_five_equal_items(void);
void test_isPalindrome2_five_diff_items(void);

/*
 * main() and test cases begin here ...
 */
int
main(int argc, char *argv[])
{
    cout << argv[0] << ": Hello World." << endl;

    test_isEmpty_list();
    test_print_empty_list();
    test_capacity();
    test_capacity_random5();
    test_isPalindrome_empty_list();
    test_isPalindrome_one_item();
    test_isPalindrome_two_equal_items();
    test_isPalindrome_two_diff_items();
    test_isPalindrome_three_equal_items();
    test_isPalindrome_three_diff_items();

    // Re-test with new implementation of palindrome checks.
    test_isPalindrome2_two_equal_items();
    test_isPalindrome2_two_diff_items();
    test_isPalindrome2_three_equal_items();
    test_isPalindrome2_three_diff_items();
    test_isPalindrome2_four_equal_items();
    test_isPalindrome2_four_diff_items();
    test_isPalindrome2_five_equal_items();
    test_isPalindrome2_five_diff_items();
}

void
test_isEmpty_list(void)
{
    cout << __func__;
    LinkedList list;
    assert(list.isEmpty());
    cout << " ... OK" << endl;
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
    list.appendToTail(2);
    list.printList();
    cout << " ... OK" << endl;
}

void
test_appendRandomToTail(void)
{
    cout << __func__;
    LinkedList list;
    list.appendRandomToTail();
    list.printList();
    cout << " ... OK" << endl;
}

void
test_capacity(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(42);
    assert(list.capacity() == 1);
    cout << " ... OK" << endl;
}

void
test_capacity_random5(void)
{
    cout << __func__;
    LinkedList list;
    const int nitems = 5;
    for (auto ictr = 0; ictr < nitems; ictr++) {
        list.appendRandomToTail();
    }
    assert(list.capacity() == nitems);
    cout << " ... OK" << endl;
}

void
test_isPalindrome_empty_list(void)
{
    cout << __func__;
    LinkedList list;
    assert(list.isPalindrome());
    cout << " ... OK" << endl;
}

void
test_isPalindrome_one_item(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    assert(list.isPalindrome());
    cout << " ... OK" << endl;
}

void
test_isPalindrome_two_equal_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(2);
    assert(list.isPalindrome());
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_two_equal_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(2);
    assert(list.isPalindrome2());
    cout << " ... OK" << endl;
}

void
test_isPalindrome_two_diff_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(3);
    assert(list.isPalindrome() == false);
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_two_diff_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(3);
    assert(list.isPalindrome2() == false);
    cout << " ... OK" << endl;
}

void
test_isPalindrome_three_equal_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(2);
    assert(list.isPalindrome());
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_three_equal_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(2);
    assert(list.isPalindrome2());
    cout << " ... OK" << endl;
}

void
test_isPalindrome_three_diff_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(3);
    assert(list.isPalindrome() == false);
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_three_diff_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(3);
    assert(list.isPalindrome2() == false);
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_four_equal_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(42);
    list.appendToTail(2);
    assert(list.isPalindrome2() == true);
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_four_diff_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(42);
    list.appendToTail(3);
    assert(list.isPalindrome2() == false);
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_five_equal_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(99);
    list.appendToTail(42);
    list.appendToTail(2);
    assert(list.isPalindrome2() == true);
    cout << " ... OK" << endl;
}

void
test_isPalindrome2_five_diff_items(void)
{
    cout << __func__;
    LinkedList list;
    list.appendToTail(2);
    list.appendToTail(42);
    list.appendToTail(99);
    list.appendToTail(42);
    list.appendToTail(3);
    assert(list.isPalindrome2() == false);
    cout << " ... OK" << endl;
}
