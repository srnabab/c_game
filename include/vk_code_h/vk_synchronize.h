#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_SYNCHRONIZE_H
#define VK_SYNCHRONIZE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createNormalSemaphore(VkSemaphore * pSemaphore, Uint32 count);
extern void SDLCALL createTimelineSemaphore(Uint64 initalValue, VkSemaphore * pSemaphore, Uint32 count);
extern void SDLCALL _createSemaphore(void * pNext, VkSemaphoreCreateFlags flags, VkSemaphore * pSemaphore, Uint32 count);
extern void SDLCALL destroySemaphore(VkSemaphore * pSemaphore, Uint32 count);
extern void SDLCALL createNormalFences(VkFence * pFence, Uint32 count);
extern void SDLCALL _createFences(void * pNext, VkFenceCreateFlags flags, VkFence * pFence, Uint32 count);
extern void SDLCALL destroyFence(VkFence * pFence, Uint32 count);

#include "SDL3/SDL_close_code.h"

#endif
