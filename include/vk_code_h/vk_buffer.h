#include "vk_all.h"

#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H

VkResult createBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pBuffer, VkDeviceMemory * pBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
VkResult beginSingleTimeCommands(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer * pCommandBuffer);
VkResult endSingleTimeCommands(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicsQueue, VkCommandBuffer * pCommandBuffer);
VkResult copyBuffer(VkBuffer * pSrcBuffer, VkBuffer * pDstBuffer, VkDeviceSize size, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue);
uint32_t findMemoryType(VkPhysicalDevice * pPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

#endif