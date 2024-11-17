#include "core.h"

#ifndef VK_UNIFORM_H
#define VK_UNIFORM_H

void createUniformBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDeviceMemory ** ppUniformBuffersMem, void *** pppUniformBuffersMapped);

#endif