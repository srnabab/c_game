#include "core.h"

#ifndef VK_ALLOC_FUNC_H
#define VK_ALLOC_FUNC_H

void * VKAPI_PTR SDL_VK_alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
void * VKAPI_PTR SDL_VK_realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
void VKAPI_PTR SDL_VK_free(void * pUserData, void * pMemory);

#endif