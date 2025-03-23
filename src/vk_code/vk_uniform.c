#include "vk_code_h/vk_uniform.h"

#include "G_constants.h"

#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

extern VK_ALL allInOne;

void createUniformBufferByBuffering(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer (*ppUniformBuffers)[2], VkDeviceMemory (*ppUniformBuffersMem)[2], void* (*pppUniformBuffersMapped)[2], VkDeviceSize bufferSize)
{
    FuncCode code = createUniformBuffersF;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        (*pppUniformBuffersMapped)[i] = NULL;

        resultVulkan(createBuffer(&(*ppUniformBuffers)[i], &(*ppUniformBuffersMem)[i], bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
        code, 3, *ppUniformBuffers, *ppUniformBuffersMem, *pppUniformBuffersMapped);

        resultVulkan(vkMapMemory(*pDevice, (*ppUniformBuffersMem)[i], 0, bufferSize, 0, &(*pppUniformBuffersMapped)[i]),
        code, 3, *ppUniformBuffers, *ppUniformBuffersMem, *pppUniformBuffersMapped);
    }
}
void destroyUniformBufferByBuffering(VkBuffer pUniformBuffers[2], VkDeviceMemory pUniformBuffersMem[2])
{
    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        vkUnmapMemory(*allInOne.pDevice, pUniformBuffersMem[i]);
        vkDestroyBuffer(*allInOne.pDevice, pUniformBuffers[i], allInOne.pAllocationCallbacks);
        vkFreeMemory(*allInOne.pDevice, pUniformBuffersMem[i], allInOne.pAllocationCallbacks);
    }
}