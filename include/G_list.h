#include "SDL3/SDL_stdinc.h"

#ifndef LIST_H
#define LIST_H 1

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