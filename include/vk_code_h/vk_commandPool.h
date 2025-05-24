#include "SDL3/SDL_stdinc.h"

#include "vulkan/vulkan.h"

#ifndef VK_COMMANDPOOL_H
#define VK_COMMANDPOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createCommandPool(VkCommandPoolCreateFlags flag, Uint32 graphicsFamilyIndice, VkCommandPool * pCommandPool);
extern void SDLCALL createCommandBuffers(VkCommandBufferLevel level, VkCommandPool commandPool, VkCommandBuffer * pCommandBuffer, Uint32 count);
extern void SDLCALL createCommandBuffer(VkCommandBufferLevel level, VkCommandPool commandPool, VkCommandBuffer ** ppCommandBuffer, Uint32 bufferCount);
extern VkCommandPool SDLCALL getCommandPoolByIndice(Uint32 indice);
extern VkCommandBuffer SDLCALL * getCommandBufferByIndice(Uint32 indice);

#include "SDL3/SDL_close_code.h"

#endif