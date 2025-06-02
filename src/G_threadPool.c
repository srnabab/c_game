#include "G_threadPool.h"
#include "G_log.h"
#include "G_allocator.h"
#include "SDL3/SDL_timer.h"

static int threadFunc(void * data)
{
    Uint32 index = ((Thread_Func_Arg*)data)->index;
    G_Thread_Pool * pThreadPool = ((Thread_Func_Arg*)data)->pThreadPool;
    bool running = pThreadPool->running;
    SDL_Semaphore * semaphore = pThreadPool->pThreadSeamphore[index];
    SDL_Semaphore * waitSemaphore = pThreadPool->pWaitTaskSemaphore[index];
    G_Task task;

    SDL_SignalSemaphore(((Thread_Func_Arg*)data)->tempSemaphore);

    while (running)
    {
        SDL_WaitSemaphore(semaphore);

        // get task
        memcpy(&task, pThreadPool->tasks + index, sizeof(G_Task));

        // run task
        if (task.canRun) 
        {
            task.executeFunc(&task);
            
        }

        SDL_LockMutex(pThreadPool->ThreadPoolMutex);

        pThreadPool->tasks[index].canRun = false;
        pThreadPool->leisureThread[index] = true;
        running = pThreadPool->running;

        SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

        SDL_SignalSemaphore(waitSemaphore);
        // SDL_Log("TIME: %10llu--Thread: %d Signal", SDL_GetTicksNS(), index);
    }

    return 0;
}
static int processTrace(void * data)
{
    G_Thread_Pool * pThreadPool = ((Thread_Func_Arg*)data)->pThreadPool;
    bool running = pThreadPool->running;
    G_Queue * pQueue = &pThreadPool->traceQueue; 
    Trace innerTrace;
    bool res;
    int i;

    SDL_SignalSemaphore(((Thread_Func_Arg*)data)->tempSemaphore);

    while (running)
    {
        res = pQueue->getHead(pQueue, &innerTrace);

        if (res)
        {
            for (i = 0;i < innerTrace.threadUsedCount;i++)
            {
                SDL_WaitSemaphore(pThreadPool->pWaitTaskSemaphore[innerTrace.threadIndices[i]]);
            }

            *innerTrace.taskAllDone = TRACE_DONE;
            // print("trace inner indices: %p(free)", innerTrace.threadIndices);
            G_free(innerTrace.threadIndices);
            innerTrace.threadIndices = NULL;
        }
        else 
        {
            SDL_Delay(1);
        }

        SDL_LockMutex(pThreadPool->ThreadPoolMutex);

        running = pThreadPool->running;

        SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
    }

    return 0;
}
bool createThreadPool(G_Thread_Pool * pThreadPool, Uint32 threadCount, bool expandable)
{
    pThreadPool->pThreads = (SDL_Thread**)G_malloc((threadCount + 1) * sizeof(SDL_Thread*));
    if (pThreadPool->pThreads == NULL)
    {
        return false;
    }
    pThreadPool->pThreadSeamphore = (SDL_Semaphore**)G_malloc(threadCount * sizeof(SDL_Semaphore*));
    if (pThreadPool->pThreadSeamphore == NULL)
    {
        G_free(pThreadPool->pThreads);
        return false;
    }
    pThreadPool->pWaitTaskSemaphore = (SDL_Semaphore**)G_malloc(threadCount * sizeof(SDL_Semaphore*));
    if (pThreadPool->pWaitTaskSemaphore  == NULL)
    {
        G_free(pThreadPool->pThreads);
        G_free(pThreadPool->pThreadSeamphore);
        return false;
    }
    pThreadPool->tasks = (G_Task*)G_malloc(threadCount * sizeof(G_Task));
    if (pThreadPool->tasks == NULL)
    {
        G_free(pThreadPool->pThreads);
        G_free(pThreadPool->pThreadSeamphore);
        G_free(pThreadPool->pWaitTaskSemaphore);
        return false;
    }
    pThreadPool->leisureThread = (bool*)G_malloc(threadCount * sizeof(bool));
    if (pThreadPool->leisureThread == NULL)
    {
        G_free(pThreadPool->tasks);
        G_free(pThreadPool->pThreads);
        G_free(pThreadPool->pThreadSeamphore);
        G_free(pThreadPool->pWaitTaskSemaphore);
        return false;
    }
    pThreadPool->doneWatch = (int*)G_calloc(128, sizeof(int));
    if (pThreadPool->doneWatch == NULL)
    {
        G_free(pThreadPool->tasks);
        G_free(pThreadPool->pThreads);
        G_free(pThreadPool->pThreadSeamphore);
        G_free(pThreadPool->pWaitTaskSemaphore);
        G_free(pThreadPool->leisureThread);
        return false;
    }

    pThreadPool->ThreadPoolMutex = SDL_CreateMutex();
    pThreadPool->threadPoolSize = threadCount;
    pThreadPool->expandable = expandable;
    pThreadPool->running = true;

    initQueue(&pThreadPool->traceQueue, sizeof(Trace), 128, NULL, NULL, NULL, NULL);
    
    Thread_Func_Arg data = {0};
    data.pThreadPool = pThreadPool;

    for (Uint32 i = 0;i < threadCount;i++)
    {
        pThreadPool->tasks[i].canRun = false;
        data.index = i;

        pThreadPool->pThreadSeamphore[i] = SDL_CreateSemaphore(0);
        pThreadPool->pWaitTaskSemaphore[i] = SDL_CreateSemaphore(0);
        pThreadPool->pThreads[i] = SDL_CreateThread(threadFunc, "threadPool", &data);

        pThreadPool->leisureThread[i] = true;
        SDL_Delay(100);
    }
    pThreadPool->pThreads[threadCount] = SDL_CreateThread(processTrace, "traceThread", &data);
    SDL_Delay(100);

    return true;
}
static int getActiveThread(G_Thread_Pool * pThreadPool)
{
    SDL_LockMutex(pThreadPool->ThreadPoolMutex);

    for (int i = 0;i < pThreadPool->threadPoolSize;i++)
    {
        if (pThreadPool->leisureThread[i]) 
        {
            pThreadPool->leisureThread[i] = false;
            SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
            return i;
        }
    }

    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

    return -1;
}
static void splitTask(int maxThreadCount, int itemCount, int minRange, int * pThreadsNeed, Range * pTaskRange)
{
    int threadsNeedCount = 0;

    if (itemCount <= minRange)
    {
       threadsNeedCount = 1; 
       pTaskRange[0].startIndex = 0;
       pTaskRange[0].endIndex = itemCount;
    }
    else
    {
        if (itemCount <= maxThreadCount * minRange)
        {
            int itemCount2 = itemCount;
            int i = 0;

            while (itemCount2 - minRange > 0)
            {
                pTaskRange[i].startIndex = threadsNeedCount * minRange; 
                threadsNeedCount++;
                pTaskRange[i].endIndex = threadsNeedCount * minRange;
                i++;
                itemCount2 -= minRange;
            }
            if (itemCount2 > 0)
            {
                pTaskRange[i].startIndex = threadsNeedCount * minRange; 
                threadsNeedCount++;
                pTaskRange[i].endIndex = pTaskRange[i].startIndex + itemCount2;
            }
        }
        else
        {
            threadsNeedCount = maxThreadCount;

            int mod = itemCount % threadsNeedCount;
            int preThread = itemCount / threadsNeedCount;
            pTaskRange[0].startIndex = 0;
            pTaskRange[0].endIndex = preThread;
            // SDL_Log("TIME: %10llu--index: 0 start: %d, end: %d", SDL_GetTicksNS(), pTaskRange[0].startIndex, pTaskRange[0].endIndex);
            for (int i = 1;i < threadsNeedCount;i++)
            {
                pTaskRange[i].startIndex = pTaskRange[i - 1].endIndex;
                pTaskRange[i].endIndex = pTaskRange[i].startIndex + preThread;
                // SDL_Log("TIME: %10llu--index: %d start: %d, end: %d", SDL_GetTicksNS(), i, pTaskRange[i].startIndex, pTaskRange[i].endIndex);

                if (threadsNeedCount - i <= mod)
                {
                    pTaskRange[i].endIndex++;
                }
            }
        }
    }

    *pThreadsNeed = threadsNeedCount;
}
int * G_AddTask(G_Thread_Pool * pThreadPool, int itemCount, int minRange, G_Task * pTask)
{
    int threadsNeedCount = 0;

    if (pThreadPool == NULL || pThreadPool->threadPoolSize == 0 || pThreadPool->pThreads == NULL)
    {
        print("thread pool is not initialized");
        return NULL;
    }
    if (itemCount <= 0 || minRange <= 0 || pTask == NULL)
    {
        print("invalid item count of min range or task");
        return NULL;
    }

    Range * pRange = (Range*)G_malloc(pThreadPool->threadPoolSize * sizeof(Range));
    if (pRange == NULL)
    {
        print("failed to allocate memory for task range");
        return NULL;
    }

    splitTask(pThreadPool->threadPoolSize, itemCount, minRange, &threadsNeedCount, pRange);

    Trace tempTrace = {};
    tempTrace.threadUsedCount = threadsNeedCount;

    tempTrace.threadIndices = (int*)G_malloc(threadsNeedCount * sizeof(int));
    // print("trace index address: %p(alloc)", tempTrace.threadIndices);

    SDL_LockMutex(pThreadPool->ThreadPoolMutex);
    for (int i = 0;i < 128;i++)
    {
        if (pThreadPool->doneWatch[i] == TRACE_FREE)
        {
            tempTrace.taskAllDone = pThreadPool->doneWatch + i;
            *tempTrace.taskAllDone = TRACE_USED;
            break;
        }
    }
    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);


    G_Task tempTask;
    memcpy(&tempTask, pTask, sizeof(G_Task));

    for (int i = 0;i < threadsNeedCount;i++)
    {
        tempTrace.threadIndices[i] = getActiveThread(pThreadPool);
        while (tempTrace.threadIndices[i] == -1)
        {
            tempTrace.threadIndices[i] = getActiveThread(pThreadPool);
        }

        tempTask.canRun = true;
        tempTask.indexRange = pRange[i];
        tempTask.threadIndex = tempTrace.threadIndices[i];
        memcpy(pThreadPool->tasks + tempTrace.threadIndices[i], &tempTask, sizeof(G_Task));
        SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[tempTrace.threadIndices[i]]);
    }
    pThreadPool->traceQueue.addTail(&pThreadPool->traceQueue, &tempTrace);

    G_free(pRange);

    return tempTrace.taskAllDone;
}
void G_WaitTask(G_Thread_Pool * pThreadPool, int * pDone)
{
    if (pDone == NULL) return;
    while (*pDone != TRACE_DONE)
    {
        SDL_Delay(1);
    }

    SDL_LockMutex(pThreadPool->ThreadPoolMutex);
    *pDone = TRACE_FREE;
    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
}
void destroyThreadPool(G_Thread_Pool * pThreadPool)
{
    if (pThreadPool->threadPoolSize == 0 || pThreadPool->pThreads == NULL)
    {
        return;
    }

    SDL_LockMutex(pThreadPool->ThreadPoolMutex);
    pThreadPool->running = false;
    Uint32 size = pThreadPool->threadPoolSize;
    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

    for (int i = 0;i < size;i++)
    {
        SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[i]);
        SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[i]);
        SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[i]);
        SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[i]);
        SDL_WaitThread(pThreadPool->pThreads[i], NULL);
        SDL_DestroySemaphore(pThreadPool->pThreadSeamphore[i]);
        SDL_DestroySemaphore(pThreadPool->pWaitTaskSemaphore[i]);
    }

    SDL_WaitThread(pThreadPool->pThreads[size], NULL);

    SDL_LockMutex(pThreadPool->ThreadPoolMutex);

    G_free(pThreadPool->pWaitTaskSemaphore);
    G_free(pThreadPool->pThreads);
    G_free(pThreadPool->pThreadSeamphore);
    G_free(pThreadPool->leisureThread);
    G_free(pThreadPool->tasks);
    G_free(pThreadPool->doneWatch);

    G_deInitQueue(&pThreadPool->traceQueue);

    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

    SDL_DestroyMutex(pThreadPool->ThreadPoolMutex);
}