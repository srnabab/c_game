#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_mutex.h"

#ifndef STACK_H
#define STACK_H 1

#include "SDL3/SDL_begin_code.h"

#define MAX_STACKS 128

struct _G_Stack;

typedef bool (*Push)(struct _G_Stack * stack, void * data);
typedef bool (*Pop)(struct _G_Stack * stack, void * data);

typedef struct _G_Stack
{
    void * data;
    int top;
    size_t dataSize;

    SDL_Mutex * mutex;
    Push pushFn;
    Pop popFn;
} G_Stack;

/**
 * \param data for custome data type, you should manage memory yourself , NULL form empty
 * \param pushFn custome push func , NULL form empty
 * \param popFn custome pop func , NULL form empty
*/
extern bool SDLCALL initStack(G_Stack * stack, size_t dataSize, Push pushFn, Pop popFn);
extern bool SDLCALL StackIsEmpty(G_Stack stack);
extern bool SDLCALL StackIsFull(G_Stack stack);
extern void SDLCALL getTop(G_Stack * stack, void * data);
extern void SDLCALL deInitStack(G_Stack * stack);

#include "SDL3/SDL_close_code.h"

#endif