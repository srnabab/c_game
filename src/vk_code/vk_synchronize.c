#include "vk_synchronize.h"

void createSemaphore(VkDevice * pDevice, VkSemaphore ** pSemaphore)
{
    FuncCode code = createSemaphoreF;

    *pSemaphore = (VkSemaphore *)malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        VK_NULL_HANDLE,
        0
    };

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateSemaphore(*pDevice, &semaphoreCreateInfo, VK_NULL_HANDLE, &((*pSemaphore)[i])), code, 0);
    //printf("semaphor created\n");
}
void createFence(VkDevice * pDevice, VkFence ** pFence)
{
    FuncCode code = createFenceF;

    *pFence = (VkFence *)malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkFence));

    VkFenceCreateInfo fenceCreateInfo = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        VK_NULL_HANDLE,
        VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateFence(*pDevice, &fenceCreateInfo, VK_NULL_HANDLE, &((*pFence)[i])), code, 0);
    //printf("fence created\n");
}