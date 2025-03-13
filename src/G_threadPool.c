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
    int i;

    while (running)
    {
        SDL_WaitSemaphore(semaphore);

        // get task
        memcpy(&task, pThreadPool->tasks + index, sizeof(G_Task));

        // run task
        if (task.canRun) 
        {
            for (i = task.indexRange.startIndex;i < task.indexRange.endIndex;i++)
            task.executeFunc(&task);
        }

        SDL_LockMutex(pThreadPool->ThreadPoolMutex);

        pThreadPool->tasks[index].canRun = false;
        pThreadPool->leisureThread[index] = true;
        running = pThreadPool->running;

        SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

        SDL_SignalSemaphore(waitSemaphore);
    }

    return 0;
}
bool createThreadPool(G_Thread_Pool * pThreadPool, Uint32 threadCount, bool expandable)
{
    pThreadPool->pThreads = (SDL_Thread**)SDL_malloc(threadCount * sizeof(SDL_Thread*));
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
    
    pThreadPool->ThreadPoolMutex = SDL_CreateMutex();
    pThreadPool->threadPoolSize = threadCount;
    pThreadPool->expandable = expandable;
    pThreadPool->running = true;
    for (Uint32 i = 0;i < threadCount;i++)
    {
        pThreadPool->tasks[i].canRun = false;
        Thread_Func_Arg data = {0};
        data.index = i;
        data.pThreadPool = pThreadPool;

        pThreadPool->pThreadSeamphore[i] = SDL_CreateSemaphore(0);
        pThreadPool->pWaitTaskSemaphore[i] = SDL_CreateSemaphore(0);
        pThreadPool->pThreads[i] = SDL_CreateThread(threadFunc, "threadPool", &data);

        pThreadPool->leisureThread[i] = true;

        SDL_Delay(100);
    }

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
            if (mod < threadsNeedCount / 2)
            {
                startIndex[0].startIndex = 0;
                startIndex[0].endIndex = preThread;
                for (int i = 1;i < threadsNeedCount;i++)
                {
                    startIndex[i].startIndex = startIndex[i - 1].endIndex;
                    startIndex[i].endIndex = startIndex[i].startIndex + preThread;

                    if (threadsNeedCount - i <= mod)
                    {
                        startIndex[i].endIndex++;
                    }
                }
            }
        }
    }

    int * indices = (int*)SDL_malloc((threadsNeedCount + 1) * sizeof(int));
    indices[0] = threadsNeedCount;
    indices = indices + 1;
    G_Task tempTask;
    memcpy(&tempTask, pTask, sizeof(G_Task));

    for (int i = 0;i < threadsNeedCount;i++)
    {
        indices[i] = getActiveThread(pThreadPool);
        if (indices[i] == -1)
        {
            indices[i] = getActiveThread(pThreadPool);
            while (indices[i] == -1)
            {
                indices[i] = getActiveThread(pThreadPool);
            }
        }
        SDL_Log("thread index: %d", indices[i]);

        tempTask.canRun = true;
        tempTask.indexRange = startIndex[i];
        memcpy(pThreadPool->tasks + indices[i], &tempTask, sizeof(G_Task));
        SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[indices[i]]);
    }

    return indices - 1;
}
void G_WaitTask(G_Thread_Pool * pThreadPool, int * taskIndex)
{
    for(int i = 1;i < taskIndex[0] + 1;i++)
    { 
        SDL_WaitSemaphore(pThreadPool->pWaitTaskSemaphore[taskIndex[i]]);
    }
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
    }

    SDL_LockMutex(pThreadPool->ThreadPoolMutex);

    SDL_free(pThreadPool->pThreads);
    SDL_free(pThreadPool->pThreadSeamphore);
    SDL_free(pThreadPool->leisureThread);
    SDL_free(pThreadPool->tasks);

    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

    SDL_DestroyMutex(pThreadPool->ThreadPoolMutex);
}