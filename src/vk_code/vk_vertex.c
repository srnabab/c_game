#include "vk_vertex.h"

void createVertexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, Vertex * vertices, uint32_t verticesCount)
{
    FuncCode code = createVertexBufferF;

    VkDeviceSize bufferSize = sizeof(vertices[0]) * verticesCount;

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    resultVulkan(createBuffer(pPhysicalDevice, pDevice, &stagingBuffer, &stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), code, 0);
    
    void * data = VK_NULL_HANDLE;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);

    resultVulkan(createBuffer(pPhysicalDevice, pDevice, pVertexBuffer, pVertexBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), code, 0);

    resultVulkan(copyBuffer(&stagingBuffer, pVertexBuffer, bufferSize, pDevice, pCommandPool, pGraphicQueue), code, 0);

    vkDestroyBuffer(*pDevice, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(*pDevice, stagingBufferMemory, VK_NULL_HANDLE);
}