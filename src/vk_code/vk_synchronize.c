#include "G_constants.h"
#include "vk_code_h/vk_synchronize.h"
#include "vk_code_h/vk_judge.h"

void createSemaphore(VkDevice * pDevice, VkSemaphore ** pSemaphore)
{
    FuncCode code = createSemaphoreF;

    *pSemaphore = (VkSemaphore *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = NULL;
    semaphoreCreateInfo.flags = 0;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateSemaphore(*pDevice, &semaphoreCreateInfo, NULL, &((*pSemaphore)[i])), code, 0);
    //printf("semaphor created\n");
}
void createFence(VkDevice * pDevice, VkFence ** pFence)
{
    FuncCode code = createFenceF;

    *pFence = (VkFence *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkFence));

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = NULL;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateFence(*pDevice, &fenceCreateInfo, NULL, &((*pFence)[i])), code, 0);
    //printf("fence created\n");
}