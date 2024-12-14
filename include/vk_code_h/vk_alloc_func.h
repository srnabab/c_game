#include "core.h"

#ifndef VK_ALLOC_FUNC_H
#define VK_ALLOC_FUNC_H

#define BYTE_OFFSET(ptr, offset) (((char*)(ptr) + (offset)))

typedef struct _SDL_VK_AllocatedBlock
{
    size_t size;
    void * memory;
} SDL_VK_AllocatedBlock;
void * VKAPI_PTR SDL_VK_allocationFunc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
void * VKAPI_PTR SDL_VK_reallocationFunc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
void VKAPI_PTR SDL_VK_freeFunc(void * pUserData, void * pMemory);

#endif