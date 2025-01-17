#include "vk_struct.h"

#ifndef VK_COMPUTESHADER_H
#define VK_COMPUTESHADER_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createShaderStorageBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkExtent2D extent2D, VkBuffer ** ppShaderStorageBuffers, VkDeviceMemory ** ppShaderStorageBuffersMem, Particle ** ppParticles);
extern void SDLCALL initializeParticles(Particle ** ppParticles, VkExtent2D extent2D);

#include "SDL3/SDL_close_code.h"

#endif