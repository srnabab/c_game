#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_mutex.h"

#ifndef STACK_H
#define STACK_H 1

#include "SDL3/SDL_begin_code.h"

#define MAX_STACKS 128

struct _EmptyStack;

typedef bool (*Push)(struct _EmptyStack * stack, void * data);
typedef void (*Pop)(struct _EmptyStack * stack, void * data);

typedef struct _EmptyStack
{
    void * data;
    int top;
    Uint32 dataSize;

    SDL_Mutex * mutex;
    Push pushFn;
    Pop popFn;
} EmptyStack;

/**
 * \param data for custome data type, you should manage memory yourself , NULL form empty
 * \param pushFn custome push func , NULL form empty
 * \param popFn custome pop func , NULL form empty
*/
extern bool SDLCALL initStack(EmptyStack * stack, size_t dataSize, Push pushFn, Pop popFn);
extern bool SDLCALL isEmpty(EmptyStack stack);
extern bool SDLCALL isFull(EmptyStack stack);
extern void SDLCALL getTop(EmptyStack * stack, void * data);
extern void SDLCALL deInitStack(EmptyStack * stack);

#include "SDL3/SDL_close_code.h"

#endif

#ifdef STACK_TEST
#include "SDL3/SDL_test.h"

void stackTest(void)
{
    SDL_Log("--------------------------\n");

    EmptyStack stack = {};
    initStack(&stack, sizeof(Uint32), NULL, NULL);

    SDL_assert(isEmpty(stack) == true);
    SDL_assert(isFull(stack) == false);

    Uint32 a = 1;
    for (Uint32 i = 0;i < 128;i++)
    {
        stack.pushFn(&stack, &a);
        a++;
    }

    SDL_assert(isFull(stack) == true);
    SDL_assert(isEmpty(stack) == false);

    for (Uint32 i = 128;i > 0;i--)
    {
        Uint32 b;
        stack.popFn(&stack, &b);

        SDL_assert(b == i);
    }
    deInitStack(&stack);

    SDL_Log("Uint32 test passed (1 / 3)\n");

    initStack(&stack, sizeof(float), NULL, NULL);

    SDL_assert(isEmpty(stack) == true);
    SDL_assert(isFull(stack) == false);

    for (Uint32 i = 0;i < 128;i++)
    {
        float c = (float)i;
        stack.pushFn(&stack, &c);
    }

    SDL_assert(isFull(stack) == true);
    SDL_assert(isEmpty(stack) == false);

    for (Uint32 i = 128;i > 0;i--)
    {
        float c;
        stack.popFn(&stack, &c);

        SDL_assert(c - (float)i <= 0.0000001f);
    }
    deInitStack(&stack);

    SDL_Log("float test passed (2 / 3)\n");

    initStack(&stack, sizeof(int), NULL, NULL);

    SDL_assert(isEmpty(stack) == true);
    SDL_assert(isFull(stack) == false);

    for (int i = 0;i < 128;i++)
    {
        stack.pushFn(&stack, &i);
    }

    SDL_assert(isFull(stack) == true);
    SDL_assert(isEmpty(stack) == false);

    for (int i = 127;i >= 0;i--)
    {
        int c;
        stack.popFn(&stack, &c);

        SDL_assert(c == i);
    }
    deInitStack(&stack);

    SDL_Log("int test passed (3 / 3)\n");

    SDL_Log("stack test passed\n");
    SDL_Log("--------------------------");
}
#endif