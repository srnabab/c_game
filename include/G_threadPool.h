#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_mutex.h"

#include "G_queue.h"

#ifndef G_THREAD_POOL_H
#define G_THREAD_POOL_H 1

#include "SDL3/SDL_begin_code.h"

typedef void (*TaskExecute)(void *);

struct _G_Task
{
    TaskExecute executeFunc;
    void * func;
    void * arg;
};
typedef struct _G_Task G_Task;

struct _G_Thread_Pool
{
    SDL_Thread ** pThreads;
    SDL_Semaphore ** pThreadSeamphore;
    bool * leisureThread;

    SDL_Mutex * ThreadPoolMutex;
    G_Queue  taskQueue;

    bool expandable;
    bool running;
    int threadPoolSize;

    int * indices;
};
typedef struct _G_Thread_Pool G_Thread_Pool;

struct _Thread_Func_Arg
{
    G_Thread_Pool * pThreadPool;
    Uint32 index;
};
typedef struct _Thread_Func_Arg Thread_Func_Arg;

extern bool SDLCALL createThreadPool(G_Thread_Pool * pThreadPool, Uint32 threadCount, bool expandable);
extern int* SDLCALL G_AddTask(G_Thread_Pool * pThreadPool, G_Task * pTask);
extern void SDLCALL destroyThreadPool(G_Thread_Pool * pThreadPool);
extern void SDLCALL G_WaitTask(G_Thread_Pool * pThreadPool, int taskIndex);

#include "SDL3/SDL_close_code.h"

#endif //G_THREAD_POOL_H

#if defined(THREAD_POOL_TEST)

#include "SDL3/SDL_test.h"

typedef struct task1Arg
{
    int a;
    float b;
    int * res;
} task1Arg;
static void task1(int a, float b, int * res)
{
    *res = (int)((float)a * b);
}
static void execute1(void * arg)
{
    void (*func)(int, float, int*) = (void (*)(int, float, int*))((G_Task*)arg)->func;
    func(((task1Arg*)((G_Task*)arg)->arg)->a, ((task1Arg*)((G_Task*)arg)->arg)->b, ((task1Arg*)((G_Task*)arg)->arg)->res);
}
static void task2(void)
{
    SDL_Delay(1000);
}
static void execute2(void * arg)
{
    (void)arg;
    task2();
}
int threadPoolTest(void)
{
    int res, passed, total, failed;
    passed = total = failed = 0;
    SDL_Log("-------------------------------------------------------------");
    SDL_Log("threadPool test\n");

    total++;
    G_Thread_Pool threadPool;
    createThreadPool(&threadPool, 8, false);

    for (Uint32 i = 0;i < 8;i++)
    {
        SDLTest_AssertCheck(threadPool.leisureThread[i] == true, "thread after init is all wait");
    }

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("threadPool test passed");
        passed++;
    }
    else 
    {
        SDL_Log("threadPool test failed");
        if (!failed) failed = passed + 1;
    }

    // single task test with result
    total++;
    G_Task tasks = {0};
    tasks.executeFunc = execute1;
    tasks.func = (void*)task1;
    int taskRes = 0;
    task1Arg task1Args = { 3, 3.6, &taskRes };
    tasks.arg = (void *)&task1Args;
    int * pIndex = G_AddTask(&threadPool, &tasks);

    G_WaitTask(&threadPool, *pIndex); 

    SDLTest_AssertCheck(taskRes == 10, "task execute test");

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("single task test with result passed");
        passed++;
    }
    else 
    {
        SDL_Log("single task test with result passed");
        if (!failed) failed = passed + 1;
    }

    total++;
    pIndex = G_AddTask(&threadPool, &tasks);
    G_Task tasks2 = {0};
    tasks2.executeFunc = execute2;
    tasks2.func = NULL;
    tasks2.arg = NULL;
    int * pIndex2 = G_AddTask(&threadPool, &tasks2);

    G_Task tasks3 = {0};
    tasks3.executeFunc = execute2;
    tasks3.func = NULL;
    tasks3.arg = NULL;
    int * pIndex3 = G_AddTask(&threadPool, &tasks3);

    G_Task tasks4 = {0};
    tasks4.executeFunc = execute2;
    tasks4.func = NULL;
    tasks4.arg = NULL;
    int * pIndex4 = G_AddTask(&threadPool, &tasks4);

    G_Task tasks5 = {0};
    tasks5.executeFunc = execute2;
    tasks5.func = NULL;
    tasks5.arg = NULL;
    int * pIndex5 = G_AddTask(&threadPool, &tasks5);

    G_Task tasks6 = {0};
    tasks6.executeFunc = execute2;
    tasks6.func = NULL;
    tasks6.arg = NULL;
    int * pIndex6 = G_AddTask(&threadPool, &tasks6);

    G_Task tasks7 = {0};
    tasks7.executeFunc = execute2;
    tasks7.func = NULL;
    tasks7.arg = NULL;
    int * pIndex7 = G_AddTask(&threadPool, &tasks7);

    G_Task tasks8 = {0};
    tasks8.executeFunc = execute2;
    tasks8.func = NULL;
    tasks8.arg = NULL;
    int * pIndex8 = G_AddTask(&threadPool, &tasks8);

    G_WaitTask(&threadPool, *pIndex8);
    G_WaitTask(&threadPool, *pIndex7);
    G_WaitTask(&threadPool, *pIndex6);
    G_WaitTask(&threadPool, *pIndex5);
    G_WaitTask(&threadPool, *pIndex4);
    G_WaitTask(&threadPool, *pIndex3);
    G_WaitTask(&threadPool, *pIndex2);
    G_WaitTask(&threadPool, *pIndex);
    SDLTest_AssertCheck(taskRes == 10, "task execute test");

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("many task test with result passed");
        passed++;
    }
    else 
    {
        SDL_Log("many task test with result passed");
        if (!failed) failed = passed + 1;
    }
    destroyThreadPool(&threadPool);

    SDLTest_LogAssertSummary();
    if (res == TEST_RESULT_PASSED) SDL_Log("\nstack test passed(%d / %d)", passed, total);
    else SDL_Log("\nstack test failed(%d / %d), first failed: %d", passed, total, failed);

    SDL_Log("-------------------------------------------------------------\n");

    return res;
}

#endif