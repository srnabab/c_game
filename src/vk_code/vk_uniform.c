#include "vk_code_h/vk_uniform.h"

#include "G_constants.h"

#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

void createUniformBufferByBuffering(VkBuffer (*ppUniformBuffers)[2], VkDeviceMemory (*ppUniformBuffersMem)[2], void* (*pppUniformBuffersMapped)[2], VkDeviceSize bufferSize)
{
    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        (*pppUniformBuffersMapped)[i] = NULL;

        resultVulkan(createBuffer(&(*ppUniformBuffers)[i], &(*ppUniformBuffersMem)[i], bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
        3, *ppUniformBuffers, *ppUniformBuffersMem, *pppUniformBuffersMapped);

        resultVulkan(vkMapMemory(allInOne.device, (*ppUniformBuffersMem)[i], 0, bufferSize, 0, &(*pppUniformBuffersMapped)[i]),
        3, *ppUniformBuffers, *ppUniformBuffersMem, *pppUniformBuffersMapped);
    }
}
