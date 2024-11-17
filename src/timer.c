#include "std_c.h"
#include "timer.h"

#define MAX_TIMER_HASH_MAP 128

static uint32_t timerCount = 0;

typedef struct _TimerHash
{
    float timeSet_Key;
    float timeAccumulated;
    bool timeDone;
    bool hasValue;
} TimerHash;

static TimerHash timerHash[MAX_TIMER_HASH_MAP];

bool initTimerSystem(void)
{
    for (int i = 0;i < MAX_TIMER_HASH_MAP;i++)
    {
        timerHash[i].hasValue = false;
        timerHash[i].timeDone = false;
        timerHash[i].timeAccumulated = 0.0f;
    }

    return true;
}

static uint32_t hashFn1(uint32_t key)
{
    return key % MAX_TIMER_HASH_MAP;
}

static uint32_t hashFn2(uint32_t key)
{
    return (key + 7) % MAX_TIMER_HASH_MAP;
}

static uint32_t hashFn3(uint32_t key)
{
    return (key + 13) % MAX_TIMER_HASH_MAP;
}

static uint32_t hashFn4(uint32_t key)
{
    return (key * 3 + 17) % MAX_TIMER_HASH_MAP;
}

static bool insertTime(float time)
{
    static int count = 0;
    if (count > 7)
        return false;

    uint32_t temp1;
    memcpy(&temp1, &time, sizeof(float));

    uint32_t pos1 = hashFn1(temp1);

    if (!timerHash[pos1].hasValue)
    {
        timerHash[pos1].hasValue = true;
        timerHash[pos1].timeSet_Key = time;
        count = 0;
        return true;
    }

    float old_key1 = timerHash[pos1].timeSet_Key;
    timerHash[pos1].timeSet_Key = time;
    uint32_t temp2;
    memcpy(&temp2, &old_key1, sizeof(float));

    uint32_t pos2 = hashFn2(temp2);

    if (!timerHash[pos2].hasValue)
    {
        timerHash[pos2].hasValue = true;
        timerHash[pos2].timeSet_Key = old_key1;
        count = 0;
        return true;
    }

    float old_key2 = timerHash[pos2].timeSet_Key;
    timerHash[pos2].timeSet_Key = old_key1;
    uint32_t temp3;
    memcpy(&temp3, &old_key2, sizeof(float));

    uint32_t pos3 = hashFn3(temp3);

    if (!timerHash[pos3].hasValue)
    {
        timerHash[pos3].hasValue = true;
        timerHash[pos3].timeSet_Key = old_key2;
        count = 0;
        return true;
    }

    float old_key3 = timerHash[pos3].timeSet_Key;
    timerHash[pos3].timeSet_Key = old_key2;
    uint32_t temp4;
    memcpy(&temp4, &old_key3, sizeof(float));

    uint32_t pos4 = hashFn4(temp4);

    if (!timerHash[pos4].hasValue)
    {
        timerHash[pos4].hasValue = true;
        timerHash[pos4].timeSet_Key = old_key3;
        count = 0;
        return true;
    }

    count++;
    return insertTime(time);
}

static bool searchInterval(float second, int * index)
{
    uint32_t temp1;
    memcpy(&temp1, &second, sizeof(float));

    uint32_t pos1 = hashFn1(temp1);
    uint32_t pos2 = hashFn2(temp1);
    uint32_t pos3 = hashFn3(temp1);
    uint32_t pos4 = hashFn4(temp1);

    if (timerHash[pos1].hasValue && timerHash[pos1].timeSet_Key == second)
    {
        *index = pos1;
        return true;
    }
    else if (timerHash[pos2].hasValue && timerHash[pos2].timeSet_Key == second)
    {
        *index = pos2;
        return true;
    }
    else if (timerHash[pos3].hasValue && timerHash[pos3].timeSet_Key == second)
    {
        *index = pos3;
        return true;
    }
    else if (timerHash[pos4].hasValue && timerHash[pos4].timeSet_Key == second)
    {
        *index = pos4;
        return true;
    }

    return false;
}
bool intervalIsDone(float second)
{
    if (timerCount > 128)
        return false;
    int index;
    //printf("second check: %f\n", second);
    if (searchInterval(second, &index))
    {
        if (timerHash[index].timeDone)
        {
            timerHash[index].timeDone = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        insertTime(second);
        timerCount++;
        return false;
    }
}
bool deleteTimeSet(float time)
{
    int index;
    if (searchInterval(time, &index))
    {
        timerHash[index].hasValue = false;
        timerHash[index].timeDone = false;
        timerHash[index].timeAccumulated = 0.0f;
    }
    return true;
}
void accumlateTime(float deltaTime)
{
    for (int i = 0;i < MAX_TIMER_HASH_MAP;i++)
    {
        if (timerHash[i].hasValue)
        {
            timerHash[i].timeAccumulated += deltaTime;
            //printf("time: %f\n", timerHash[i].timeAccumulated);
            if (timerHash[i].timeAccumulated >= timerHash[i].timeSet_Key)
            {
                timerHash[i].timeAccumulated -= timerHash[i].timeSet_Key;
                timerHash[i].timeDone = true;
            }
        }
    }
}