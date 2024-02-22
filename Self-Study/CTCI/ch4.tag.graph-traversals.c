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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// -----------------------------------------------------------------------------
// Useful typedefines
typedef unsigned int uint32;

// -----------------------------------------------------------------------------
// Limits for this program

// Graph Capacity: # nodes we can track in a graph
const int Max_Num_Nodes = 1000;

// -----------------------------------------------------------------------------
// Define a node in a graph, giving its identity and list of connected nodes
typedef struct graphnode
{
    int     id;
    uint32  degree;
    int *   tonodes;    // Allocated memory; needs to be free'ed.
} GRAPHNODE;

// Graph Function Prototypes
GRAPHNODE * buildGraph(uint32 numnodes, int *nodeids, int *nto_nodes,
                       int **to_nodes);
GRAPHNODE * mkNodesArray(uint32 numnodes);
int *       mkToNodesArray(uint32 degree);
void        freeGraph(GRAPHNODE **nodep, uint32 numnodes);

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(const char *msg);
void test_prEmptyGraphNode(void);
void test_prGraphNode(void);
void test_buildGraph_1node(void);

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
                        , { "test_buildGraph_1node"     , test_buildGraph_1node }
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

/*
 * -----------------------------------------------------------------------------
 * buildGraph(): Graph constructor.
 *
 * Receive inputs describing the nodes and edges in a graph and construct a
 * graph structure.
 *
 * This will:
 *  - Allocate an array of 'n' nodes in the graph.
 *  - For each node, allocate an array of 't' tonodes[] array of connected nodes
 *  - Builds each node in the graph
 *
 * Parameters:
 *  numnodes    - Number of nodes in this graph.
 *  nodeids     - Array of IDs of nodes in the graph
 *  nto_nodes   - Array of # of 'to' nodes for each node in nodeids[]
 *  to_nodes    - Array of ptrs to arrays of 'to' nodes for each node.
 *
 * NOTE: Caller should guarantee that all arrays are sized correctly.
 *       We do not perform any further data structure validation in this fn.
 *
 * Returns: Ptr to start of GRAPHNODES[] array (sized by 'n' nodes in the graph).
 * -----------------------------------------------------------------------------
 */
GRAPHNODE *
buildGraph(uint32 numnodes, int *nodeids, int *nto_nodes, int **to_nodes)
{
    // Validate, briefly, input arguments
    assert(numnodes < Max_Num_Nodes);
    assert(nodeids);
    assert(nto_nodes);
    assert(to_nodes);

    GRAPHNODE *nodes = NULL;
    if ((nodes = mkNodesArray(numnodes)) == NULL) {
        return nodes;
    }

    // Fill-up node-IDs in all nodes in the graph
    for (uint32 nctr = 0; nctr < numnodes; nctr++) {
        nodes[nctr].id = nodeids[nctr];

        // Fill-up the 'to' nodes info, allocating memory if needed
        nodes[nctr].degree = nto_nodes[nctr];
        uint32 degree = nodes[nctr].degree;
        if (degree) {
            nodes[nctr].tonodes = mkToNodesArray(degree);

            int *tonodes = nodes[nctr].tonodes;
            int *tonodes_inp = to_nodes[nctr];
            for (uint32 dctr = 0; dctr < degree; dctr++) {
                tonodes[dctr] = tonodes_inp[dctr];
            }
        }
    }

    return nodes;
}

// -----------------------------------------------------------------------------
GRAPHNODE *
mkNodesArray(uint32 numnodes)
{
    assert(numnodes < Max_Num_Nodes);

    GRAPHNODE *nodes = NULL;
    size_t size = (numnodes * sizeof(*nodes));
    nodes = malloc(size);
    if (!nodes) {
        return nodes;
    }
    memset(nodes, 0, size);
    return nodes;
}

// -----------------------------------------------------------------------------
int *
mkToNodesArray(uint32 degree)
{
    assert(degree < Max_Num_Nodes);

    int *tonodes = NULL;
    size_t size = (degree * sizeof(*tonodes));
    tonodes = malloc(size);
    if (!tonodes) {
        return tonodes;
    }
    memset(tonodes, 0, size);
    return tonodes;
}

void
freeGraph(GRAPHNODE **nodep, uint32 numnodes)
{
    GRAPHNODE *nodes = (nodep ? *nodep : (GRAPHNODE *) NULL);
    if (!nodes || !numnodes) {
        return;
    }

    for (int nctr = 0; nctr < numnodes; nctr++) {
        // Free memory allocated for tonodes[] array
        if (nodes[nctr].degree) {
            free(nodes[nctr].tonodes);
            nodes[nctr].tonodes = NULL;
        }
    }
    free(nodes);
    *nodep = (GRAPHNODE *) NULL;
}

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
    TEST_START();
    GRAPHNODE gnode = {0};
    gnode.id = 1;
    prGRAPHNODE(&gnode);
    TEST_END();
}

void
test_prGraphNode(void)
{
    TEST_START();

    GRAPHNODE gnode = {0};
    gnode.id = 1;

    gnode.degree = 3;
    int tonodes[gnode.degree];
    for (int nctr = 0; nctr < gnode.degree; nctr++) {
        tonodes[nctr] = (nctr + 10);
    }
    gnode.tonodes = tonodes;

    prGRAPHNODE(&gnode);
    TEST_END();
}

void
test_buildGraph_1node(void)
{
    TEST_START();

    uint32 numnodes = 1;
    int    nodeids[] = {1};
    int    nto_nodes[] = {2};
    int    *to_nodes[1];

    // Initialize the array of to[] nodes that each node is pointing to.
    to_nodes[0] = {1, 2};

    TEST_END();
}
