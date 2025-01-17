#include "SDL3/SDL_stdinc.h"

#ifndef STACK_H
#define STACK_H 1

#include "SDL3/SDL_begin_code.h"

#define MAX_STACKS 128

typedef enum _DataType
{
    self,
    i8, u8, i16, u16, i32, u32, i64, u64,
    f32, d64, b1
} DataType;

struct _EmptyStack;

typedef bool (*Push)(struct _EmptyStack * stack, void * data);
typedef void* (*Pop)(struct _EmptyStack * stack);

typedef struct _EmptyStack
{
    void * data;
    int top;
    DataType type;
    Push pushFn;
    Pop popFn;
} EmptyStack;

/**
 * \param data for custome data type, you should manage memory yourself , NULL form empty
 * \param pushFn custome push func , NULL form empty
 * \param popFn custome pop func , NULL form empty
*/
extern bool SDLCALL initStack(EmptyStack * stack, DataType type, void * data, Push pushFn, Pop popFn);
extern bool SDLCALL isEmpty(EmptyStack stack);
extern bool SDLCALL isFull(EmptyStack stack);
extern bool SDLCALL deInitStack(EmptyStack * stack);

#include "SDL3/SDL_close_code.h"

#endif