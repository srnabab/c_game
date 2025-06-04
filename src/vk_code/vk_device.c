#include "vk_code_h/vk_device.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_pop_window.h"
#include "G_log.h"
#include "G_allocator.h"

#define RTX_2060 0
#define INTEL_GPU 1
#define GPU_CHOOSED RTX_2060

extern VK_ALL allInOne;

static Uint64 getPhysicalDeviceTotalMemory(VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties)
{
	Uint64 physicalDeviceTotalMemory = 0;
	for(Uint32 i = 0; i < pPhysicalDeviceMemoryProperties->memoryHeapCount; i++)
	{
		if((pPhysicalDeviceMemoryProperties->memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)
		{
			physicalDeviceTotalMemory += pPhysicalDeviceMemoryProperties->memoryHeaps[i].size;
		}
	}
	print("device memory: %llu", physicalDeviceTotalMemory);
	return physicalDeviceTotalMemory;
}
static bool requiredExtensionSupportedCheck(Uint32 extensionCount, char ** extension, Uint32 physicalDeviceExtensionCount, VkExtensionProperties * pExtensionProperties)
{
    Uint32 count = -1;//u32 overflow
    for (Uint32 i = 0;i < extensionCount;i++)
    {
        for (Uint32 q = 0;q < physicalDeviceExtensionCount;q++)
        {
            if (SDL_strcmp(extension[i], pExtensionProperties[q].extensionName) == 0)
            {
                count++;
                break;
            }
        }
        if (count != i)
        {
            pushMessage(SDL_MESSAGEBOX_WARNING, "Error", "Vulkan Error\nExtension: %s not supported by GPU", extension[i]);
            return false;
        }
    }
    count++;
    if (count != extensionCount)
    {
        print("extension not supported");
        return false;
    }

    return true;
}
static int getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, Uint32 physicalDeviceNumber)
{
	VkPhysicalDeviceProperties *physicalDeviceProperties = (VkPhysicalDeviceProperties *)G_malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceProperties));
	VkPhysicalDeviceMemoryProperties *physicalDeviceMemoryProperties = (VkPhysicalDeviceMemoryProperties *)G_malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceMemoryProperties));

	int index = -1;
    Uint64 bestMemory = 0;

    const Uint32 requiredDeviceExtensionCount = 1;
    const char * requiredDeviceExtensions[1] = {
        "VK_KHR_swapchain",
    };

	for(Uint32 i = 0; i < physicalDeviceNumber; i++)
	{
        Uint32 physicalDeviceExtensionCount = 0;
        vkEnumerateDeviceExtensionProperties(pPhysicalDevices[i], NULL, &physicalDeviceExtensionCount, NULL);
        VkExtensionProperties * physicalDeviceExtension = (VkExtensionProperties*)G_malloc(physicalDeviceExtensionCount * sizeof(VkExtensionProperties));
        vkEnumerateDeviceExtensionProperties(pPhysicalDevices[i], NULL, &physicalDeviceExtensionCount, physicalDeviceExtension);

        if (!requiredExtensionSupportedCheck(requiredDeviceExtensionCount, (char**)requiredDeviceExtensions, physicalDeviceExtensionCount, physicalDeviceExtension))
        {
            G_free(physicalDeviceExtension);
            continue;
        }

		vkGetPhysicalDeviceProperties(pPhysicalDevices[i], &physicalDeviceProperties[i]);
		
		vkGetPhysicalDeviceMemoryProperties(pPhysicalDevices[i], &physicalDeviceMemoryProperties[i]);

		if (physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
            if (index == -1)
            {
                index = i;
                bestMemory = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[i]);
            }
            else if (physicalDeviceProperties[index].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                index = i;
                bestMemory = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[i]);
            }
            else if (physicalDeviceProperties[index].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                Uint64 memoryTemp = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[i]);
                if (bestMemory < memoryTemp)
                {
                    index = i;
                    bestMemory = memoryTemp;
                }
            }
		}
		else if (physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
            if (index == -1)
            {
                index = i;
                bestMemory = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[i]);
            }
            else if (physicalDeviceProperties[index].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                Uint64 memoryTemp = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[i]);
                if (bestMemory < memoryTemp)
                {
                    index = i;
                    bestMemory = memoryTemp;
                }
            }
            else if (physicalDeviceProperties[index].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                ;
            }
		}

		Uint32 apiVersion = physicalDeviceProperties[i].apiVersion;
		print("apiVersion: %u.%u.%u", VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion), VK_API_VERSION_PATCH(apiVersion));

		Uint32 driverVersion = physicalDeviceProperties[i].driverVersion;
		print("driverVersion: %u.%u", (driverVersion >> 14) & 0x3FF, driverVersion & 0x3FFF);

		print("vendor Id: %x", physicalDeviceProperties[i].vendorID);

		print("device Id: %x", physicalDeviceProperties[i].deviceID);

		print("device type: %u", physicalDeviceProperties[i].deviceType);

		print(physicalDeviceProperties[i].deviceName);
	
		print("max uniform buffers: %u, max storage buffers: %u", physicalDeviceProperties[i].limits.maxDescriptorSetUniformBuffers, physicalDeviceProperties[i].limits.maxDescriptorSetStorageBuffers);

        print("max image samplers: %u", physicalDeviceProperties[i].limits.maxPerStageDescriptorSamplers);

		print("max color attachments: %u", physicalDeviceProperties[i].limits.maxColorAttachments);

		char pipelineCacheUUID[VK_UUID_SIZE * 2];
		for (unsigned j = 0;j < VK_UUID_SIZE;j++)
		{
			SDL_snprintf(pipelineCacheUUID + 2 * j, 255, "%x", physicalDeviceProperties[i].pipelineCacheUUID[j]);
		}
		print(pipelineCacheUUID);

        G_free(physicalDeviceExtension);
	}

	G_free(physicalDeviceMemoryProperties);
	G_free(physicalDeviceProperties);

	return index;
}
void pickPhysicalDevice(void)
{
    Uint32 deviceCount = 0;
    resultVulkan(vkEnumeratePhysicalDevices(allInOne.instance, &deviceCount, NULL), 0);

    if (deviceCount == 0)
    {
        print("failed to find GPUs with Vulkan support");
    }

    VkPhysicalDevice * devices = (VkPhysicalDevice *)G_malloc(deviceCount * sizeof(VkPhysicalDevice));
    resultVulkan(vkEnumeratePhysicalDevices(allInOne.instance, &deviceCount, devices), 1, devices);

	VkPhysicalDevice device = devices[getBestPhysicalDeviceIndex(devices, deviceCount)];
    device = devices[GPU_CHOOSED];
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	if (deviceFeatures.geometryShader == VK_TRUE && deviceFeatures.independentBlend == VK_TRUE && deviceFeatures.wideLines == VK_TRUE)
	{
		G_free(devices);
		//printf("devive picked\n");
		allInOne.physicalDevice = device;
	}
    else
    {
        G_free(devices);
    }

    if (GPU_CHOOSED) print("device picked: INTEL_GPU");
    else print("device picked: RTX_2060");
}
static bool * extensionSupportedCheck_Optional(Uint32 neededExtensionCount, char ** neededExtensions, Uint32 extensionCount, VkExtensionProperties * pExtensionProperties)
{
    Uint32 count = -1;//u32 overflow
    bool * group = (bool*)G_malloc(sizeof(bool) * neededExtensionCount);
    for (Uint32 i = 0;i < neededExtensionCount;i++)
    {
        group[i] = false;
        for (Uint32 q = 0;q < extensionCount;q++)
        {
            if (SDL_strcmp(neededExtensions[i], pExtensionProperties[q].extensionName) == 0)
            {
                count++;
                group[i] = true;
                break;
            }
        }
    }
    count++;

    return group;
}
static Uint32 configureQueueCreateInfo(VkDeviceQueueCreateInfo * pCreateInfo, float * pQueuePriority)
{
    Uint32 queueFamilyCount = 0;

    if (allInOne.queueFamilyIndices.graphicsFamily.familyIndice != -1)
    {
        pCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        pCreateInfo[0].pNext = NULL;
        pCreateInfo[0].flags = 0;
        pCreateInfo[0].queueFamilyIndex = allInOne.queueFamilyIndices.graphicsFamily.familyIndice;
        pCreateInfo[0].queueCount = allInOne.queueFamilyIndices.graphicsFamily.queueCount;
        pCreateInfo[0].pQueuePriorities = pQueuePriority;
        queueFamilyCount++;
    }

    if (allInOne.queueFamilyIndices.computeFamily.familyIndice != -1)
    {
        pCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        pCreateInfo[1].pNext = NULL;
        pCreateInfo[1].flags = 0;
        pCreateInfo[1].queueFamilyIndex = allInOne.queueFamilyIndices.computeFamily.familyIndice;
        pCreateInfo[1].queueCount = allInOne.queueFamilyIndices.computeFamily.queueCount;
        pCreateInfo[1].pQueuePriorities = pQueuePriority;
        queueFamilyCount++;
    }

    if (allInOne.queueFamilyIndices.transferFamily.familyIndice != -1)
    {
        pCreateInfo[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        pCreateInfo[2].pNext = NULL;
        pCreateInfo[2].flags = 0;
        pCreateInfo[2].queueFamilyIndex = allInOne.queueFamilyIndices.transferFamily.familyIndice;
        pCreateInfo[2].queueCount = allInOne.queueFamilyIndices.transferFamily.queueCount;
        pCreateInfo[2].pQueuePriorities = pQueuePriority;
        queueFamilyCount++;       
    }

    if (allInOne.queueFamilyIndices.presentFamily.familyIndice != -1)
    {
        if (allInOne.queueFamilyIndices.presentFamily.familyIndice == allInOne.queueFamilyIndices.graphicsFamily.familyIndice)
        {
            pCreateInfo[0].queueCount++;
        }
        else
        {
            pCreateInfo[3].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            pCreateInfo[3].pNext = NULL;
            pCreateInfo[3].flags = 0;
            pCreateInfo[3].queueFamilyIndex = allInOne.queueFamilyIndices.presentFamily.familyIndice;
            pCreateInfo[3].queueCount = allInOne.queueFamilyIndices.presentFamily.queueCount;
            pCreateInfo[3].pQueuePriorities = pQueuePriority;
            queueFamilyCount++;
        }
    }
    return queueFamilyCount;
}
void createLogicalDevice(void)
{
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(allInOne.physicalDevice, &supportedFeatures);

    if (!supportedFeatures.samplerAnisotropy)
    {
        resultVulkan(VK_ERROR_INITIALIZATION_FAILED, 0);
    }
    
    const Uint32 requiredDeviceExtensionCount = 1;
    const char * requiredDeviceExtensions[1] = {
        "VK_KHR_swapchain",
    };
    Uint32 optionalDeviceExtensionCount = 9;
    const char * vmaExtension[9] = {
        "VK_KHR_dedicated_allocation",
        "VK_KHR_bind_memory2",
        "VK_KHR_maintenance4",
        "VK_KHR_maintenance5",
        "VK_EXT_memory_budget",
        "VK_KHR_buffer_device_address",
        "VK_EXT_memory_priority",
        "VK_AMD_device_coherent_memory",
        "VK_KHR_external_memory_win32",
    };

    Uint32 physicalDeviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(allInOne.physicalDevice, NULL, &physicalDeviceExtensionCount, NULL);
    VkExtensionProperties * physicalDeviceExtension = (VkExtensionProperties*)G_malloc(physicalDeviceExtensionCount * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(allInOne.physicalDevice, NULL, &physicalDeviceExtensionCount, physicalDeviceExtension);

    bool * enabledGroup = extensionSupportedCheck_Optional(optionalDeviceExtensionCount, (char **)vmaExtension, physicalDeviceExtensionCount, physicalDeviceExtension);
    G_free(physicalDeviceExtension);
    char ** enabledExtension = (char**)G_malloc(sizeof(char**) * (optionalDeviceExtensionCount + requiredDeviceExtensionCount));

    Uint32 enabledExtensionCount = 0;
    enabledExtension[0] = (char *)requiredDeviceExtensions[0];
    enabledExtensionCount++;

    for (int i = 0;i < optionalDeviceExtensionCount;i++)
    {
        if (enabledGroup[i])
        {
            enabledExtension[enabledExtensionCount] = (char*)vmaExtension[i];
            enabledExtensionCount++;
            print("device extension:%s supported", vmaExtension[i]);
        }
        else
        {
            print("device extension:%s not supported by device", vmaExtension[i]);
        }
    }
    G_free(enabledGroup);

    VkDeviceQueueCreateInfo queueCreateInfo[4];

    float queuePriority[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    Uint32 queueFamilyCount = configureQueueCreateInfo(queueCreateInfo, queuePriority);

    Uint32 layersCount = 1;
    const char * validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeatures = {0};
    timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
    timelineSemaphoreFeatures.pNext = NULL;
    timelineSemaphoreFeatures.timelineSemaphore = VK_TRUE;

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.independentBlend = VK_TRUE;
    deviceFeatures.wideLines = VK_TRUE;

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {0};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &timelineSemaphoreFeatures;
    deviceFeatures2.features = deviceFeatures;

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &deviceFeatures2;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pQueueCreateInfos = queueCreateInfo;
    createInfo.enabledLayerCount = layersCount;
    createInfo.ppEnabledLayerNames = validationLayers;
    createInfo.enabledExtensionCount = enabledExtensionCount;
    createInfo.ppEnabledExtensionNames = (const char* const *)enabledExtension;
    createInfo.pEnabledFeatures = NULL;

    resultVulkan(vkCreateDevice(allInOne.physicalDevice, &createInfo, allInOne.pAllocationCallbacks, &allInOne.device), 0);

    G_free(enabledExtension);
    //printf("logical device created\n");
}
