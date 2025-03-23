#include "vulkan/vulkan.h"

#ifndef VK_UNIFORM_H
#define VK_UNIFORM_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createUniformBufferByBuffering(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkDeviceMemory (*ppUniformBuffersMem)[2], void* (*pppUniformBuffersMapped)[2], VkDeviceSize bufferSize);
extern void SDLCALL destroyUniformBufferByBuffering(VkBuffer pUniformBuffers[2], VkDeviceMemory pUniformBuffersMem[2]);

#include "SDL3/SDL_close_code.h"

#endif