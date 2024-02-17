/*
 * -----------------------------------------------------------------------------
 * Implement basic Tree Traversal algorithms
 *
 * Ref:
 *
 * Usage: $ gcc -o ch4.tag.tree-traversals ch4.tag.tree-traversals.c
 *        $ leaks -atExit -- ./ch4.tag.tree-traversals
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

typedef unsigned int uint32;

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

// Function Prototypes
Node *mkNode(const int val);
void freeNode(Node **np);
void prNode(const Node *np);

// values is an array of 'nitems' ints.
Node *makeTree(int *values, int nitems);

void mkTree(Node *qOfNodes[], int qsize, int *values, int nitems);

void prTree(Node *rootp);
void prTreeRecurse(Node *rootp, uint32 level, char nodeType);
void prNodeLevel(Node *rootp, uint32 level, char nodeType);

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

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
                          { "test_prNode"           , test_prNode }
                        , { "test_mkTree_1node"     , test_mkTree_1node }
                        , { "test_freeTree_1node"   , test_freeTree_1node }
                        , { "test_mkTree_3nodes"    , test_mkTree_3nodes }
                        , { "test_mkTree_5nodes"    , test_mkTree_5nodes }
                        , { "test_mkTree_7nodes"    , test_mkTree_7nodes }
                        , { "test_mkTree_8nodes"    , test_mkTree_8nodes }
                        , { "test_mkTree_9nodes"    , test_mkTree_9nodes }
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
 * prTree() - Tree printing routine.
 * -----------------------------------------------------------------------------
 */
void
prTree(Node *rootp)
{
    if (!rootp)
        return;

    printf("\nTree at rootp=%p\n", rootp);
    prTreeRecurse(rootp, 0, 'R');
}

void
prTreeRecurse(Node *nodep, uint32 level, char nodeType)
{
    assert(nodep != NULL);

    prNodeLevel(nodep, level, nodeType);
    if (nodep->left) {
        prTreeRecurse(nodep->left, (level + 1), 'l');
    }
    if (nodep->right) {
        prTreeRecurse(nodep->right, (level + 1), 'r');
    }
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
