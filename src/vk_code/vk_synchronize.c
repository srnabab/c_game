#include "G_constants.h"

#include "vk_code_h/vk_synchronize.h"
#include "SDL_stdinc.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"
#include "vulkan_core.h"

extern VK_ALL allInOne;

void createNormalSemaphore(VkSemaphore * pSemaphore, Uint32 count)
{
    _createSemaphore(NULL, 0, pSemaphore, count);
}
void createTimelineSemaphore(Uint64 initalValue, VkSemaphore * pSemaphore, Uint32 count)
{
    VkSemaphoreTypeCreateInfo timelineCreateInfo = {0};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.pNext = NULL;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = initalValue;

    _createSemaphore(&timelineCreateInfo, 0, pSemaphore, count);
}
void _createSemaphore(void * pNext, VkSemaphoreCreateFlags flags, VkSemaphore * pSemaphore, Uint32 count)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {0};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = pNext;
    semaphoreCreateInfo.flags = flags;

    for (int i = 0;i < count;i++) vkCreateSemaphore(allInOne.device, &semaphoreCreateInfo, allInOne.pAllocationCallbacks, pSemaphore + i);
}
void destroySemaphore(VkSemaphore * pSemaphore, Uint32 count)
{
    for (int i = 0;i < count;i++) vkDestroySemaphore(allInOne.device, pSemaphore[i], allInOne.pAllocationCallbacks);
}
void createNormalFences(VkFence * pFence, Uint32 count)
{
    _createFences(NULL, VK_FENCE_CREATE_SIGNALED_BIT, pFence, count);
}
void _createFences(void * pNext, VkFenceCreateFlags flags, VkFence * pFence, Uint32 count)
{
    VkFenceCreateInfo fenceCreateInfo = {0};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = pNext;
    fenceCreateInfo.flags = flags;

    for (int i = 0;i < count;i++) vkCreateFence(allInOne.device, &fenceCreateInfo, allInOne.pAllocationCallbacks, pFence + i);
}
void destroyFence(VkFence * pFence, Uint32 count)
{
    for (int i = 0;i < count;i++) vkDestroyFence(allInOne.device, pFence[i], allInOne.pAllocationCallbacks);
}
