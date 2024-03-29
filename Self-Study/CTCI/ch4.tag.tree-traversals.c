/*
 * -----------------------------------------------------------------------------
 * Implement basic Tree Traversal algorithms
 *
 * Ref:
 * - https://www.geeksforgeeks.org/generating-random-number-range-c/
 *
 * Usage: $ gcc -o ch4.tag.tree-traversals ch4.tag.tree-traversals.c
 *        $ leaks -atExit -- ./ch4.tag.tree-traversals
 *
 * Implemented:
 *  - Tree construction using BFS 'search' construction
 *  - Preorder, Inorder, Postorder and Level Order traversal to print nodes
 *  - Qs 4.2 Build a minimal tree
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

typedef unsigned int uint32;

typedef _Bool bool;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 * Definition of basic tree Node struct.
 */
typedef struct node {
    struct node *   left;
    struct node *   right;
    int             data;
} Node;

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))
#define PRARRAY(arr) prArray((arr), ARRAYSIZE(arr))

/*
 * Basic random int32 generation support.
 */
#define RAND_INIT() srand(time(0))
#define NEW_RAND()  RAND_INT_RANGE(0, 100)
#define RAND_INT_RANGE(lower, upper)                \
        ((rand() % (upper - lower + 1)) + lower)

/* Useful constants */
#define K_KILO  1024
#define MILLION (1000 * 1000)

// Tree-printing traversal orders
typedef enum {
      PR_TREE_INORDER
    , PR_TREE_PREORDER
    , PR_TREE_POSTORDER
    , PR_TREE_LEVELORDER } traversal_t;

// Function Prototypes
Node *mkNode(const int val);
void freeNode(Node **np);
void prNode(const Node *np);

// values is an array of 'nitems' ints.
Node *makeTree(int *values, int nitems);

void mkTree(Node *qOfNodes[], int qsize, int *values, int nitems);

Node * mkMinimalBinaryTree(int *values, const int nitems);
Node * mkMBT_recurse(int *values, const int nitems);

bool isValidBinTree(const Node *nodep);
bool isValid_BinTree(const Node *nodep, const int sentinel, bool is_right);

void prTree(Node *rootp);
void prTreeTraverse(Node *rootp, traversal_t traverse);
void prTreeInorder(Node *rootp, uint32 level, char nodeType);
void prTreePreorder(Node *rootp, uint32 level, char nodeType);
void prTreePostorder(Node *rootp, uint32 level, char nodeType);
void prTreeLevelorder(Node *rootp, uint32 level, char nodeType);
int  prTree_bfs_Levelorder(Node *qNodes[], int qsize);
void prNodeLevel(Node *rootp, uint32 level, char nodeType);
int  numLevels(Node *rootp);

void prArray(int *arr, int size);
void test_this(void);
void test_msg(const char *msg);
void test_prNode(void);
void test_mkTree_1node(void);
void test_mkTree_3nodes(void);
void test_mkTree_5nodes(void);
void test_mkTree_7nodes(void);
void test_mkTree_8nodes(void);
void test_mkTree_9nodes(void);
void test_freeTree_1node(void);
void test_mkTree_random_10_nodes(void);
void test_prTree_9nodes_inorder(void);
void test_prTree_9nodes_postorder(void);
void test_numLevels_10_nodes(void);
void test_numLevels_random_128_nodes(void);

void test_prTree_Levelorder_3nodes(void);
void test_prTree_Levelorder_5nodes(void);
void test_prTree_Levelorder_7nodes(void);
void test_prTree_Levelorder_9nodes(void);

void test_mkMinimalBinaryTree_1node(void);
void test_mkMinimalBinaryTree_2nodes(void);
void test_mkMinimalBinaryTree_3nodes(void);
void test_isValidBinTree(void);
void test_mkMinimalBinaryTree_4nodes(void);
void test_mkMinimalBinaryTree_5nodes(void);
void test_mkMinBinaryTree_random_20_nodes(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
                  { "test_prNode"                       , test_prNode }
                , { "test_mkTree_1node"                 , test_mkTree_1node }
                , { "test_freeTree_1node"               , test_freeTree_1node }
                , { "test_mkTree_3nodes"                , test_mkTree_3nodes }
                , { "test_mkTree_5nodes"                , test_mkTree_5nodes }
                , { "test_mkTree_7nodes"                , test_mkTree_7nodes }
                , { "test_mkTree_8nodes"                , test_mkTree_8nodes }
                , { "test_mkTree_9nodes"                , test_mkTree_9nodes }
                , { "test_mkTree_random_10_nodes"       , test_mkTree_random_10_nodes }
                , { "test_prTree_9nodes_inorder"        , test_prTree_9nodes_inorder }
                , { "test_prTree_9nodes_postorder"      , test_prTree_9nodes_postorder }
                , { "test_numLevels_10_nodes"           , test_numLevels_10_nodes }
                , { "test_numLevels_random_128_nodes"   , test_numLevels_random_128_nodes }
                , { "test_prTree_Levelorder_3nodes"     , test_prTree_Levelorder_3nodes }
                , { "test_prTree_Levelorder_5nodes"     , test_prTree_Levelorder_5nodes }
                , { "test_prTree_Levelorder_7nodes"     , test_prTree_Levelorder_7nodes }
                , { "test_prTree_Levelorder_9nodes"     , test_prTree_Levelorder_9nodes }

                , { "test_mkMinimalBinaryTree_1node"    , test_mkMinimalBinaryTree_1node }
                , { "test_mkMinimalBinaryTree_2nodes"   , test_mkMinimalBinaryTree_2nodes }
                , { "test_mkMinimalBinaryTree_3nodes"   , test_mkMinimalBinaryTree_3nodes }
                , { "test_isValidBinTree"               , test_isValidBinTree }
                , { "test_mkMinimalBinaryTree_4nodes"   , test_mkMinimalBinaryTree_4nodes }
                , { "test_mkMinimalBinaryTree_5nodes"   , test_mkMinimalBinaryTree_5nodes }
                , { "test_mkMinBinaryTree_random_20_nodes"   , test_mkMinBinaryTree_random_20_nodes }
};

const int Num_Test_fns = ARRAYSIZE(Test_fns);

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

/*
 * -----------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
    const char *hello_msg = "Hello World";
    printf("%s: %s. (argc=%d)\n", argv[0], hello_msg, argc);

    int rv = 0;
    // Run all test cases if no args are provided.
    if (argc == 1) {

        test_this();
        // test_msg(hello_msg);
        for (int tctr = 0; tctr < Num_Test_fns; tctr++) {
            Test_fns[tctr].tfn();
        }

    } else if (strncmp("--help", argv[1], strlen("--help")) == 0) {
        printf(Usage, argv[0]);
        return rv;
    } else if (strncmp("test_", argv[1], strlen("test_")) == 0) {

        // Execute the named test-function, if it's a supported test-function
        int tctr = 0;
        for (; tctr < Num_Test_fns; tctr++) {
            if (!strcmp(Test_fns[tctr].tfn_name, argv[1])) {
                Test_fns[tctr].tfn();
                break;
            }
        }
        if (tctr == ARRAYSIZE(Test_fns)) {
            printf("Warning: Named test-function '%s' not found.\n", argv[1]);
            rv = 1;
        }
    } else {
        printf("Unknown argument: '%s'\n", argv[1]);
        rv = 1;
    }

    return rv;
}

// **** Node manipulation routines ****
Node *
mkNode(const int val)
{
    Node *np = malloc(sizeof(*np));
    if (!np) {
        return (Node *) NULL;
    }
    np->left = NULL;
    np->right = NULL;
    np->data = val;
    return np;
}

// Free memory allocated to Node and clear-out its handle in caller's context.
void
freeNode(Node **np)
{
    if (*np) {
        free(*np);
        *np = NULL;
    }
}

void
prNode(const Node *np)
{
    printf("np=%p {left=%p, data=%d, right=%p}\n",
            np, np->left, np->data, np->right);
}


/*
 * -----------------------------------------------------------------------------
 * makeTree(): Tree 'constructor' routine.
 *
 * Given an array of integers, build a binary tree, using Breadth-First 'Search'
 * for building tree at each level.
 * -----------------------------------------------------------------------------
 */
Node *
makeTree(int *values, const int nitems)
{
    if (!values || !nitems) {
        return (Node *) NULL;
    }
    Node *rootp = mkNode(values[0]);
    if (!rootp) {
        return NULL;
    }
    Node *qNodes[nitems];
    memset(qNodes, 0, sizeof(qNodes));
    qNodes[0] = rootp;

    // This is where the recursive tree construction begins
    mkTree(qNodes, 1, (values + 1), (nitems - 1));

    return rootp;
}

/*
 * -----------------------------------------------------------------------------
 * mkTree() - BFS implementation to build tree at each level.
 *
 * Parameters:
 *  qNodes  - Ptr to head of the queue
 *  qsize   - Queue has these many items, incl 1st item in queue
 * -----------------------------------------------------------------------------
 */
void
mkTree(Node *qNodes[], int qsize, int *values, int nitems)
{
    // If input is exhausted, we are done.
    if (nitems == 0) {
        return;
    }

    Node *curr = qNodes[0];
    assert(curr->left == NULL);
    assert(curr->right == NULL);

    curr->left = mkNode(*values);
    values++;
    nitems--;
    if (nitems == 0) {
        return;
    }
    curr->right = mkNode(*values);
    values++;
    nitems--;

    // Shove two newly allocated nodes to the tail of the queue
    qNodes[qsize++] = curr->left;
    qNodes[qsize++] = curr->right;

    // Recurse to build tree for next node at head-of-the-queue.
    // (Therefore, queue size goes down by 1.)
    mkTree((qNodes + 1), (qsize - 1), values, nitems);
}

/*
 * -----------------------------------------------------------------------------
 * freeTree() - Routine to free allocated for all nodes in a tree.
 *
 * Implement a post-order traversal of all nodes in the tree, so that we free
 * the memory for all nodes in the left sub-tree, then the right sub-tree
 * -AND THEN- free the memory for the input 'nodep'.
 * -----------------------------------------------------------------------------
 */
void
freeTree(Node **nodep)
{
    Node *node = *nodep;
    if (node == NULL) {
        return;
    }
    if (node->left) {
        freeTree(&node->left);
    }
    if (node->right) {
        freeTree(&node->right);
    }
    freeNode(nodep);
}

/*
 * -----------------------------------------------------------------------------
 * Prob 4.2 Minimal Tree (Pg. 75) Given a sorted (increasing order) array with
 * unique integers, build a binary tree with minimal height.
 *
 * Algorithm:
 *  - "Mid-point" item will be the root of the tree.
 *  - Items < mid-point value will be inserted to left subtree
 *  - items >= mid-point value will be inserted to right subtree
 *  - Find the mid-point item in the array. Connect it to the root node
 *  - Recurse on the left [0 - (n/2) - 1] entries, connecting to root->left
 *  - Recurse on ((n/2) + 1, (n-1)] entries, connecting to root->right
 * -----------------------------------------------------------------------------
 */
Node *
mkMinimalBinaryTree(int *values, const int nitems)
{
    Node *rootp = NULL;
    if (!values || !nitems) {
        return rootp;
    }
    rootp = mkMBT_recurse(values, nitems);
    return rootp;
}

Node *
mkMBT_recurse(int *values, const int nitems)
{
    Node *rootp = NULL;
    if (!values || !nitems) {
        return rootp;
    }
    if (nitems == 1) {
        rootp = mkNode(*values);
    } else if (nitems <= 3) {
        rootp = mkNode(*(values + 1));
        rootp->left = mkNode(*values);
        if (nitems == 3) {
            rootp->right = mkNode(*(values + 2));
        }
    } else {
        const int mid = (nitems / 2);

        rootp = mkNode(*(values + mid));

        rootp->left = mkMBT_recurse(values, mid);
        rootp->right = mkMBT_recurse((values + mid + 1), (nitems - mid - 1));
    }
    return rootp;
}

/*
 * -----------------------------------------------------------------------------
 * Check if tree rooted at 'rootp' is a valid minimal binary tree.
 * All nodes to left  of rootp should be < rootp->data.
 * All nodes to right of rootp should be >= rootp->data.
 * -----------------------------------------------------------------------------
 */
bool
isValidBinTree(const Node *nodep)
{
    bool rv = TRUE;
    if (nodep == NULL) {
        return rv;
    }
    if (!nodep->left && !nodep->right) {
        return rv;
    }
    if (nodep->left) {
        rv &= isValid_BinTree(nodep->left, nodep->data, FALSE);
    }
    if (nodep->right) {
        rv &= isValid_BinTree(nodep->right, nodep->data, TRUE);
    }
    return rv;
}

/*
 * Check if the sub-tree rooted at 'nodep' is valid binary tree.
 * 'sentinel' is the data-value at this nodep's parent node.
 * 'is_right' indicates if this nodep is parent-node->left or
 * parent-node->right child.
 *
 * - If nodep is left child  of its parent, all values in all
 *   nodes of this sub-tree should be < sentinel value.
 * - If nodep is right child of its parent, all values in all
 *   nodes of this sub-tree should be >= sentinel value.
 */
bool
isValid_BinTree(const Node *nodep, const int sentinel, bool is_right)
{
    if (!is_right) {
       if (nodep->data >= sentinel) {
            return FALSE;
        }
    } else if (nodep->data < sentinel) {
        return FALSE;
    }

    bool rv = TRUE;
    if (nodep->left) {
        rv &= isValid_BinTree(nodep->left, nodep->data, FALSE);
    }
    if (nodep->right) {
        rv &= isValid_BinTree(nodep->right, nodep->data, TRUE);
    }
    return rv;
}

/*
 * -----------------------------------------------------------------------------
 * prTree() - Tree printing routine(s). Default is preorder traversal.
 * -----------------------------------------------------------------------------
 */
void
prTree(Node *rootp)
{
    if (!rootp)
        return;

    prTreeTraverse(rootp, PR_TREE_PREORDER);
}

// -----------------------------------------------------------------------------
void
prTreeTraverse(Node *rootp, traversal_t traverse)
{
    if (!rootp) {
        return;
    }
    const char *traverse_type = ((traverse == PR_TREE_INORDER)   ? "Inorder" :
                                 (traverse == PR_TREE_PREORDER)  ? "Preorder" :
                                 (traverse == PR_TREE_POSTORDER) ? "Postorder"
                                                                 : "Level-order");
    printf("\nTree at rootp=%p, %s traversal\n", rootp, traverse_type);
    switch(traverse)
    {
      case PR_TREE_INORDER:
          prTreeInorder(rootp, 0, 'R');
          break;
      case PR_TREE_PREORDER:
          prTreePreorder(rootp, 0, 'R');
          break;
      case PR_TREE_POSTORDER:
          prTreePostorder(rootp, 0, 'R');
          break;
      case PR_TREE_LEVELORDER:
          prTreeLevelorder(rootp, 0, 'R');
          break;
    }
}

// -----------------------------------------------------------------------------
void
prTreeInorder(Node *nodep, uint32 level, char nodeType)
{
    assert(nodep != NULL);

    if (nodep->left) {
        prTreeInorder(nodep->left, (level + 1), 'l');
    }
    prNodeLevel(nodep, level, nodeType);

    if (nodep->right) {
        prTreeInorder(nodep->right, (level + 1), 'r');
    }
}

// -----------------------------------------------------------------------------
void
prTreePreorder(Node *nodep, uint32 level, char nodeType)
{
    assert(nodep != NULL);

    prNodeLevel(nodep, level, nodeType);
    if (nodep->left) {
        prTreePreorder(nodep->left, (level + 1), 'l');
    }
    if (nodep->right) {
        prTreePreorder(nodep->right, (level + 1), 'r');
    }
}

// -----------------------------------------------------------------------------
void
prTreePostorder(Node *nodep, uint32 level, char nodeType)
{
    assert(nodep != NULL);

    if (nodep->left) {
        prTreePostorder(nodep->left, (level + 1), 'l');
    }
    if (nodep->right) {
        prTreePostorder(nodep->right, (level + 1), 'r');
    }
    prNodeLevel(nodep, level, nodeType);
}

/*
 * -----------------------------------------------------------------------------
 * Driver routine to recurse through tree and print nodes in level-order.
 * At each level, we have to maintain a queue of nodes as found at that level.
 * As we don't know the depth of the tree, do a quick traversal to find # of levels.
 * -----------------------------------------------------------------------------
 */
void
prTreeLevelorder(Node *rootp, uint32 level, char nodeType)
{
    assert(rootp != NULL);
    int nlevels = numLevels(rootp);
    int numnodes = (exp2(nlevels + 1) - 1);

    // Declare an on-stack 'queue' of nodes
    Node *qNodes[numnodes];
    memset(qNodes, 0, sizeof(qNodes));

    // Initialize BFS traversal of tree using this queue, starting from root.
    int head = 0;
    qNodes[head] = rootp;
    int qsize = 1;
    while (qsize > 0) {
        qsize = prTree_bfs_Levelorder(&qNodes[head], qsize);

        // We processed one node at the head of the queue. So qsize decreases by 1.
        head++;
        qsize--;
    }
}

/*
 * -----------------------------------------------------------------------------
 * Process the node at the head of qNodes queue.
 * 'qsize' is # of elements in the queue including this one.
 */
int
prTree_bfs_Levelorder(Node *qNodes[], int qsize)
{
    if (qsize == 0) {
        return qsize;
    }
    Node *nodep = qNodes[0];
    prNodeLevel(nodep, 0, '?');

    // Add this nodes children to the queue
    if (nodep->left) {
        qNodes[qsize++] = nodep->left;
    }
    if (nodep->right) {
        qNodes[qsize++] = nodep->right;
    }
    return qsize;
}


void
prNodeLevel(Node *nodep, uint32 level, char nodeType)
{
    assert(nodep != NULL);
    printf("[%p lvl=%d:%s:val=%d [lc=%p, rc=%p]\n",
            nodep, level,
            ((nodeType) == 'R' ? "R " :
             (nodeType) == 'l' ? "<-" :
             (nodeType) == 'r' ? "->"
                               : "  "),
              nodep->data,
              nodep->left, nodep->right);
}

/*
 * Traverse left-side of the root to identify # of levels of the tree.
 * We assume that tree construction was done such that left child is built
 * before right child.
 */
int
numLevels(Node *rootp)
{
    int rv = -1;
    if (!rootp) {
        return rv;
    }
    rv = 0;
    while (rootp->left) {
        rv++;
        rootp = rootp->left;
    }
    return rv;
}

// **** Helper functions ****
void
prArray(int *arr, int size)
{
    printf(" [ ");
    for (int ictr = 0; ictr < size; ictr++) {
        printf("%d ", arr[ictr]);
    }
    printf("]\n");

}

// **** Test cases ****

void
test_this(void)
{
    TEST_START();

    assert(1 == 1);
    TEST_END();
}

void
test_msg(const char *msg)
{
    TEST_START();

    const char *expmsg = "Hello World";
    assert(strncmp(expmsg, msg, strlen(expmsg)) == 0);
    TEST_END();
}

// Verifies mkNode(), prNode() and freeNode()
void
test_prNode(void)
{
    TEST_START();
    Node *np = mkNode(5);
    assert(np != NULL);
    prNode(np);
    freeNode(&np);
    assert(np == NULL);
    TEST_END();
}

// Basic verification of tree construction and print of 1-node tree
void
test_mkTree_1node(void)
{
    TEST_START();

    int values[] = {42};
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    freeTree(&rootp);
    TEST_END();
}

void
test_freeTree_1node(void)
{
    TEST_START();

    int values[] = {42};
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    freeTree(&rootp);
    assert(rootp == NULL);
    freeTree(&rootp);
    TEST_END();
}

void
test_mkTree_3nodes(void)
{
    TEST_START();

    int values[] = {42, 22, 33};
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    freeTree(&rootp);
    TEST_END();
}

void
test_mkTree_5nodes(void)
{
    TEST_START();

    int values[] = {42, 22, 33, 99, 112};
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    freeTree(&rootp);
    TEST_END();
}


void
test_mkTree_7nodes(void)
{
    TEST_START();

    int values[] = {42, 22, 33, 99, 112, 4, 55};
    assert(ARRAYSIZE(values) == 7);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    freeTree(&rootp);
    TEST_END();
}

void
test_mkTree_8nodes(void)
{
    TEST_START();

    int values[] = {42, 22, 33, 99, 112, 4, 55, 66};
    assert(ARRAYSIZE(values) == 8);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    freeTree(&rootp);
    TEST_END();
}

void
test_mkTree_9nodes(void)
{
    TEST_START();

    int values[] = {42, 22, 33, 99, 112, 4, 55, 66, 900};
    assert(ARRAYSIZE(values) == 9);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    freeTree(&rootp);
    TEST_END();
}

void
test_mkTree_random_10_nodes(void)
{
    TEST_START();

    int numNodes = 10;
    int values[numNodes];
    RAND_INIT();
    for (int ictr = 0; ictr < numNodes; ictr++) {
        values[ictr] = NEW_RAND();
    }
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    freeTree(&rootp);
    TEST_END();
}

void
test_prTree_9nodes_inorder(void)
{
    TEST_START();

    int values[] = {42, 22, 33, 99, 112, 4, 55, 66, 900};
    assert(ARRAYSIZE(values) == 9);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTreeTraverse(rootp, PR_TREE_INORDER);
    freeTree(&rootp);
    TEST_END();
}


void
test_prTree_9nodes_postorder(void)
{
    TEST_START();

    int values[] = {42, 22, 33, 99, 112, 4, 55, 66, 900};
    assert(ARRAYSIZE(values) == 9);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTreeTraverse(rootp, PR_TREE_POSTORDER);
    freeTree(&rootp);
    TEST_END();
}

void
test_numLevels_10_nodes(void)
{
    TEST_START();

    int values[] = {42, 22, 33, 99, 112, 4, 55, 66, 900, 1000};
    assert(ARRAYSIZE(values) == 10);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);

    const int expNumLevels = 3;
    assert(numLevels(rootp) == expNumLevels);
    freeTree(&rootp);
    TEST_END();
}

void
test_numLevels_random_128_nodes(void)
{
    TEST_START();

    int numNodes = 128;
    int values[numNodes];
    RAND_INIT();
    for (int ictr = 0; ictr < numNodes; ictr++) {
        values[ictr] = NEW_RAND();
    }
    Node *rootp = makeTree(values, ARRAYSIZE(values));

    // 128 nodes in a binary tree: [ 2**(l + 1) - 1] = 128
    const int expNumLevels = log2(numNodes * 1.0);
    int levels = numLevels(rootp);
    printf(" Exp # of levels=%d, Actual # of levels=%d", expNumLevels, levels);
    assert(numLevels(rootp) == expNumLevels);
    freeTree(&rootp);
    TEST_END();
}

void
test_prTree_Levelorder_3nodes(void)
{
    TEST_START();
    int values[] = {42, 22, 33};
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTreeTraverse(rootp, PR_TREE_LEVELORDER);
    freeTree(&rootp);
    TEST_END();
}

void
test_prTree_Levelorder_5nodes(void)
{
    TEST_START();
    int values[] = {42, 22, 33, 99, 112};
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    prTree(rootp);
    prTreeTraverse(rootp, PR_TREE_LEVELORDER);
    freeTree(&rootp);
    TEST_END();
}

void
test_prTree_Levelorder_7nodes(void)
{
    TEST_START();
    int values[] = {42, 22, 33, 99, 112, 4, 55};
    assert(ARRAYSIZE(values) == 7);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    prTree(rootp);
    prTreeTraverse(rootp, PR_TREE_LEVELORDER);
    freeTree(&rootp);
    TEST_END();
}

void
test_prTree_Levelorder_9nodes(void)
{
    TEST_START();
    int values[] = {42, 22, 33, 99, 112, 4, 55, 66, 900};
    assert(ARRAYSIZE(values) == 9);
    PRARRAY(values);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    prTree(rootp);
    prTreeTraverse(rootp, PR_TREE_LEVELORDER);
    freeTree(&rootp);
    TEST_END();
}

void
test_mkMinimalBinaryTree_1node(void)
{
    TEST_START();
    int values[] = {2};
    assert(ARRAYSIZE(values) == 1);
    PRARRAY(values);
    Node *rootp = mkMinimalBinaryTree(values, ARRAYSIZE(values));
    assert(rootp);

    prTree(rootp);
    assert(isValidBinTree(rootp));
    freeTree(&rootp);
    TEST_END();
}

void
test_mkMinimalBinaryTree_2nodes(void)
{
    TEST_START();
    int values[] = {2, 10};
    assert(ARRAYSIZE(values) == 2);
    PRARRAY(values);
    Node *rootp = mkMinimalBinaryTree(values, ARRAYSIZE(values));
    assert(rootp);

    prTree(rootp);
    assert(isValidBinTree(rootp));
    freeTree(&rootp);
    TEST_END();
}

void
test_mkMinimalBinaryTree_3nodes(void)
{
    TEST_START();
    int values[] = {2, 42, 83};
    assert(ARRAYSIZE(values) == 3);
    PRARRAY(values);
    Node *rootp = mkMinimalBinaryTree(values, ARRAYSIZE(values));
    assert(rootp);

    prTree(rootp);
    assert(isValidBinTree(rootp));
    freeTree(&rootp);
    TEST_END();
}

void
test_isValidBinTree(void)
{
    TEST_START();
    // Provide illegal values so output tree becomes invalid.
    int values[] = {42, 2, 83};
    assert(ARRAYSIZE(values) == 3);
    PRARRAY(values);
    Node *rootp = mkMinimalBinaryTree(values, ARRAYSIZE(values));
    assert(rootp);

    prTree(rootp);
    assert(isValidBinTree(rootp) == FALSE);
    freeTree(&rootp);
    TEST_END();
}

void
test_mkMinimalBinaryTree_4nodes(void)
{
    TEST_START();
    int values[] = {2, 42, 83, 84};
    assert(ARRAYSIZE(values) == 4);
    PRARRAY(values);
    Node *rootp = mkMinimalBinaryTree(values, ARRAYSIZE(values));
    assert(rootp);

    prTree(rootp);
    assert(isValidBinTree(rootp));
    freeTree(&rootp);
    TEST_END();
}

void
test_mkMinimalBinaryTree_5nodes(void)
{
    TEST_START();
    int values[] = {2, 42, 83, 84, 90};
    assert(ARRAYSIZE(values) == 5);
    PRARRAY(values);
    Node *rootp = mkMinimalBinaryTree(values, ARRAYSIZE(values));
    assert(rootp);

    prTree(rootp);
    assert(isValidBinTree(rootp));
    freeTree(&rootp);
    TEST_END();
}

void
test_mkMinBinaryTree_random_20_nodes(void)
{
    TEST_START();
    int numNodes = 20;
    int values[numNodes];
    assert(ARRAYSIZE(values) == 20);
    RAND_INIT();

    // Generate n-random unique, ascending ints
    for (int ictr = 0, randlow = 0; ictr < numNodes; ictr++) {
        int randhigh = (randlow + K_KILO);
        values[ictr] = RAND_INT_RANGE(randlow, randhigh);
        randlow = values[ictr] + 1;
    }
    PRARRAY(values);
    Node *rootp = mkMinimalBinaryTree(values, ARRAYSIZE(values));
    assert(rootp);

    prTree(rootp);
    assert(isValidBinTree(rootp));
    freeTree(&rootp);
    TEST_END();
}
