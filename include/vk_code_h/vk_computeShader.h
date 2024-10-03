#include "vk_all.h"
#include "vk_buffer.h"

#ifndef VK_COMPUTESHADER_H
#define VK_COMPUTESHADER_H

void createShaderStorageBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkExtent2D extent2D, VkBuffer ** ppShaderStorageBuffers, VkDeviceMemory ** ppShaderStorageBuffersMem, Particle ** ppParticles);
float randomFloat(void);
void initializeParticles(Particle ** ppParticles, VkExtent2D extent2D);

#endif