#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_struct.h"
#include "vk_code_h/vk_judge.h"

#include "G_log.h"

extern VK_ALL allInOne;

void findQueueFamilies(void)
{
    FuncCode code = findQueueFamiliesF;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*allInOne.pPhysicalDevice, &queueFamilyCount, NULL);
    logMessage("queueFamilyCount: %u", queueFamilyCount);

    VkQueueFamilyProperties * queueFamily = (VkQueueFamilyProperties *)SDL_malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(*allInOne.pPhysicalDevice, &queueFamilyCount, queueFamily);

    for (uint32_t i = 0;i < queueFamilyCount;i++)
    {
        logMessage("queueFlags: %d", queueFamily[i].queueFlags);
    }
    bool ok1, ok2, ok3, ok4;
    ok1 = ok2 = ok3 = ok4 = false;
    for (uint32_t i = 0;i < queueFamilyCount;i++)
    {
        if ((queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !ok1)
        {
            allInOne.pQueueFamilyIndices->graphicsFamily.familyIndice = i;
            allInOne.pQueueFamilyIndices->graphicsFamily.queueCount = queueFamily[i].queueCount;
            //printf("in graphicsFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok1 = true;
        }

        VkBool32 presentSupport = false;
        resultVulkan(vkGetPhysicalDeviceSurfaceSupportKHR(*allInOne.pPhysicalDevice, i, *allInOne.pSurface2D, &presentSupport), code, 1, queueFamily);
        if (presentSupport && !ok2) 
        {
            allInOne.pQueueFamilyIndices->presentFamily.familyIndice = i;
            allInOne.pQueueFamilyIndices->presentFamily.queueCount = queueFamily[i].queueCount;
            //printf("in presentFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok2 = true;
        }

        if ((queueFamily[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !ok3)
        {
            allInOne.pQueueFamilyIndices->computeFamily.familyIndice = i;
            allInOne.pQueueFamilyIndices->computeFamily.queueCount = queueFamily[i].queueCount;
            //printf("in computeFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok3 = true;
        }

        if ((queueFamily[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && !ok4)
        {
            allInOne.pQueueFamilyIndices->transferFamily.familyIndice = i;
            allInOne.pQueueFamilyIndices->transferFamily.queueCount = queueFamily[i].queueCount;
            ok4 = true;
        }

        if (ok1 && ok2 && ok3 && ok4)
            break;
    }
    //printf("graphicsFamily:: %u\n", pQueueFamilyIndices->graphicsFamily);
    //printf("presentFamily: %u\n", pQueueFamilyIndices->presentFamily);

    SDL_free(queueFamily);
}
void createQueue(Uint32 index, VkQueue * pQueue)
{
    vkGetDeviceQueue(*allInOne.pDevice, index, 0, pQueue);
}