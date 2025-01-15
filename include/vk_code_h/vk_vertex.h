#include "G_begin_code.h"
#include "vulkan/vulkan.h"
#include "vk_struct.h"

#ifndef VK_VERTEX_H
#define VK_VERTEX_H 1

void createVertexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, void ** ppVertexBufferMemMapped, Vertex * vertices, uint32_t verticesCount);

#endif