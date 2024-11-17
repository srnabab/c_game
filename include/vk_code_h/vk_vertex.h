#include "core.h"

#ifndef VK_VERTEX_H
#define VK_VERTEX_H

void createVertexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, void ** ppVertexBufferMemMapped, Vertex * vertices, uint32_t verticesCount);

#endif