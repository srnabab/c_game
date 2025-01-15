#include "SDL3/SDL_stdinc.h"

#ifndef TIMER_H
#define TIMER_H 1

bool initTimerSystem(void);
bool intervalIsDone(uint64_t nano, int * id, int repeat);
bool deleteTimeSet(int *id);
void accumlateTime(uint64_t nano);
bool deInitTimerSystem(void);

#define MS_TO_NS 1000000ULL
uint64_t f32_ms_to_ns(float ms);
#define S_TO_NS 1000000000ULL
uint64_t u32_s_to_ns(uint32_t s);
#define MIN_TO_NS 60000000000ULL
uint64_t u32_min_to_ns(uint32_t min);
#define HOUR_TO_NS 3600000000000ULL
uint64_t u32_hour_to_ns(uint32_t hour);
bool addTimerFunc(uint64_t nano, int * id, int repeat, int (*func)(void *), void * data);

#endif