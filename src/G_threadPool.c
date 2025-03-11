#include "G_threadPool.h"
#include "SDL3/SDL_timer.h"

static int threadFunc(void * data)
{
    Uint32 index = ((Thread_Func_Arg*)data)->index;
    G_Thread_Pool * pThreadPool = ((Thread_Func_Arg*)data)->pThreadPool;
    bool running = pThreadPool->running;
    SDL_Semaphore * semaphore = pThreadPool->pThreadSeamphore[index];
    SDL_Mutex * mutex = pThreadPool->ThreadPoolMutex;
    G_Task task;
    bool res;

    while (running)
    {
        SDL_WaitSemaphore(semaphore);

        SDL_LockMutex(mutex);

        // get task
        res = pThreadPool->taskQueue.getHead(&pThreadPool->taskQueue, &task);

        SDL_UnlockMutex(mutex);

        // run task
        if (res) task.executeFunc(&task);

        SDL_LockMutex(pThreadPool->ThreadPoolMutex);

        pThreadPool->leisureThread[index] = true;
        running = pThreadPool->running;

        SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
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
    pThreadPool->leisureThread = (bool*)SDL_malloc(threadCount * sizeof(bool));
    if (pThreadPool->leisureThread == NULL)
    {
        SDL_free(pThreadPool->pThreads);
        SDL_free(pThreadPool->pThreadSeamphore);
        return false;
    }
    pThreadPool->indices = (int*)SDL_malloc(threadCount * sizeof(int));
    if (pThreadPool->leisureThread == NULL)
    {
        SDL_free(pThreadPool->pThreads);
        SDL_free(pThreadPool->pThreadSeamphore);
        SDL_free(pThreadPool->leisureThread);
        return false;
    }

    bool res = initQueue(&pThreadPool->taskQueue, sizeof(G_Task), threadCount, NULL, NULL, NULL, NULL);
    if (res == false)
    {
        SDL_free(pThreadPool->pThreads);
        SDL_free(pThreadPool->pThreadSeamphore);
        SDL_free(pThreadPool->leisureThread);
        SDL_free(pThreadPool->indices);
        return false;
    }
    
    pThreadPool->ThreadPoolMutex = SDL_CreateMutex();
    pThreadPool->threadPoolSize = threadCount;
    pThreadPool->expandable = expandable;
    pThreadPool->running = true;
    for (Uint32 i = 0;i < threadCount;i++)
    {
        pThreadPool->indices[i] = i;
        Thread_Func_Arg data = {0};
        data.index = i;
        data.pThreadPool = pThreadPool;

        pThreadPool->pThreadSeamphore[i] = SDL_CreateSemaphore(0);
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
            SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
            return i;
        }
    }

    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

    return -1;
}
int * G_AddTask(G_Thread_Pool * pThreadPool, G_Task * pTask)
{
    SDL_LockMutex(pThreadPool->ThreadPoolMutex);

    bool res = pThreadPool->taskQueue.addTail(&pThreadPool->taskQueue, pTask);
    if (!res)
    {
        SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
        return NULL;
    }
    int index = getActiveThread(pThreadPool);
    if (index == -1) 
    {
        pThreadPool->taskQueue.getTail(&pThreadPool->taskQueue, pTask);
        SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
        return NULL;
    }

    pThreadPool->leisureThread[index] = false;

    SDL_SignalSemaphore(pThreadPool->pThreadSeamphore[index]);

    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
    return pThreadPool->indices + index;
}
void G_WaitTask(G_Thread_Pool * pThreadPool, int taskIndex)
{
    while (1)
    {
        SDL_LockMutex(pThreadPool->ThreadPoolMutex);
        if (pThreadPool->leisureThread[taskIndex]) 
        {
            SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
            break;
        }
        SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);
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

    SDL_UnlockMutex(pThreadPool->ThreadPoolMutex);

    G_deInitQueue(&pThreadPool->taskQueue);

    SDL_DestroyMutex(pThreadPool->ThreadPoolMutex);
}