#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_SYNCHRONIZE_H
#define VK_SYNCHRONIZE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createSemaphore(VkDevice * pDevice, VkSemaphore ** pSemaphore);
extern void SDLCALL createFence(VkDevice * pDevice, VkFence ** pFence);

#include "SDL3/SDL_close_code.h"

#endif