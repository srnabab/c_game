#include "G_test.h"
#define STACK_TEST 0
#define QUEUE_TEST 0
#define THREAD_POOL_TEST 0

#include "G_stack.h"
#include "G_queue.h"
#include "G_threadPool.h"

int TestAll(void)
{
    int res = 0;

#if STACK_TEST
    res = stackTest();
    if (res) return res;
#endif

#if QUEUE_TEST
    res = queueTest(); 
    if (res) return res;

    res = queueReiszeTest();
    if (res) return res;
#endif

#if THREAD_POOL_TEST
    res = threadPoolTest();
    if (res) return res;
#endif

    return res;
}