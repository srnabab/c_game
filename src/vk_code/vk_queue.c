#include "vk_queue.h"

void findQueueFamilies(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, QueueFamilyIndices * pQueueFamilyIndices)
{
    FuncCode code = findQueueFamiliesF;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice, &queueFamilyCount, NULL);
    logMessage("queueFamilyCount: %u", queueFamilyCount);

    VkQueueFamilyProperties * queueFamily = (VkQueueFamilyProperties *)SDL_malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice, &queueFamilyCount, queueFamily);

    for (uint32_t i = 0;i < queueFamilyCount;i++)
    {
        logMessage("queueFlags: %d", queueFamily[i].queueFlags);
    }
    bool ok1, ok2, ok3;
    ok1 = ok2 = ok3 = false;
    for (uint32_t i = 0;i < queueFamilyCount;i++)
    {
        if ((queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !ok1)
        {
            pQueueFamilyIndices->graphicsFamily.familyIndice = i;
            pQueueFamilyIndices->graphicsFamily.queueCount = queueFamily[i].queueCount;
            //printf("in graphicsFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok1 = true;
        }

        VkBool32 presentSupport = false;
        resultVulkan(vkGetPhysicalDeviceSurfaceSupportKHR(*pPhysicalDevice, i, *pSurface, &presentSupport), code, 1, queueFamily);
        if (presentSupport && !ok2) 
        {
            pQueueFamilyIndices->presentFamily.familyIndice = i;
            pQueueFamilyIndices->presentFamily.queueCount = queueFamily[i].queueCount;
            //printf("in presentFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok2 = true;
        }

        if ((queueFamily[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !ok3)
        {
            pQueueFamilyIndices->computeFamily.familyIndice = i;
            pQueueFamilyIndices->computeFamily.queueCount = queueFamily[i].queueCount;
            //printf("in computeFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok3 = true;
        }

        if (ok1 && ok2 && ok3)
            break;
    }
    //printf("graphicsFamily:: %u\n", pQueueFamilyIndices->graphicsFamily);
    //printf("presentFamily: %u\n", pQueueFamilyIndices->presentFamily);

    SDL_free(queueFamily);
}
void createGraphicsQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pGraphicsQueue)
{
    vkGetDeviceQueue(*pDevice, indice, 0, pGraphicsQueue);

    //printf("graphicsQueue created\n");
}
void createPresentQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pPresentQueue)
{
    vkGetDeviceQueue(*pDevice, indice, 0, pPresentQueue);

    //printf("presentQueue created\n");
}
void createComputeQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pComputeQueue)
{
    vkGetDeviceQueue(*pDevice, indice, 0, pComputeQueue);
}