#include "SDL3/SDL_stdinc.h"
#include "vk_code_h/vk_index.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"

void createIndexBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pIndexBuffer, VkDeviceMemory * pIndexBufferMemory, void ** pIndexBufferMemMapped, uint16_t * indices, uint32_t indicesCount)
{
    FuncCode code = createIndexBufferF;

    VkDeviceSize bufferSize = sizeof(indices[0]) * indicesCount;

    resultVulkan(createBuffer(pPhysicalDevice, pDevice, pIndexBuffer, pIndexBufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), code, 0);

    vkMapMemory(*pDevice, *pIndexBufferMemory, 0, bufferSize, 0, pIndexBufferMemMapped);
    memcpy(*pIndexBufferMemMapped, indices, (size_t)bufferSize);
}