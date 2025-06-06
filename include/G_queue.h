#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_mutex.h"

#ifndef G_QUEUE_H
#define G_QUEUE_H 1

struct _G_Queue;

typedef bool (*QueueOp)(struct _G_Queue *, void * data);

struct _G_Queue
{
    void * data;
    size_t dataSize;

    SDL_Mutex * mutex;
    QueueOp addTail;
    QueueOp getHead;
    QueueOp addHead;
    QueueOp getTail;
    
    int32_t capacity;
    int32_t head;
    int32_t tail;
};
typedef struct _G_Queue G_Queue;

extern bool SDLCALL initQueue(G_Queue * queue, size_t dataSize, int32_t capacity, QueueOp addTail, QueueOp getHead, QueueOp addHead, QueueOp getTail);
extern bool SDLCALL G_QueueIsFull(G_Queue queue);
extern bool SDLCALL G_QueueIsEmpty(G_Queue queue);
extern bool SDLCALL G_QueueResize(G_Queue * queue, int newCapacity);
extern void SDLCALL G_deInitQueue(G_Queue * queue);

#endif // G_queue.h
