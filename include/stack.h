#include <stdio.h>
#include <stdbool.h>

#ifndef STACK_H
#define STACK_H

#define MAX_STACKS 128

typedef enum _DataType
{
    self,
    i8, u8, i16, u16, i32, u32, i64, u64,
    f32, d64, b1
} DataType;

struct _EmptyStack;

typedef bool (*Push)(struct _EmptyStack * stack, void * data);
typedef bool (*Pop)(struct _EmptyStack * stack, void * data);

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
bool initStack(EmptyStack * stack, DataType type, void * data, Push pushFn, Pop popFn);
bool isEmpty(EmptyStack stack);
bool isFull(EmptyStack stack);
bool deInitStack(EmptyStack * stack);

#endif