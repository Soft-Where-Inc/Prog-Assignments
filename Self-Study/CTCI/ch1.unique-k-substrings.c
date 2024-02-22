/*
 * -----------------------------------------------------------------------------
 * Implement an algorithm to find longest substring with 'k' unique chars
 * from any string over an alphabet. (Say 'a-zZ-Z0-9') Return start of
 * the longest such sub-string if it exists. NULL otherwise.
 *
 * Ref:
 *
 * Usage: gcc -o ch1.unique-k-substrings ch1.unique-k-substrings.c
 *
 * History:
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

const char *Usage = "%s [ --help | test_<fn-name> ]\n";

#define ARRAYSIZE(arr) ((int) (sizeof(arr) / sizeof(*arr)))

typedef unsigned int    uint32;
typedef int             bool;

#ifndef TRUE
#define TRUE    1
#endif  // TRUE

#ifndef FALSE
#define FALSE    0
#endif  // FALSE

// String Function Prototypes
bool
char_in_unique_k_set(const char *sp, char ch, int *unique_k, int found_k);

void
update_curr_char_in_unique_k(const char *sp, char *ch, int *unique_k, int found_k);

void
replace_curr_char_in_unique_k(const char *sp, char *ch, int *unique_k, int found_k);

// Test Function Prototypes
void test_this(void);
void test_that(void);
void test_msg(const char *msg);

void test_null_input(void);
void test_k_gt_strlen(void);

void test_nchars_in_alphabet(void);

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
                    , { "test_nchars_in_alphabet"   , test_nchars_in_alphabet }
                    , { "test_null_input"           , test_null_input }
                    , { "test_k_gt_strlen"          , test_k_gt_strlen }
    };

// Test start / end info-msg macros
#define TEST_START()  printf("%s ", __func__)
#define TEST_END()    printf(" ...OK\n")

// ---- ASSUMPTIONS ----

// Alphabet is of known finite size; say, [a-zA-Z0-9]
#define NUM_CHARS_IN_ALPHABET   (  ('z' - 'a' + 1)      \
                                 + ('Z' - 'A' + 1)      \
                                 + ('9' - '0' + 1) )

/*
 * Find and return the start of the longest sub-string with 'k' unique
 * characters occurring in input string 'sp'.
 *
 * Algorithm:
 *  - Walk the string from its head, one char @ a time.
 *
 *  - Track the 'most recently seen' k-unique chars, but rather than
 *    tracking the char itself, track the highest index of the char
 *    as seen while traversing the string. Call this k_indexes[].
 *
 *  - Maintain int found_k which will "count" the # of unique chars
 *    we've seen so far.
 *
 *  - If (found_k == k); i.e. we have found our 'unique set of chars for this
 *    run', then if we find another char, we have to replace the char with
 *    the smallest index. The start of the run will be reset to the index of
 *    next unique char.
 *      (Assert: index-of-char-being-replaced + 1) == index-of-next=unique-char)
 *
 *  - Before replacing a unique-char that is being 'aged-out of the sliding
 *    window', track the longest-length-of-substring-so-far, and its start
 *    offset. Keep track of the potential longest-length in case the new
 *    run of unique-chars sub-string is shorter than previously found run.
 */
char *
longest_substr_k(const char *sp, const uint32 k, uint32 *substr_len)
{
    // Handle lapsed cases ...
    if (!sp || (k == 0) || (k > strlen(sp))) {
        return (char *) NULL;
    }

    if (k == 1) {
        // Handle this special-case differently; To be implemented.
        return (char *) sp;
    }

    // Declare tracker items
    int    unique_k[k];
    uint32 found_k = 0;

    // Initialize to -1 as 0 is a valid index in the string.
    memset(unique_k, -1, sizeof(unique_k));

    const char *endp = (sp + strlen(sp));

    char *start_substr = (char *)sp;  // Start of potentially longest substr-so-far
    int   longest = 0;

    // Process each char in the string, maintaining results-so-far as we find them.
    char *curr = (char *) sp;
    while (curr < endp) {

        if (!char_in_unique_k_set(sp, *curr, unique_k, found_k)) {
            if (found_k < k) {
                unique_k[found_k] = (curr - sp);
                found_k++;
                longest++;

            } else {
                // Implement replacement algorithm ... Come back to it.
                replace_curr_char_in_unique_k(sp, curr, unique_k, found_k);
            }
        } else {
            update_curr_char_in_unique_k(sp, curr, unique_k, found_k);
        }
        curr++;
    }

    return (char *) NULL;
}

/*
 * char_in_unique_k_set(): Is the newly found character 'ch' already in the current
 * set of up-to-k-unique chars found so far?
 */
bool
char_in_unique_k_set(const char *sp, char ch, int *unique_k, int found_k)
{
    if (!found_k || !unique_k) {
        return FALSE;
    }
    for (int ictr = 0 ; ictr < found_k; ictr++) {
        if (*(sp + unique_k[ictr]) == ch) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * update_curr_char_in_unique_k():
 *
 * Search has previously found current-character to "be in the set of unique-k"
 * characters. Replace that character's previously-found-index to the index of the
 * current character. Shuffle this set of indexes so that they are always in
 * increasing order. In other words, the most-recently-found char in this unique-set
 * will always appear "last" in this set, which is tracking the char's index.
 */
void
update_curr_char_in_unique_k(const char *sp, char *ch, int *unique_k, int found_k)
{
    int cctr = 0;
    while (sp[unique_k[cctr]] != *ch) {
        cctr++;
    }

    // 'ch' is the most-recently-seen character, So shuffle all previously-seen
    // characters' indexes up, and move this char to the end of the unique set.
    while (cctr < (found_k - 1)) {
        unique_k[cctr] = unique_k[cctr + 1];
    }

    // Stash-away the index for the most-recently-instance of this unique char
    unique_k[cctr] = (ch - sp);
}

void
replace_curr_char_in_unique_k(const char *sp, char *ch, int *unique_k, int found_k)
{
}

// -----------------------------------------------------------------------------
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
test_null_input()
{
    TEST_START();

    const char *s = NULL;
    char *rv = longest_substr_k(s, 1, (uint32 *) NULL);
    assert(rv == NULL);
    TEST_END();
}

void
test_k_gt_strlen()
{
    TEST_START();

    const char *s = "abc";
    char *rv = longest_substr_k(s, 4, (uint32 *) NULL);
    assert(rv == NULL);
    TEST_END();
}
void
test_nchars_in_alphabet()
{
    TEST_START();

    assert(NUM_CHARS_IN_ALPHABET == (26 + 26 + 10));
    TEST_END();
}
