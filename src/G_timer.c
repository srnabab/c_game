#include "G_timer.h"
#include "G_log.h"

#include "G_struct.h"

typedef struct _Timer
{
    int id;
    Uint64 nanoSecond;
    Uint64 accumulated;
    int repeat;
    bool done;
    int (*func)(void *);
    void * data;
} Timer;

static Timer timerS[128];

extern G_SYNC allSync;

bool initTimerSystem(void)
{
    memset(timerS, 0, sizeof(timerS));
    
    return true;
}

Uint64 f32_ms_to_ns(float ms)
{
    return (Uint64)(ms * MS_TO_NS);
}
Uint64 f32_s_to_ns(float s)
{
    return (Uint64)(s * S_TO_NS);
}
Uint64 u32_s_to_ns(Uint32 s)
{
    return (Uint64)(s * S_TO_NS);
}
Uint64 u32_min_to_ns(Uint32 min)
{
    return (Uint64)(min * MIN_TO_NS);
}
Uint64 u32_hour_to_ns(Uint32 hour)
{
    return (Uint64)(hour * HOUR_TO_NS);
}

static bool addTimer(Uint64 nano, int id, int repeat)
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
    if (id == 129)
    {
        logMessage("here");
    }
    for (int i = 0;i < 128;i++)
    {
        if (id == timerS[i].id)
        {
            return &timerS[i];
        }
    }
    return NULL;
}
bool intervalIsDone(Uint64 nano, int * id, int repeat)
{
    static int Ids = 1;
    if (*id)
    {
        Timer * tempTimer = findTimer(*id);
        if (tempTimer == NULL)
        {
            return false;
        }

        SDL_LockMutex(allSync.timerMutex);
        if (tempTimer->done)
        {
            if (tempTimer->func != NULL)
            {
                tempTimer->func(tempTimer->data);
            }
            tempTimer->done = false;

            SDL_UnlockMutex(allSync.timerMutex);
            return true;
        }
    }
    else
    {
        SDL_LockMutex(allSync.timerMutex);
        *id = Ids;
        Ids++;

        addTimer(nano, *id, repeat);
        SDL_UnlockMutex(allSync.timerMutex);
    }
    return false;
}
bool addTimerFunc(Uint64 nano, int * id, int repeat, int (*func)(void *), void * data)
{
    if (*id)
    {
        return intervalIsDone(nano, id, repeat);
    }
    else
    {
        intervalIsDone(nano, id, repeat);
        SDL_LockMutex(allSync.timerMutex);
        Timer * tempTimer = findTimer(*id);

        tempTimer->data = data;
        tempTimer->func = func;
        SDL_UnlockMutex(allSync.timerMutex);
    }

    return false;
}
bool deleteTimeSet(int *id)
{
    Timer * tempTimer = findTimer(*id);

    SDL_LockMutex(allSync.timerMutex);
    memset(tempTimer, 0, sizeof(Timer));
    SDL_UnlockMutex(allSync.timerMutex);

    *id = 0;

    return true;
}
static bool deleteTimeSetIn(int *id)
{
    Timer * tempTimer = findTimer(*id);

    memset(tempTimer, 0, sizeof(Timer));

    *id = 0;

    return true;
}
void accumlateTime(Uint64 nano)
{
    SDL_LockMutex(allSync.timerMutex);
    for (int i = 0;i < 128;i++)
    {
        if (timerS[i].id)
        {
            if (timerS[i].repeat)
            {
                timerS[i].accumulated += nano;
            }
            if (timerS[i].repeat == 0)
            {
                deleteTimeSetIn(&timerS[i].id);
            }

            if (timerS[i].accumulated >= timerS[i].nanoSecond)
            {
                timerS[i].accumulated -= timerS[i].nanoSecond;
                timerS[i].done = true;

                if (timerS[i].repeat > 0)
                {
                    timerS[i].repeat--;
                }
            }
        }
    }
    SDL_UnlockMutex(allSync.timerMutex);
}
bool deInitTimerSystem(void)
{
    return true;
}