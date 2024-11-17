#include "std_c.h"

#ifndef TIMER_H
#define TIMER_H

bool initTimerSystem(void);
bool intervalIsDone(float second);
bool deleteTimeSet(float time);
void accumlateTime(float deltaTime);

#endif