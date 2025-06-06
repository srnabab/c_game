#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_mutex.h"

#include "G_queue.h"

#ifndef G_THREAD_POOL_H
#define G_THREAD_POOL_H 1

#include "SDL3/SDL_begin_code.h"

#define TRACE_USED 1
#define TRACE_FREE 0
#define TRACE_DONE 2

typedef void (*TaskExecute)(void *);

struct _Range
{
    int startIndex;
    int endIndex;
};
typedef struct _Range Range;

struct _Trace
{
    int * threadIndices;
    int * taskAllDone;
    int threadUsedCount;
};
typedef struct _Trace Trace;

struct _G_Task
{
    TaskExecute executeFunc;
    void * func;
    void * arg;

    // private
    Range indexRange;

    // private
    int threadIndex;

    // private
    bool canRun;
};
typedef struct _G_Task G_Task;

struct _G_Thread_Pool
{
    // private
    SDL_Thread ** pThreads;
    // private
    SDL_Semaphore ** pThreadSeamphore;
    // private
    SDL_Semaphore ** pWaitTaskSemaphore;
    // private
    SDL_Semaphore * pWaitFreeThreadSemaphore;
    // private
    bool * leisureThread;

    // private
    SDL_Mutex * ThreadPoolMutex;
    // private
    G_Task * tasks;
    // private
    G_Queue traceQueue;
    // private
    int * doneWatch;

    // private
    int threadPoolSize;
    // private
    bool expandable;
    // private
    bool running;
    // private
    bool needThread;
};
typedef struct _G_Thread_Pool G_Thread_Pool;

struct _Thread_Func_Arg
{
    G_Thread_Pool * pThreadPool;
    SDL_Semaphore * tempSemaphore;
    Uint32 index;
};
typedef struct _Thread_Func_Arg Thread_Func_Arg;

extern bool SDLCALL createThreadPool(G_Thread_Pool * pThreadPool, Uint32 threadCount, const char * name, bool expandable);
extern int* SDLCALL G_AddTask(G_Thread_Pool * pThreadPool, int itemCount, int minRange, G_Task * pTask);
extern void SDLCALL destroyThreadPool(G_Thread_Pool * pThreadPool);
extern void SDLCALL G_WaitTask(G_Thread_Pool * pThreadPool, int * taskIndex);

#include "SDL3/SDL_close_code.h"

#endif //G_THREAD_POOL_H
