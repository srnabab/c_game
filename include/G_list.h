#include "SDL3/SDL_stdinc.h"

#ifndef LIST_H
#define LIST_H 1

#include "SDL3/SDL_begin_code.h"

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

extern void SDLCALL initList(List list, bool (*compareFn)(void *, void *));
extern bool SDLCALL insertNodeEnd(List list, void * data);
extern Node* SDLCALL searchPreNode(List list, void * data);
extern bool SDLCALL deleteNode(List list, void * data);
extern Node* SDLCALL toEnd(List list);

#include "SDL3/SDL_close_code.h"

#endif