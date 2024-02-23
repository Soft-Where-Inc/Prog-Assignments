/*
 * -----------------------------------------------------------------------------
 * Implement <describe problem being implemented>
 *
 * Ref:
 *
 * Usage: gcc -o template-program-c template-program.c
 *        ./template-program-c [--help | test_<something> | test_<prefix> ]
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(const char *msg);

// -----------------------------------------------------------------------------
// List of test functions one can invoke from the command-line
typedef struct test_fns
{
    const char *    tfn_name;
    void            (*tfn)(void);
} TEST_FNS;

TEST_FNS Test_fns[] = {
                          { "test_this"     , test_this }
                        , { "test_that"     , test_that }
                      };

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

/*
 * *****************************************************************************
 * main()
 * *****************************************************************************
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
        test_msg(hello_msg);
        for (int tctr = 0; tctr < ARRAYSIZE(Test_fns); tctr++) {
            Test_fns[tctr].tfn();
        }
    } else if (strncmp("--help", argv[1], strlen("--help")) == 0) {
        printf(Usage, argv[0]);
        return rv;
    } else if (strncmp("test_", argv[1], strlen("test_")) == 0) {

        // Execute the named test-function, if it's a supported test-function
        int tctr = 0;
        int ntests = 0;
        for (; tctr < ARRAYSIZE(Test_fns); tctr++) {
            if (!strncmp(Test_fns[tctr].tfn_name, argv[1], strlen(argv[1]))) {
                Test_fns[tctr].tfn();
                ntests++;
            }
        }
        if (tctr == ARRAYSIZE(Test_fns) && !ntests) {
            printf("Warning: Named test-function '%s' not found.\n", argv[1]);
            rv = 1;
        }
    } else {
        printf("Unknown argument: '%s'\n", argv[1]);
        rv = 1;
    }

    return rv;
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
}
