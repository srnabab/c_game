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
    size_t dataSize;

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
extern bool SDLCALL StackIsEmpty(EmptyStack stack);
extern bool SDLCALL StackIsFull(EmptyStack stack);
extern void SDLCALL getTop(EmptyStack * stack, void * data);
extern void SDLCALL deInitStack(EmptyStack * stack);

#include "SDL3/SDL_close_code.h"

#endif

#if defined(STACK_TEST)
#include "SDL3/SDL_test.h"

int stackTest(void)
{
    int res, passed, total, failed;
    passed = total = failed = 0;
    SDL_Log("-------------------------------------------------------------");
    SDL_Log("stack test\n");

    total++;
    EmptyStack stack = {};
    initStack(&stack, sizeof(Uint32), NULL, NULL);

    SDLTest_AssertCheck(StackIsEmpty(stack) == true, "StackIsEmpty Uint32 test");
    SDLTest_AssertCheck(StackIsFull(stack) == false, "StackIsFull Uint32 test");

    Uint32 a = 1;
    for (Uint32 i = 0;i < 128;i++)
    {
        stack.pushFn(&stack, &a);
        a++;
    }

    SDLTest_AssertCheck(StackIsFull(stack) == true, "StackIsFull Uint32 test");
    SDLTest_AssertCheck(StackIsEmpty(stack) == false, "StackIsEmpty Uint32 test");


    for (Uint32 i = 128;i > 0;i--)
    {
        Uint32 b;
        stack.popFn(&stack, &b);

        SDLTest_AssertCheck(b == i, "defaultPopFn Uint32 test");
    }
    deInitStack(&stack);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("stack Uint32 test passed");
        passed++;
    }
    else 
    {
        SDL_Log("stack Uint32 test failed");
        if (!failed) failed = passed + 1;
    }

    total++;
    initStack(&stack, sizeof(float), NULL, NULL);

    SDLTest_AssertCheck(StackIsEmpty(stack) == true, "StackIsEmpty float test");
    SDLTest_AssertCheck(StackIsFull(stack) == false, "StackIsFull float test");

    for (Uint32 i = 0;i < 128;i++)
    {
        float c = (float)i;
        stack.pushFn(&stack, &c);
    }

    SDLTest_AssertCheck(StackIsFull(stack) == true, "StackIsFull float test");
    SDLTest_AssertCheck(StackIsEmpty(stack) == false, "StackIsEmpty float test");

    for (Uint32 i = 128;i > 0;i--)
    {
        float c;
        stack.popFn(&stack, &c);

        SDLTest_AssertCheck(c - (float)i <= 0.0000001f, "defaultPopFn float test");
    }
    deInitStack(&stack);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        passed++;
        SDL_Log("stack float test passed");
    }
    else 
    {
        SDL_Log("stack float test failed");
        if (!failed) failed = passed + 1;
    }

    total++;
    initStack(&stack, sizeof(int), NULL, NULL);

    SDLTest_AssertCheck(StackIsEmpty(stack) == true, "StackIsFull int test");
    SDLTest_AssertCheck(StackIsFull(stack) == false, "StackIsEmpty int test");

    for (int i = 0;i < 128;i++)
    {
        stack.pushFn(&stack, &i);
    }

    SDLTest_AssertCheck(StackIsFull(stack) == true, "StackIsEmpty int test");
    SDLTest_AssertCheck(StackIsEmpty(stack) == false, "StackIsFull int test");

    for (int i = 127;i >= 0;i--)
    {
        int c;
        stack.popFn(&stack, &c);

        SDLTest_AssertCheck(c == i, "defaultPopFn int test");
    }
    deInitStack(&stack);

    res = SDLTest_AssertSummaryToTestResult();
    if (res == TEST_RESULT_PASSED) 
    {
        SDL_Log("stack int test passed");
        passed++;
    }
    else 
    {
        SDL_Log("stack int test failed");
        if (!failed) failed = passed + 1;
    }

    SDLTest_LogAssertSummary();
    if (res == TEST_RESULT_PASSED) SDL_Log("\nstack test passed(%d / %d)", passed, total);
    else SDL_Log("\nstack test failed(%d / %d), first failed: %d", passed, total, failed);

    SDL_Log("-------------------------------------------------------------\n");

    return res;
}
#endif