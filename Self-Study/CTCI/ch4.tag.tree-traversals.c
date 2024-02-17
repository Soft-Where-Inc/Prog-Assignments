/*
 * -----------------------------------------------------------------------------
 * Implement basic Tree Traversal algorithms
 *
 * Ref:
 *
 * Usage: gcc -o ch4.tag.tree-traversals ch4.tag.tree-traversals.c
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

/*
 * Definition of basic tree Node struct.
 */
typedef struct node {
    struct node *   left;
    struct node *   right;
    int             data;
} Node;

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Function Prototypes
Node *mkNode(const int val);
void freeNode(Node **np);
void prNode(const Node *np);

// values is an array of 'nitems' ints.
Node *makeTree(int *values, int nitems);

void mkTree(Node *qOfNodes[], int qsize, int *values, int nitems);

void prTree(Node *rootp);

void test_this(void);
void test_msg(const char *msg);
void test_prNode(void);
void test_mkTree_1node(void);

int
main(int argc, char *argv[])
{
    const char *hello_msg = "Hello World";
    printf("%s: %s. (argc=%d)\n", argv[0], hello_msg, argc);

    // Run all test cases if no args are provided.
    if (argc == 1) {
        test_this();
        // test_msg(hello_msg);
        test_prNode();
        test_mkTree_1node();
    } else if (strncmp("--help", argv[1], strlen("--help")) == 0) {
        printf(Usage, argv[0]);
        return 0;
    } else if (strncmp("test_", argv[1], strlen("test_")) == 0) {
    } else {
        printf("Unknown argument: '%s'\n", argv[1]);
    }

    return 0;
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
 * prTree() - Tree printing routine.
 * -----------------------------------------------------------------------------
 */
void
prTree(Node *rootp)
{
    if (!rootp)
        return;

    printf("[%d]\n", rootp->data);
    if (rootp->left) {
        prTree(rootp->left);
    }
    if (rootp->right) {
        prTree(rootp->right);
    }
}

// **** Test cases ****

void
test_this(void)
{
    printf("%s", __func__);

    assert(1 == 1);
    printf(" ... OK\n");
}

void
test_msg(const char *msg)
{
    printf("%s", __func__);

    const char *expmsg = "Hello World";
    assert(strncmp(expmsg, msg, strlen(expmsg)) == 0);
    printf(" ... OK\n");
}

// Verifies mkNode(), prNode() and freeNode()
void
test_prNode(void)
{
    printf("%s ", __func__);
    Node *np = mkNode(5);
    assert(np != NULL);
    prNode(np);
    freeNode(&np);
    assert(np == NULL);
    printf(" ... OK\n");
}

// Basic verification of tree construction and print of 1-node tree
void
test_mkTree_1node(void)
{
    printf("%s", __func__);

    int values[] = {42};
    assert(1 == 1);
    Node *rootp = makeTree(values, ARRAYSIZE(values));
    assert(rootp);
    prTree(rootp);
    printf(" ... OK\n");
}
