#include "G_stack.h"

static bool defaultPushFn(EmptyStack * pStack, void * data)
{
    if (isFull(*pStack)) return false;

    SDL_LockMutex(pStack->mutex);
    pStack->top++;
    memcpy((char*)pStack->data + (pStack->dataSize * pStack->top), data, pStack->dataSize); 
    SDL_UnlockMutex(pStack->mutex);

    return true;
}
static void defaultPopFn(EmptyStack * pStack, void * data)
{
    if (isEmpty(*pStack)) return;

    SDL_LockMutex(pStack->mutex);
    pStack->top--;
    memcpy(data, (char*)pStack->data + (pStack->dataSize * (pStack->top + 1)), pStack->dataSize);
    SDL_UnlockMutex(pStack->mutex);
}
bool initStack(EmptyStack * stack, size_t dataSize, Push pushFn, Pop popFn)
{
    stack->top = -1;

    stack->data = SDL_malloc(MAX_STACKS * dataSize);
    if (stack->data == NULL)
    {
        return false;
    }

    stack->mutex = SDL_CreateMutex();
    stack->dataSize = dataSize;

    if (pushFn == NULL) stack->pushFn = defaultPushFn;
    else stack->pushFn = pushFn;
    if (popFn == NULL) stack->popFn = defaultPopFn;
    else stack->popFn = popFn;

    return true;
}
bool isEmpty(EmptyStack stack)
{
    if (stack.top == -1)
        return true;

    return false;
}
bool isFull(EmptyStack stack)
{
    if (stack.top == MAX_STACKS - 1)
        return true;
    
    return false;
}
void getTop(EmptyStack * stack, void * data)
{
    SDL_LockMutex(stack->mutex);
    memcpy(data, (char*)stack->data + (stack->dataSize * stack->top), stack->dataSize);
    SDL_UnlockMutex(stack->mutex);
}
void deInitStack(EmptyStack * stack)
{
    SDL_free(stack->data);
    stack->top = -1;
    stack->dataSize = 0;
    stack->popFn = NULL;
    stack->pushFn = NULL;
    SDL_DestroyMutex(stack->mutex);
}