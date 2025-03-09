#include "G_test.h"
#define STACK_TEST 1

#include "G_stack.h"

void TestAll(void)
{
#ifdef STACK_TEST
    stackTest();
#endif
}