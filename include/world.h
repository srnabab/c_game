#include "core.h"

#ifndef WORLD_H
#define WORLD_H

void initWorld(void);
void createCircle(float x, float y);
void updateCircle(VkExtent2D * pExtent2D, Vertex ** ppVertices);
uint32_t getBoxCount(void);
void cleanWorld(void);
void destroyFloor(void);
int stepWorld(void * arg);

#endif