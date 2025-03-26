#include "G_constants.h"

#include "vk_code_h/vk_synchronize.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

void createSemaphoreByBuffering(VkSemaphore (*ppSemaphore)[2])
{
    FuncCode code = createSemaphoreF;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = NULL;
    semaphoreCreateInfo.flags = 0;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateSemaphore(*allInOne.pDevice, &semaphoreCreateInfo, allInOne.pAllocationCallbacks, (*ppSemaphore) + i), code, 0);
    //printf("semaphor created\n");
}
void createFenceByBuffering(VkFence (*ppFence)[2])
{
    FuncCode code = createFenceF;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = NULL;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateFence(*allInOne.pDevice, &fenceCreateInfo, allInOne.pAllocationCallbacks, (*ppFence) + i), code, 0);
    //printf("fence created\n");
}