#include "vk_index.h"

void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, uint16_t * indices, uint32_t indicesCount)
{
    FuncCode code = createIndexBufferF;

    VkDeviceSize bufferSize = sizeof(indices[0]) * indicesCount;

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    resultVulkan(createBuffer(pPhysicalDevice, pDevice, &stagingBuffer, &stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), code, 0);
    
    void * data = VK_NULL_HANDLE;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices, (size_t)bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);

    resultVulkan(createBuffer(pPhysicalDevice, pDevice, pIndexBuffer, pIndexBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), code, 0);

    resultVulkan(copyBuffer(&stagingBuffer, pIndexBuffer, bufferSize, pDevice, pCommandPool, pGraphicQueue), code, 0);

    vkDestroyBuffer(*pDevice, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(*pDevice, stagingBufferMemory, VK_NULL_HANDLE);
}