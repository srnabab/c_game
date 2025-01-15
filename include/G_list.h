#include "std_c.h"
#include "SDL3/SDL.h"

#ifndef LIST_H
#define LIST_H

typedef struct _Node
{
    void * data;
    struct _Node * next;
} Node;

typedef struct _List
{
    Node * head;
    bool (*compareFn)(void *, void *);
} List;

void initList(List list, bool (*compareFn)(void *, void *));
bool insertNodeEnd(List list, void * data);
Node * searchPreNode(List list, void * data);
bool deleteNode(List list, void * data);
Node * toEnd(List list);

#endif