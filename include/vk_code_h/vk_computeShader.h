#include "G_begin_code.h"
#include "vulkan/vulkan.h"
#include "vk_struct.h"

#ifndef VK_COMPUTESHADER_H
#define VK_COMPUTESHADER_H 1

void createShaderStorageBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkExtent2D extent2D, VkBuffer ** ppShaderStorageBuffers, VkDeviceMemory ** ppShaderStorageBuffersMem, Particle ** ppParticles);
float randomFloat(void);
void initializeParticles(Particle ** ppParticles, VkExtent2D extent2D);

#endif