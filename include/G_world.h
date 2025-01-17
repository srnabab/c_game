#include "vk_struct.h"

#ifndef WORLD_H
#define WORLD_H 1

#include "SDL3/SDL_begin_code.h"

#define SCALE_FACTOR 0.01f
#define SCALE_FACTOR_INV (1/SCALE_FACTOR)

void initWorld(void);
void createCircle(float x, float y);
void updateCircle(VkExtent2D * pExtent2D, Vertex ** ppVertices);
uint32_t getBoxCount(void);
void cleanWorld(void);
void destroyFloor(void);
int stepWorld(void * arg);

#include "SDL3/SDL_close_code.h"

#endif