#include "G_constants.h"

#include "vk_code_h/vk_synchronize.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

void createSemaphoreByBuffering(VkSemaphore (*ppSemaphore)[2])
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = NULL;
    semaphoreCreateInfo.flags = 0;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateSemaphore(allInOne.device, &semaphoreCreateInfo, allInOne.pAllocationCallbacks, (*ppSemaphore) + i), 0);
    //printf("semaphor created\n");
}
void createTimelineSemaphoreByBuffering(VkSemaphore (*ppSemaphore)[2])
{
    VkSemaphoreTypeCreateInfo timelineCreateInfo = {};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.pNext = NULL;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = &timelineCreateInfo;
    semaphoreCreateInfo.flags = 0;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateSemaphore(allInOne.device, &semaphoreCreateInfo, allInOne.pAllocationCallbacks, (*ppSemaphore) + i), 0);
    //printf("timeline semaphor created\n");
}
void createFenceByBuffering(VkFence (*ppFence)[2])
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = NULL;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        resultVulkan(vkCreateFence(allInOne.device, &fenceCreateInfo, allInOne.pAllocationCallbacks, (*ppFence) + i), 0);
    //printf("fence created\n");
}