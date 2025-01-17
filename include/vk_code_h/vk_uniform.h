#include "vulkan/vulkan.h"

#ifndef VK_UNIFORM_H
#define VK_UNIFORM_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createUniformBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDeviceMemory ** ppUniformBuffersMem, void *** pppUniformBuffersMapped, VkDeviceSize bufferSize);

#include "SDL3/SDL_close_code.h"

#endif