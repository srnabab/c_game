#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_UNIFORM_H
#define VK_UNIFORM_H 1

void createUniformBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDeviceMemory ** ppUniformBuffersMem, void *** pppUniformBuffersMapped, VkDeviceSize bufferSize);

#endif