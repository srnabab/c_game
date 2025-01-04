#include "core.h"

#ifndef WORLD_H
#define WORLD_H

#define SCALE_FACTOR 0.01f
#define SCALE_FACTOR_INV (1/SCALE_FACTOR)

// void * box2d_SDL_Alloc(unsigned int size, int alignment);
// void box2d_SDL_Free(void * memory);
void initWorld(void);
void createCircle(float x, float y);
void updateCircle(VkExtent2D * pExtent2D, Vertex ** ppVertices);
uint32_t getBoxCount(void);
void cleanWorld(void);
void destroyFloor(void);
int stepWorld(void * arg);

#endif