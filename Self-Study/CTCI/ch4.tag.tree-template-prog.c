/*
 * -----------------------------------------------------------------------------
 * Implement basic Tree Traversal algorithms
 *
 * Ref:
 *
 * Usage: gcc -o ch4.tag.tree-template-prog ch4.tag.tree-template-prog.c
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
    struct node *   next;
    int             data;
} Node;

// Function Prototypes
Node *mkNode(const int val);
void freeNode(Node **np);
void prNode(const Node *np);

void test_this(void);
void test_msg(const char *msg);
void test_prNode(void);

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
    np->next = NULL;
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
    printf("np=%p {next=%p, data=%d}\n", np, np->next, np->data);
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
