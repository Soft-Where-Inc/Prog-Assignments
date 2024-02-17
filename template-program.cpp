/*
 * -----------------------------------------------------------------------------
 * Implement <describe problem being implemented>
 *
 * Ref:
 *
 * Usage: g++ -o template-program-cpp template-program.cpp
 *
 * History:
 * -----------------------------------------------------------------------------
 */

#include <iostream>

using namespace std;

string Usage = " [ --help | test_<fn-name> ]\n";

// Function Prototypes
void test_this(void);
void test_msg(string&);

int
main(const int argc, const char *argv[])
{
    string hello_msg = "Hello World.";
    cout << argv[0] << ": " << hello_msg << " (argc=" << argc << ")" << endl;

    // Run all test cases if no args are provided.
    if (argc == 1) {
        test_this();
        test_msg(hello_msg);
    } else if (strncmp("--help", argv[1], strlen("--help")) == 0) {
        cout << argv[0] << Usage << endl;
        return 0;
    } else if (strncmp("test_", argv[1], strlen("test_")) == 0) {
    } else {
        cout << "Unknown argument: " << argv[1] << endl;
    }
}

// **** Test cases ****

void
test_this(void)
{
    cout << __func__;

    assert(1 == 1);
    cout << " ... OK" << endl;
}

void
test_msg(string& msg)
{
    cout << __func__;

    assert(msg == "Hello World.");
    cout << " ... OK" << endl;
}

