#include "G_threadPool.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_log.h"

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
            SDL_free(innerTrace.threadIndices);
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
    pThreadPool->pThreads = (SDL_Thread**)SDL_malloc((threadCount + 1) * sizeof(SDL_Thread*));
    if (pThreadPool->pThreads == NULL)
    {
        return false;
    }
    pThreadPool->pThreadSeamphore = (SDL_Semaphore**)SDL_malloc(threadCount * sizeof(SDL_Semaphore*));
    if (pThreadPool->pThreadSeamphore == NULL)
    {
        SDL_free(pThreadPool->pThreads);
        return false;
    }
    pThreadPool->pWaitTaskSemaphore = (SDL_Semaphore**)SDL_malloc(threadCount * sizeof(SDL_Semaphore*));
    if (pThreadPool->pWaitTaskSemaphore  == NULL)
    {
        SDL_free(pThreadPool->pThreads);
        SDL_free(pThreadPool->pThreadSeamphore);
        return false;
    }
    pThreadPool->tasks = (G_Task*)SDL_malloc(threadCount * sizeof(G_Task));
    if (pThreadPool->tasks == NULL)
    {
        SDL_free(pThreadPool->pThreads);
        SDL_free(pThreadPool->pThreadSeamphore);
        SDL_free(pThreadPool->pWaitTaskSemaphore);
        return false;
    }
    pThreadPool->leisureThread = (bool*)SDL_malloc(threadCount * sizeof(bool));
    if (pThreadPool->leisureThread == NULL)
    {
        SDL_free(pThreadPool->tasks);
        SDL_free(pThreadPool->pThreads);
        SDL_free(pThreadPool->pThreadSeamphore);
        SDL_free(pThreadPool->pWaitTaskSemaphore);
        return false;
    }
    pThreadPool->doneWatch = (int*)SDL_calloc(128, sizeof(int));
    if (pThreadPool->doneWatch == NULL)
    {
        SDL_free(pThreadPool->tasks);
        SDL_free(pThreadPool->pThreads);
        SDL_free(pThreadPool->pThreadSeamphore);
        SDL_free(pThreadPool->pWaitTaskSemaphore);
        SDL_free(pThreadPool->leisureThread);
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
int * G_AddTask(G_Thread_Pool * pThreadPool, int itemCount, int minRange, G_Task * pTask)
{
    int threadsNeedCount = 0;
    Range * startIndex = (Range*)SDL_malloc(pThreadPool->threadPoolSize * sizeof(Range));
    if (itemCount <= minRange)
    {
       threadsNeedCount = 1; 
       startIndex[0].startIndex = 0;
       startIndex[0].endIndex = itemCount;
    }
    else
    {
        if (itemCount <= pThreadPool->threadPoolSize * minRange)
        {
            int itemCount2 = itemCount;
            int i = 0;

            while (itemCount2 - minRange > 0)
            {
                startIndex[i].startIndex = threadsNeedCount * minRange; 
                threadsNeedCount++;
                startIndex[i].endIndex = threadsNeedCount * minRange;
                i++;
                itemCount2 -= minRange;
            }
            if (itemCount2 > 0)
            {
                startIndex[i].startIndex = threadsNeedCount * minRange; 
                threadsNeedCount++;
                startIndex[i].endIndex = startIndex[i].startIndex + itemCount2;
            }
        }
        else
        {
            threadsNeedCount = pThreadPool->threadPoolSize;

            int mod = itemCount % threadsNeedCount;
            int preThread = itemCount / threadsNeedCount;
            startIndex[0].startIndex = 0;
            startIndex[0].endIndex = preThread;
            // SDL_Log("TIME: %10llu--index: 0 start: %d, end: %d", SDL_GetTicksNS(), startIndex[0].startIndex, startIndex[0].endIndex);
            for (int i = 1;i < threadsNeedCount;i++)
            {
                startIndex[i].startIndex = startIndex[i - 1].endIndex;
                startIndex[i].endIndex = startIndex[i].startIndex + preThread;
                // SDL_Log("TIME: %10llu--index: %d start: %d, end: %d", SDL_GetTicksNS(), i, startIndex[i].startIndex, startIndex[i].endIndex);

                if (threadsNeedCount - i <= mod)
                {
                    startIndex[i].endIndex++;
                }
            }
        }
    }

    Trace tempTrace = {};
    tempTrace.threadUsedCount = threadsNeedCount;

    tempTrace.threadIndices = (int*)SDL_malloc(threadsNeedCount * sizeof(int));

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
        tempTask.indexRange = startIndex[i];
        tempTask.threadIndex = tempTrace.threadIndices[i];
        memcpy(pThreadPool->tasks + tempTrace.threadIndices[i], &tempTask, sizeof(G_Task));
        SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[tempTrace.threadIndices[i]]);
    }
    pThreadPool->traceQueue.addTail(&pThreadPool->traceQueue, &tempTrace);

    SDL_free(startIndex);

    return tempTrace.taskAllDone;
}
void G_WaitTask(G_Thread_Pool * pThreadPool, int * pDone)
{
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

    SDL_free(pThreadPool->pThreads);
    SDL_free(pThreadPool->pThreadSeamphore);
    SDL_free(pThreadPool->leisureThread);
    SDL_free(pThreadPool->tasks);
    SDL_free(pThreadPool->doneWatch);

    G_deInitQueue(&pThreadPool->traceQueue);

    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

    SDL_DestroyMutex(pThreadPool->ThreadPoolMutex);
}