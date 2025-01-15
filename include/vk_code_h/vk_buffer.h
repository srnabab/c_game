#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H 1

VkResult G_CALL createBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pBuffer, VkDeviceMemory * pBufferMemory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
VkResult G_CALL beginSingleTimeCommands(VkDevice * pDevice, VkCommandPool * pCommandPool, VkCommandBuffer * pCommandBuffer);
VkResult G_CALL endSingleTimeCommands(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicsQueue, VkCommandBuffer * pCommandBuffer);
VkResult G_CALL copyBuffer(VkBuffer * pSrcBuffer, VkBuffer * pDstBuffer, VkDeviceSize size, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue);
int G_CALL findMemoryType(VkPhysicalDevice * pPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

#endif