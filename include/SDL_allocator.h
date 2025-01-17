#include "SDL3/SDL_stdinc.h"

#ifndef SDL_ALLOCATOR_H
#define SDL_ALLOCATOR_H 1

#include "SDL3/SDL_begin_code.h"

#define BYTE_OFFSET(ptr, offset) (((char*)(ptr) + (offset)))

typedef struct _SDL_AllocatedBlock
{
    int64_t size;
    void * memory;
} SDL_AllocatedBlock;

#include "SDL3/SDL_close_code.h"

#endif