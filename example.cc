/**
 * A sample of my micro unit test library
 */
#include <string>
#include "mut.h"
using namespace std;

// Define some functions to test ...

int get_max(int x, int y)
{
    return (x > y) ? x : y;
}

bool is_even(int n)
{
    return !(n & 0x01);
}

// Register some test cases

MICRO_UNIT(Test_get_max)
{
    ASSERT_EQ(get_max(3, 5), 5)
    ASSERT_NE(get_max(0, 2), 0)
}

MICRO_UNIT(Test_is_even)
{
    ASSERT_TRUE(is_even(127))
}

MICRO_UNIT(Test_add)
{
    ASSERT_NE(1 + 2, 4)
}

MICRO_UNIT(Test_strequ)
{
    ASSERT_EQ(string("123"), string("abc"))
}

MICRO_UNIT(Test_const)
{
    MUT_DEBUG << "This is debug message" << std::endl;
    ASSERT_FALSE((1024 << 2) == 2048)
    if (float(1.0000000001f) == float(1.0f)) {
        MUT_PASS()
    }
}


MICRO_UNIT_TEST_MAIN
{
    bool ok = mut::unit_tester::run();
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}




