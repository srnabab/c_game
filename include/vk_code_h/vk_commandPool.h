#include "vulkan/vulkan.h"

#ifndef VK_COMMANDPOOL_H
#define VK_COMMANDPOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createCommandPool(VkDevice * pDevice, uint32_t graphicsFamilyIndice, VkCommandPool * pCommandPool);
extern void SDLCALL createCommandbuffer(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer ** pCommandBuffer);

#include "SDL3/SDL_close_code.h"

#endif