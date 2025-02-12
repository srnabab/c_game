#include "vk_struct.h"

#ifndef VK_COMPUTESHADER_H
#define VK_COMPUTESHADER_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createShaderStorageBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer (*ppShaderStorageBuffers)[2], VkDeviceMemory (*ppShaderStorageBuffersMem)[2]);

#include "SDL3/SDL_close_code.h"

#endif