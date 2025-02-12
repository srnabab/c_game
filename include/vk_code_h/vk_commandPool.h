#include "SDL3/SDL_stdinc.h"

#include "vulkan/vulkan.h"

#ifndef VK_COMMANDPOOL_H
#define VK_COMMANDPOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createCommandPool(VkCommandPoolCreateFlags flag, Uint32 graphicsFamilyIndice, VkCommandPool * pCommandPool);
extern void SDLCALL createCommandbufferByBuffering(VkCommandBufferLevel level, VkCommandPool * pCommandPool, VkCommandBuffer (*ppCommandBuffer)[MAX_FRAMES_IN_FLIGHT]);
extern void SDLCALL createCommandBuffer(VkCommandBufferLevel level, VkCommandPool * pCommandPool, VkCommandBuffer ** ppCommandBuffer, Uint32 bufferCount);

#include "SDL3/SDL_close_code.h"

#endif