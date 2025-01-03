#include "vk_uniform.h"
#include "vk_buffer.h"

void createUniformBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer ** ppUniformBuffers, VkDeviceMemory ** ppUniformBuffersMem, void *** pppUniformBuffersMapped, VkDeviceSize bufferSize)
{
    FuncCode code = createUniformBuffersF;

    *ppUniformBuffers = (VkBuffer *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkBuffer));
    *ppUniformBuffersMem = (VkDeviceMemory *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkDeviceMemory));
    *pppUniformBuffersMapped = (void **)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(void*));

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        (*pppUniformBuffersMapped)[i] = NULL;

        resultVulkan(createBuffer(pPhysicalDevice, pDevice, &(*ppUniformBuffers)[i], &(*ppUniformBuffersMem)[i], bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
        code, 3, *ppUniformBuffers, *ppUniformBuffersMem, *pppUniformBuffersMapped);

        resultVulkan(vkMapMemory(*pDevice, (*ppUniformBuffersMem)[i], 0, bufferSize, 0, &(*pppUniformBuffersMapped)[i]),
        code, 3, *ppUniformBuffers, *ppUniformBuffersMem, *pppUniformBuffersMapped);

        //printf("void*: %p\n", (*pppUniformBuffersMapped)[i]);
    }
}