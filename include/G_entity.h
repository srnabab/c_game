#include "SDL3/SDL_stdinc.h"
// #include "SDL3/SDL_atomic.h"

#include "cglm/types.h"

#ifndef G_ENETITY_H
#define G_ENETITY_H 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

struct _G_Point_Int
{
    int32_t x;
    int32_t y;
};
typedef struct _G_Point_Int G_Point_Int;

struct _G_Point_Float
{
    float x;
    float y;
};
typedef struct _G_Point_Float G_Point_Float;

union _G_Point
{
    struct _G_Point_Int pointUint;
    struct _G_Point_Float pointFloat;
};
typedef union _G_Point G_Point;

struct _G_Entity
{
    vec2 position;
    float speed;
    bool direction[4];
};
typedef struct _G_Entity G_Entity;

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL EntityMove(G_Entity * entity, float deltaTime);
extern void SDLCALL initEntity(G_Entity * entity, float x, float y, float speed);
extern void SDLCALL setEntityPosition(G_Entity * entity, float x, float y);

#include "SDL3/SDL_close_code.h"

#endif // G_enetity.h