#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_atomic.h"

#ifndef G_ENETITY_H
#define G_ENETITY_H 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

struct _G_Point_Uint
{
    Uint32 x;
    Uint32 y;
};
struct _G_Point_Float
{
    float x;
    float y;
};

union _G_Point
{
    struct _G_Point_Uint pointUint;
    struct _G_Point_Float pointFloat;
};
typedef union _G_Point G_Point;

struct _G_Entity
{
    struct _G_Point_Float position;
    float speed;
    bool direction[4];
};
typedef struct _G_Entity G_Entity;

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL EntityMove(G_Entity * entity, float deltaTime);
extern void SDLCALL initEntity(G_Entity * entity, float x, float y, float speed);

#include "SDL3/SDL_close_code.h"

#endif // G_enetity.h