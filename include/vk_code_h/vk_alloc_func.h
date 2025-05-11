#include "vulkan/vulkan.h"
#include "SDL3/SDL_stdinc.h"

#ifndef VK_ALLOC_FUNC_H
#define VK_ALLOC_FUNC_H 1

#include "SDL3/SDL_begin_code.h"

extern void * SDL_MALLOC VKAPI_PTR VK_alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
extern void * SDL_MALLOC VKAPI_PTR VK_realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
extern void VKAPI_PTR VK_free(void * pUserData, void * pMemory);

#include "SDL3/SDL_close_code.h"

#endif