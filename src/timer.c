#include "std_c.h"
#include "timer.h"
#include "SDL3/SDL.h"

typedef struct _Timer
{
    int id;
    uint64_t nanoSecond;
    uint64_t accumulated;
    int repeat;
    bool done;
    int (*func)(void *);
    void * data;
} Timer;

// typedef struct _TimerList
// {
//     Timer * node;
//     Timer * next;
// } TimerList;

// static TimerList * root = NULL;
static Timer timerS[128];
static SDL_Mutex * timerMutex = NULL;

bool initTimerSystem(void)
{
    memset(timerS, 0, sizeof(timerS));

    timerMutex = SDL_CreateMutex();

    return true;
}

uint64_t f32_ms_to_ns(float ms)
{
    return (uint64_t)(ms * MS_TO_NS);
}
uint64_t u32_s_to_ns(uint32_t s)
{
    return (uint64_t)(s * S_TO_NS);
}
uint64_t u32_min_to_ns(uint32_t min)
{
    return (uint64_t)(min * MIN_TO_NS);
}
uint64_t u32_hour_to_ns(uint32_t hour)
{
    return (uint64_t)(hour * HOUR_TO_NS);
}

static bool addTimer(uint64_t nano, int id, int repeat)
{
    for (int i = 0;i < 128;i++)
    {
        if (timerS[i].id == 0)
        {
            timerS[i].id = id;
            timerS[i].repeat = repeat;
            timerS[i].nanoSecond = nano;
            return true;
        }
    }

    return false;
}
static Timer * findTimer(int id)
{
    for (int i = 0;i < 128;i++)
    {
        if (id == timerS[i].id)
        {
            return &timerS[i];
        }
    }
    return NULL;
}
bool intervalIsDone(uint64_t nano, int * id, int repeat)
{
    if (*id == 0)
    {
        static int Ids = 1;
        SDL_LockMutex(timerMutex);
        *id = Ids;
        Ids++;

        addTimer(nano, *id, repeat);
        SDL_UnlockMutex(timerMutex);
    }
    else
    {
        Timer * tempTimer = findTimer(*id);

        SDL_LockMutex(timerMutex);
        if (tempTimer->done)
        {
            if (tempTimer->func != NULL)
            {
                tempTimer->func(tempTimer->data);
            }
            tempTimer->done = false;

            SDL_UnlockMutex(timerMutex);
            return true;
        }
    }
    return false;
}
bool addTimerFunc(uint64_t nano, int * id, int repeat, int (*func)(void *), void * data)
{
    intervalIsDone(nano, id, repeat);

    SDL_LockMutex(timerMutex);
    Timer * tempTimer = findTimer(*id);

    tempTimer->data = data;
    tempTimer->func = func;
    SDL_UnlockMutex(timerMutex);

    return true;
}
bool deleteTimeSet(int *id)
{
    Timer * tempTimer = findTimer(*id);

    SDL_LockMutex(timerMutex);
    memset(tempTimer, 0, sizeof(Timer));
    SDL_UnlockMutex(timerMutex);

    *id = 0;

    return true;
}
void accumlateTime(uint64_t nano)
{
    SDL_LockMutex(timerMutex);
    for (int i = 0;i < 128;i++)
    {
        if (timerS[i].id)
        {
            if (timerS[i].repeat > 0)
            {
                timerS[i].accumulated += nano;
                timerS[i].repeat--;
            }
            else if (timerS[i].repeat < 0)
            {
                timerS[i].accumulated += nano;
            }

            if (timerS[i].accumulated >= timerS[i].nanoSecond)
            {
                timerS[i].accumulated -= timerS[i].nanoSecond;
                timerS[i].done = true;
            }
        }
    }
    SDL_UnlockMutex(timerMutex);
}
bool deInitTimerSystem(void)
{
    SDL_DestroyMutex(timerMutex);
    return true;
}