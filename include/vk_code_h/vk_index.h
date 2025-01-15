#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_INDEX_H
#define VK_INDEX_H 1

void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, void ** ppIndexBufferMemMapped, uint16_t * indices, uint32_t indicesCount);

#endif