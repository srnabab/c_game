#include "G_test.h"
// #define STACK_TEST 1
// #define QUEUE_TEST 1
#define THREAD_POOL_TEST 1

#include "G_stack.h"
#include "G_queue.h"
#include "G_threadPool.h"

int TestAll(void)
{
    int res = 0;

#ifdef STACK_TEST
    res = stackTest();
    if (res) return res;
#endif

#ifdef QUEUE_TEST
    res = queueTest(); 
    if (res) return res;

    res = queueReiszeTest();
    if (res) return res;
#endif

#ifdef THREAD_POOL_TEST
    res = threadPoolTest();
    if (res) return res;
#endif

    return res;
}