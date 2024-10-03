#include "vk_all.h"
#include "judge.h"

#ifndef FLOW_H
#define FLOW_H

bool initWindow(void);
void initVulkan(void);
//process draw and imput
//void mainLoop(void);
//for aesthetic reasons
static inline void initializeAllInOne(void);
static inline void initializeRecreate(void);

#endif //flow.h