#include "vk_struct.h"

#ifndef WORLD_H
#define WORLD_H 1

#include "SDL3/SDL_begin_code.h"

#define SCALE_FACTOR 0.01f
#define SCALE_FACTOR_INV (1/SCALE_FACTOR)

#define TIME_STEP (1.0f / 120.0f)
#define SUB_STEP_COUNT 4

extern void SDLCALL initWorld(void);
extern void SDLCALL createCircle(float x, float y);
extern void SDLCALL updateCircle(VkExtent2D * pExtent2D, Vertex ** ppVertices);
extern uint32_t SDLCALL getBoxCount(void);
extern void SDLCALL cleanWorld(void);
extern void SDLCALL destroyFloor(void);
extern int SDLCALL stepWorld(void * arg);

#include "SDL3/SDL_close_code.h"

#endif