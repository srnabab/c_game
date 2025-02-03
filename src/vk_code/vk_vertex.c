#include "vk_code_h/vk_vertex.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"

void createVertexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, void ** ppVertexBufferMemMapped, Vertex * vertices, uint32_t verticesCount)
{
    FuncCode code = createVertexBufferF;

    // need change buffer size calculation
    VkDeviceSize bufferSize = sizeof(vertices[0]) * verticesCount;

    resultVulkan(createBuffer(pPhysicalDevice, pDevice, pVertexBuffer, pVertexBufferMemory, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), code, 0);
    
    vkMapMemory(*pDevice, *pVertexBufferMemory, 0, bufferSize, 0, ppVertexBufferMemMapped);
    memcpy(*ppVertexBufferMemMapped, vertices, (size_t)bufferSize);
}