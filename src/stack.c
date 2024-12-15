#include "stack.h"
#include "SDL3/SDL_stdinc.h"
#include <stdlib.h>
#include <stdint.h>

static bool push_i8(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((int8_t *)stack->data)[stack->top] = *(int8_t *)data;

    return true;
}
static void* pop_i8(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((int8_t *)stack->data)[stack->top + 1];
}
static bool push_i16(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((int16_t *)stack->data)[stack->top] = *(int16_t *)data;

    return true;
}
static void* pop_i16(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((int16_t *)stack->data)[stack->top + 1];
}
static bool push_i32(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((int32_t *)stack->data)[stack->top] = *(int32_t *)data;

    return true;
}
static void* pop_i32(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((int32_t *)stack->data)[stack->top + 1];
}
static bool push_i64(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((int64_t *)stack->data)[stack->top] = *(int8_t *)data;

    return true;
}
static void* pop_i64(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((int64_t *)stack->data)[stack->top + 1];
}
static bool push_u8(EmptyStack * stack, void * data)
{
    if (isFull(*stack))
        return false;
    
    stack->top++;

    ((uint8_t *)stack->data)[stack->top] = *(uint8_t *)data;

    return true;
}
static void* pop_u8(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((uint8_t *)stack->data)[stack->top + 1];
}
static bool push_u16(EmptyStack * stack, void * data)
{
    if (isFull(*stack))
        return false;
    
    stack->top++;

    ((uint16_t *)stack->data)[stack->top] = *(uint16_t *)data;

    return true;
}
static void* pop_u16(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((uint16_t *)stack->data)[stack->top + 1];
}
static bool push_u32(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((uint32_t *)stack->data)[stack->top] = *(uint32_t *)data;

    return true;
}
static void* pop_u32(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((uint32_t *)stack->data)[stack->top + 1];
}
static bool push_u64(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((uint64_t *)stack->data)[stack->top] = *(uint64_t *)data;

    return true;
}
static void* pop_u64(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((uint64_t *)stack->data)[stack->top + 1];
}
static bool push_f32(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((float *)stack->data)[stack->top] = *(float *)data;

    return true;
}
static void* pop_f32(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((float *)stack->data)[stack->top + 1];
}
static bool push_d64(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((double *)stack->data)[stack->top] = *(double *)data;

    return true;
}
static void* pop_d64(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((double *)stack->data)[stack->top + 1];
}
static bool push_b1(EmptyStack * stack, void * data)
{
    if (isFull(*stack)) return false;
    
    stack->top++;

    ((bool *)stack->data)[stack->top] = *(bool *)data;

    return true;
}
static void* pop_b1(EmptyStack * stack)
{
    if (isEmpty(*stack)) return NULL;

    stack->top--;

    return (void*)&((bool *)stack->data)[stack->top + 1];
}

bool initStack(EmptyStack * stack, DataType type, void * data, Push pushFn, Pop popFn)
{
    stack->top = -1;

    stack->data = NULL;

    switch (type)
    {
        case i8:
        stack->data = (int8_t *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_i8;
        stack->popFn = pop_i8;
        break;

        case i16:
        stack->data = (int16_t *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_i16;
        stack->popFn = pop_i16;
        break;

        case i32:
        stack->data = (int32_t *)SDL_malloc(128 * sizeof(int32_t));
        stack->pushFn = push_i32;
        stack->popFn = pop_i32;
        break;

        case i64:
        stack->data = (int64_t *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_i64;
        stack->popFn = pop_i64;
        break;

        case u8:
        stack->data = (uint8_t *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_u8;
        stack->popFn = pop_u8;
        break;

        case u16:
        stack->data = (uint8_t *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_u16;
        stack->popFn = pop_u16;
        break;

        case u32:
        stack->data = (uint8_t *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_u32;
        stack->popFn = pop_u32;
        break;

        case u64:
        stack->data = (uint8_t *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_u64;
        stack->popFn = pop_u64;
        break;

        case f32:
        stack->data = (float *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_f32;
        stack->popFn = pop_f32;
        break;

        case d64:
        stack->data = (double *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_d64;
        stack->popFn = pop_d64;
        break;

        case b1:
        stack->data = (bool *)SDL_malloc(128 * sizeof(int8_t));
        stack->pushFn = push_b1;
        stack->popFn = pop_b1;
        break;

        case self:
        stack->data = data;
        stack->popFn = popFn;
        stack->pushFn = pushFn;
        break;
    }
    stack->type = type;

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
bool deInitStack(EmptyStack * stack)
{
    SDL_free(stack->data);
    stack->top = -1;
    stack->popFn = NULL;
    stack->pushFn = NULL;

    return true;
}