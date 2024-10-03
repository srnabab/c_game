#include "vk_all.h"
#include "vk_buffer.h"

#ifndef VK_UNIFORM_H
#define VK_UNIFORM_H

void createUniformBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDeviceMemory ** ppUniformBuffersMem, void *** pppUniformBuffersMapped);

#endif