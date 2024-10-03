#include "vk_queue.h"

static const uint32_t deviceExtensionCount = 1;
static const char * deviceExtensions[] = {"VK_KHR_swapchain"};

void findQueueFamilies(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, QueueFamilyIndices * pQueueFamilyIndices)
{
    FuncCode code = findQueueFamiliesF;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice, &queueFamilyCount, VK_NULL_HANDLE);
    printf("queueFamilyCount: %u\n", queueFamilyCount);

    VkQueueFamilyProperties * queueFamily = (VkQueueFamilyProperties *)malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(*pPhysicalDevice, &queueFamilyCount, queueFamily);

    for (int i = 0;i < queueFamilyCount;i++)
    {
        printf("queueFlags: %d\n", queueFamily[i].queueFlags);
    }
    bool ok1, ok2, ok3;
    ok1 = ok2 = ok3 = false;
    for (int i = 0;i < queueFamilyCount;i++)
    {
        if ((queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !ok1)
        {
            pQueueFamilyIndices->graphicsFamily = i;
            //printf("in graphicsFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok1 = true;
        }

        VkBool32 presentSupport = false;
        resultVulkan(vkGetPhysicalDeviceSurfaceSupportKHR(*pPhysicalDevice, i, *pSurface, &presentSupport), code, 1, queueFamily);
        if (presentSupport && !ok2) 
        {
            pQueueFamilyIndices->presentFamily = i;
            //printf("in presentFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok2 = true;
        }

        if ((queueFamily[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !ok3)
        {
            pQueueFamilyIndices->computeFamily = i;
            //printf("in computeFamily: %u\n", i);
            //printf("in queue count:%u\n", queueFamily[i].queueCount);
            ok3 = true;
        }

        if (ok1 && ok2 && ok3)
            break;
    }
    //printf("graphicsFamily:: %u\n", pQueueFamilyIndices->graphicsFamily);
    //printf("presentFamily: %u\n", pQueueFamilyIndices->presentFamily);

    free(queueFamily);
}
bool isDeviceSuitable(VkPhysicalDevice * pPhysicalDevice)
{
    bool extensionsSupported = !checkDeviceExtensionSupport(pPhysicalDevice);
    //printf("bool: %u\n", extensionsSupported);
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(*pPhysicalDevice, &supportedFeatures);
    //printf("supported feature sampler: %u\n", supportedFeatures.samplerAnisotropy);

    return extensionsSupported && !supportedFeatures.samplerAnisotropy;
}
bool checkDeviceExtensionSupport(VkPhysicalDevice * pPhysicalDevice)
{
    FuncCode code = checkDeviceExtensionSupportF;
    uint32_t extensionCount = 0;
    resultVulkan(vkEnumerateDeviceExtensionProperties(*pPhysicalDevice, VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE), code, 0);
    //printf("extensionCount: %u\n", extensionCount);

    VkExtensionProperties * availableExtension = (VkExtensionProperties *)malloc(extensionCount * sizeof(VkExtensionProperties));
    resultVulkan(vkEnumerateDeviceExtensionProperties(*pPhysicalDevice, VK_NULL_HANDLE, &extensionCount, availableExtension), code, 1, availableExtension);

    bool support = false;
    for (int q = 0;q < deviceExtensionCount;q++)
    {
        support = false;
        for (int i = 0;i < extensionCount;i++)
        {
            //printf("%s\n", availableExtension[i].extensionName);
            if (!strcmp(deviceExtensions[q], availableExtension[i].extensionName))
            {
                printf("extension %s supported\n", deviceExtensions[q]);
                support = true;
                break;
            }
        }
        if (!support)
            return support;
    }

    free(availableExtension);

    return support;
}
void createLogicalDevice(VkPhysicalDevice * pPhysicalDevice, QueueFamilyIndices indice, VkDevice * pDevice)
{
    FuncCode code = createLogicalDeviceF;
    resultVulkan(isDeviceSuitable(pPhysicalDevice), code, 0);

    uint32_t indices[2] = {indice.graphicsFamily, indice.presentFamily};

    VkDeviceQueueCreateInfo queueCreateInfo[2];

    float queuePriority = 1.0f;

    if (indices[1] != indices[0])
    {
        for (int i = 0;i < 2;i++)
        {
            queueCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo[i].pNext = VK_NULL_HANDLE;
            queueCreateInfo[i].flags = 0;
            queueCreateInfo[i].queueFamilyIndex = indices[i];
            queueCreateInfo[i].queueCount = 1;
            queueCreateInfo[i].pQueuePriorities = &queuePriority;
        }
    }
    else
    {
        queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[0].pNext = VK_NULL_HANDLE;
        queueCreateInfo[0].flags = 0;
        queueCreateInfo[0].queueFamilyIndex = indices[0];
        queueCreateInfo[0].queueCount = 1;
        queueCreateInfo[0].pQueuePriorities = &queuePriority;
    }

    uint32_t layersCount = 1;
    const char * validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;


    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = VK_NULL_HANDLE;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = queueCreateInfo;
    createInfo.enabledLayerCount = layersCount;
    createInfo.ppEnabledLayerNames = validationLayers;
    createInfo.enabledExtensionCount = deviceExtensionCount;
    createInfo.ppEnabledExtensionNames = deviceExtensions;
    createInfo.pEnabledFeatures = &deviceFeatures;

    if (indices[0] != indices[1])
        createInfo.queueCreateInfoCount = 2;

    resultVulkan(vkCreateDevice(*pPhysicalDevice, &createInfo, VK_NULL_HANDLE, pDevice), code, 0);
    //printf("logical device created\n");
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