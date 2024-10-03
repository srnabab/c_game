#include "vk_all.h"
#include "vk_buffer.h"

#ifndef VK_VERTEX_H
#define VK_VERTEX_H

void createVertexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, Vertex * vertices, uint32_t verticesCount);

#endif