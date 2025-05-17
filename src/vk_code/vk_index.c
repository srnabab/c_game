#include "vk_code_h/vk_index.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

// void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, void ** pIndexBufferMemMapped, Uint16 * indices, Uint32 indicesCount)
// {
//     FuncCode  createIndexBufferF;

//     VkDeviceSize bufferSize = sizeof(indices[0]) * indicesCount;

//     resultVulkan(createBuffer(pIndexBuffer, pIndexBufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), 0);

//     vkMapMemory(*pDevice, *pIndexBufferMemory, 0, bufferSize, 0, pIndexBufferMemMapped);
//     memcpy(*pIndexBufferMemMapped, indices, (size_t)bufferSize);
// }

void createIndexBuffer(VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, void ** pIndexBufferMemMapped, void * indices, Uint32 indicesCount, size_t indexSize, bool staging)
{
    VkDeviceSize bufferSize = indexSize * indicesCount;

    if (staging)
    {
        resultVulkan(createBuffer(pIndexBuffer, pIndexBufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), 0);

        vkMapMemory(allInOne.device, *pIndexBufferMemory, 0, bufferSize, 0, pIndexBufferMemMapped);
        memset(*pIndexBufferMemMapped, 0, (size_t)bufferSize);
    }
    else
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        resultVulkan(createBuffer(&stagingBuffer, &stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), 0);
        
        void * data;
        vkMapMemory(allInOne.device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices, bufferSize);
        vkUnmapMemory(allInOne.device, stagingBufferMemory);

        resultVulkan(createBuffer(pIndexBuffer, pIndexBufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), 0);

        copyBuffer(NULL, allInOne.graphicCommandPool, &stagingBuffer, pIndexBuffer, bufferSize);
        
        vkDestroyBuffer(allInOne.device, stagingBuffer, allInOne.pAllocationCallbacks);
        vkFreeMemory(allInOne.device, stagingBufferMemory, allInOne.pAllocationCallbacks);
    }
}