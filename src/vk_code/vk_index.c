#include "vk_code_h/vk_index.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

extern VK_ALL allInOne;

// void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, void ** pIndexBufferMemMapped, uint16_t * indices, uint32_t indicesCount)
// {
//     FuncCode code = createIndexBufferF;

//     VkDeviceSize bufferSize = sizeof(indices[0]) * indicesCount;

//     resultVulkan(createBuffer(pIndexBuffer, pIndexBufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), code, 0);

//     vkMapMemory(*pDevice, *pIndexBufferMemory, 0, bufferSize, 0, pIndexBufferMemMapped);
//     memcpy(*pIndexBufferMemMapped, indices, (size_t)bufferSize);
// }

void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, void ** pIndexBufferMemMapped, uint16_t * indices, uint32_t indicesCount)
{
    FuncCode code = createIndexBufferF;

    VkDeviceSize bufferSize = sizeof(indices[0]) * indicesCount;
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(&stagingBuffer, &stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    void * data;
    vkMapMemory(*allInOne.pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices, bufferSize);
    vkUnmapMemory(*allInOne.pDevice, stagingBufferMemory);

    resultVulkan(createBuffer(pIndexBuffer, pIndexBufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), code, 0);

    copyBuffer(&stagingBuffer, pIndexBuffer, bufferSize);
    
    vkDestroyBuffer(*allInOne.pDevice, stagingBuffer, allInOne.pAllocationCallbacks);
    vkFreeMemory(*allInOne.pDevice, stagingBufferMemory, allInOne.pAllocationCallbacks);
}