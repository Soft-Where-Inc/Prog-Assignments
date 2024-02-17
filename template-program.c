/*
 * -----------------------------------------------------------------------------
 * Implement <describe problem being implemented>
 *
 * Ref:
 *
 * Usage: gcc -o template-program-c template-program.c
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

// Function Prototypes
void test_this(void);
void test_msg(const char *msg);

int
main(int argc, char *argv[])
{
    const char *hello_msg = "Hello World";
    printf("%s: %s. (argc=%d)\n", argv[0], hello_msg, argc);

    // Run all test cases if no args are provided.
    if (argc == 1) {
        test_this();
        test_msg(hello_msg);
    } else if (strncmp("--help", argv[1], strlen("--help")) == 0) {
        printf(Usage, argv[0]);
        return 0;
    } else if (strncmp("test_", argv[1], strlen("test_")) == 0) {
    } else {
        printf("Unknown argument: '%s'\n", argv[1]);
    }

    return 0;
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
