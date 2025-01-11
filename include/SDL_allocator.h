#include "SDL3/SDL_stdinc.h"

#ifndef SDL_ALLOCATOR_H
#define SDL_ALLOCATOR_H

#define BYTE_OFFSET(ptr, offset) (((char*)(ptr) + (offset)))

typedef struct _SDL_AllocatedBlock
{
    int64_t size;
    void * memory;
} SDL_AllocatedBlock;

#endif