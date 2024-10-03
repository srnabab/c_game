#include "vk_all.h"
#include "vk_buffer.h"

#ifndef VK_INDEX_H
#define VK_INDEX_H

void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, uint16_t * indices, uint32_t indicesCount);

#endif