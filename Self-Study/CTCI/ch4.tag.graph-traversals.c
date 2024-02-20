/*
 * -----------------------------------------------------------------------------
 * Implement Graph building and traversals algorithms.
 *
 * Ref:
 *
 * Usage: gcc -o ch4.tag.graph-traversals ch4.tag.graph-traversals.c
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// -----------------------------------------------------------------------------
// Useful typedefines
typedef unsigned int uint32;

// -----------------------------------------------------------------------------
// Limits for this program
const int Num_nodes = 1000; // Graph Capacity: # nodes we can track in a graph

// -----------------------------------------------------------------------------
// Define a node in a graph, giving its identity and list of connected nodes
typedef struct graphnode
{
    int     id;
    uint32  degree;
    int *   tonodes;    // Allocated memory; needs to be free'ed.
} GRAPHNODE;

// Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(const char *msg);
void test_prEmptyGraphNode(void);
void test_prGraphNode(void);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
                          { "test_this"                 , test_this }
                        , { "test_that"                 , test_that }
                        , { "test_prEmptyGraphNode"     , test_prEmptyGraphNode }
                        , { "test_prGraphNode"          , test_prGraphNode }
                      };

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

int
main(int argc, char *argv[])
{
    const char *hello_msg = "Hello World";
    printf("%s: %s. (argc=%d)\n", argv[0], hello_msg, argc);

    int rv = 0;
    // Run all test cases if no args are provided.
    if (argc == 1) {
        for (int tctr = 0; tctr < ARRAYSIZE(Test_fns); tctr++) {
            Test_fns[tctr].tfn();
        }
    } else if (strncmp("--help", argv[1], strlen("--help")) == 0) {
        printf(Usage, argv[0]);
        return rv;
    } else if (strncmp("test_", argv[1], strlen("test_")) == 0) {

        // Execute the named test-function, if it's a supported test-function
        int tctr = 0;
        for (; tctr < ARRAYSIZE(Test_fns); tctr++) {
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

// **** Graph Manipulation Routines ****

void
prGRAPHNODE(GRAPHNODE *nodep)
{
    if (!nodep) {
        return;
    }
    printf("ID=%d, degree=%u", nodep->id, nodep->degree);
    if (!nodep->degree) {
        printf(" []\n");
        return;
    }
    printf(" [");
    char sep = ' ';
    for (int nctr = 0; nctr < nodep->degree; nctr++) {
        printf("%c%d", sep, nodep->tonodes[nctr]);
        sep = ',';
    }
    printf(" ]\n");
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
test_that(void)
{
    TEST_START();
    test_msg("Hello World");
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

void
test_prEmptyGraphNode(void)
{
    GRAPHNODE gnode = {0};
    gnode.id = 1;
    prGRAPHNODE(&gnode);
}


void
test_prGraphNode(void)
{
    GRAPHNODE gnode = {0};
    gnode.id = 1;

    gnode.degree = 3;
    int tonodes[gnode.degree];
    for (int nctr = 0; nctr < gnode.degree; nctr++) {
        tonodes[nctr] = (nctr + 10);
    }
    gnode.tonodes = tonodes;

    prGRAPHNODE(&gnode);
}
