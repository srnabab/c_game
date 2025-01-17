#include "vulkan/vulkan.h"
#include "SDL3/SDL_stdinc.h"

#ifndef VK_ALLOC_FUNC_H
#define VK_ALLOC_FUNC_H 1

#include "SDL3/SDL_begin_code.h"

extern void * SDL_MALLOC SDL_ALLOC_SIZE(2) VKAPI_PTR SDL_VK_alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
extern void * SDL_MALLOC SDL_ALLOC_SIZE(3) VKAPI_PTR SDL_VK_realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
extern void VKAPI_PTR SDL_VK_free(void * pUserData, void * pMemory);

#include "SDL3/SDL_close_code.h"

#endif