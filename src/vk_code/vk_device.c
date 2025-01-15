#include "vk_code_h/vk_device.h"
#include "vk_code_h/vk_judge.h"
#include "SDL3/SDL_stdinc.h"
#include "G_pop_window.h"
#include "G_log.h"

void pickPhysicalDevice(VkInstance * pInstance, VkPhysicalDevice * pPhysicalDevice)
{
	FuncCode code = pickPhysicalDeviceF;
    uint32_t deviceCount = 0;
    resultVulkan(vkEnumeratePhysicalDevices(*pInstance, &deviceCount, NULL), code, 0);

    if (deviceCount == 0)
    {
        logMessage("failed to find GPUs with Vulkan support");
    }

    VkPhysicalDevice * devices = (VkPhysicalDevice *)SDL_malloc(deviceCount * sizeof(VkPhysicalDevice));
    resultVulkan(vkEnumeratePhysicalDevices(*pInstance, &deviceCount, devices), code, 1, devices);

	VkPhysicalDevice device = devices[getBestPhysicalDeviceIndex(devices, deviceCount)];
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	if (deviceFeatures.geometryShader)
	{
		SDL_free(devices);
		//printf("devive picked\n");
		*pPhysicalDevice = device;
	}
    else
    {
        SDL_free(devices);
    }
}
uint64_t getPhysicalDeviceTotalMemory(VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties)
{
	uint64_t physicalDeviceTotalMemory = 0;
	for(uint32_t i = 0; i < pPhysicalDeviceMemoryProperties->memoryHeapCount; i++)
	{
		if((pPhysicalDeviceMemoryProperties->memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)
		{
			physicalDeviceTotalMemory += pPhysicalDeviceMemoryProperties->memoryHeaps[i].size;
		}
	}
	logMessage("device memory: %llu", physicalDeviceTotalMemory);
	return physicalDeviceTotalMemory;
}
static bool requiredExtensionSupportedCheck(uint32_t extensionCount, char ** extension, uint32_t physicalDeviceExtensionCount, VkExtensionProperties * pExtensionProperties)
{
    uint32_t count = -1;//u32 overflow
    for (uint32_t i = 0;i < extensionCount;i++)
    {
        for (uint32_t q = 0;q < physicalDeviceExtensionCount;q++)
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
        logMessage("extension not supported");
        return false;
    }

    return true;
}
int getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, uint32_t physicalDeviceNumber)
{
	VkPhysicalDeviceProperties *physicalDeviceProperties = (VkPhysicalDeviceProperties *)SDL_malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceProperties));
	VkPhysicalDeviceMemoryProperties *physicalDeviceMemoryProperties = (VkPhysicalDeviceMemoryProperties *)SDL_malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceMemoryProperties));

	int index = -1;
    uint64_t bestMemory = 0;

    const uint32_t requiredDeviceExtensionCount = 1;
    const char * requiredDeviceExtensions[1] = {
        "VK_KHR_swapchain",
    };

	for(uint32_t i = 0; i < physicalDeviceNumber; i++)
	{
        uint32_t physicalDeviceExtensionCount = 0;
        vkEnumerateDeviceExtensionProperties(pPhysicalDevices[i], NULL, &physicalDeviceExtensionCount, NULL);
        VkExtensionProperties * physicalDeviceExtension = (VkExtensionProperties*)SDL_malloc(physicalDeviceExtensionCount * sizeof(VkExtensionProperties));
        vkEnumerateDeviceExtensionProperties(pPhysicalDevices[i], NULL, &physicalDeviceExtensionCount, physicalDeviceExtension);

        if (!requiredExtensionSupportedCheck(requiredDeviceExtensionCount, (char**)requiredDeviceExtensions, physicalDeviceExtensionCount, physicalDeviceExtension))
        {
            SDL_free(physicalDeviceExtension);
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
                uint64_t memoryTemp = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[i]);
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
                uint64_t memoryTemp = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[i]);
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

		uint32_t apiVersion = physicalDeviceProperties[i].apiVersion;
		logMessage("apiVersion: %u.%u.%u", VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion), VK_API_VERSION_PATCH(apiVersion));

		uint32_t driverVersion = physicalDeviceProperties[i].driverVersion;
		logMessage("driverVersion: %u.%u", (driverVersion >> 14) & 0x3FF, driverVersion & 0x3FFF);

		logMessage("vendor Id: %x", physicalDeviceProperties[i].vendorID);

		logMessage("device Id: %x", physicalDeviceProperties[i].deviceID);

		logMessage("device type: %u", physicalDeviceProperties[i].deviceType);

		logMessage(physicalDeviceProperties[i].deviceName);
	
		logMessage("max uniform buffers: %u, max storage buffers: %u", physicalDeviceProperties[i].limits.maxDescriptorSetUniformBuffers, physicalDeviceProperties[i].limits.maxDescriptorSetStorageBuffers);

		logMessage("max color attachments: %u", physicalDeviceProperties[i].limits.maxColorAttachments);

		char pipelineCacheUUID[VK_UUID_SIZE * 2];
		for (unsigned j = 0;j < VK_UUID_SIZE;j++)
		{
			SDL_snprintf(pipelineCacheUUID + 2 * j, 255, "%x", physicalDeviceProperties[i].pipelineCacheUUID[j]);
		}
		logMessage(pipelineCacheUUID);

        SDL_free(physicalDeviceExtension);
	}

	SDL_free(physicalDeviceMemoryProperties);
	SDL_free(physicalDeviceProperties);

	return index;
}
static bool * extensionSupportedCheck_Optional(uint32_t neededExtensionCount, char ** neededExtensions, uint32_t extensionCount, VkExtensionProperties * pExtensionProperties)
{
    uint32_t count = -1;//u32 overflow
    bool * group = (bool*)SDL_malloc(sizeof(bool) * neededExtensionCount);
    for (uint32_t i = 0;i < neededExtensionCount;i++)
    {
        group[i] = false;
        for (uint32_t q = 0;q < extensionCount;q++)
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
static uint32_t configureQueueCreateInfo(VkDeviceQueueCreateInfo * pCreateInfo, QueueFamily * indices, float * pQueuePriority)
{
    uint32_t queueFamilyCount = 0;
    if ((indices[1].familyIndice != indices[0].familyIndice) || (indices[2].familyIndice != indices[1].familyIndice))
    {
        if (indices[1].familyIndice == indices[0].familyIndice)
        {
            for (int i = 1;i < 3;i++)
            {
                pCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                pCreateInfo[i].pNext = NULL;
                pCreateInfo[i].flags = 0;
                pCreateInfo[i].queueFamilyIndex = indices[i].familyIndice;
                pCreateInfo[i].queueCount = indices[i].queueCount;
                pCreateInfo[i].pQueuePriorities = pQueuePriority;

                queueFamilyCount++;
            }
        }
        else if (indices[2].familyIndice == indices[1].familyIndice)
        {
            for (int i = 0;i < 2;i++)
            {
                pCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                pCreateInfo[i].pNext = NULL;
                pCreateInfo[i].flags = 0;
                pCreateInfo[i].queueFamilyIndex = indices[i].familyIndice;
                pCreateInfo[i].queueCount = indices[i].queueCount;
                pCreateInfo[i].pQueuePriorities = pQueuePriority;

                queueFamilyCount++;
            }
        }
        else if (indices[2].familyIndice == indices[0].familyIndice)
        {
            for (int i = 0;i < 3;i += 2)
            {
                pCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                pCreateInfo[i].pNext = NULL;
                pCreateInfo[i].flags = 0;
                pCreateInfo[i].queueFamilyIndex = indices[i].familyIndice;
                pCreateInfo[i].queueCount = indices[i].queueCount;
                pCreateInfo[i].pQueuePriorities = pQueuePriority;

                queueFamilyCount++;
            }
        }
        else
        {
            for (int i = 0;i < 3;i++)
            {
                pCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                pCreateInfo[i].pNext = NULL;
                pCreateInfo[i].flags = 0;
                pCreateInfo[i].queueFamilyIndex = indices[i].familyIndice;
                pCreateInfo[i].queueCount = indices[i].queueCount;
                pCreateInfo[i].pQueuePriorities = pQueuePriority;

                queueFamilyCount++;
            }
        }
    }

    if (queueFamilyCount == 0)
    {
        pCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        pCreateInfo[0].pNext = NULL;
        pCreateInfo[0].flags = 0;
        pCreateInfo[0].queueFamilyIndex = indices[0].familyIndice;
        pCreateInfo[0].queueCount = indices[0].queueCount;
        pCreateInfo[0].pQueuePriorities = pQueuePriority;

        queueFamilyCount++;
    }

    return queueFamilyCount;
}
void createLogicalDevice(VkPhysicalDevice * pPhysicalDevice, QueueFamilyIndices indice, VkDevice * pDevice)
{
    FuncCode code = createLogicalDeviceF;

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(*pPhysicalDevice, &supportedFeatures);

    if (!supportedFeatures.samplerAnisotropy)
    {
        cleanup(code);
    }
    
    const uint32_t requiredDeviceExtensionCount = 1;
    const char * requiredDeviceExtensions[1] = {
        "VK_KHR_swapchain",
    };
    uint32_t optionalDeviceExtensionCount = 9;
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

    uint32_t physicalDeviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(*pPhysicalDevice, NULL, &physicalDeviceExtensionCount, NULL);
    VkExtensionProperties * physicalDeviceExtension = (VkExtensionProperties*)SDL_malloc(physicalDeviceExtensionCount * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(*pPhysicalDevice, NULL, &physicalDeviceExtensionCount, physicalDeviceExtension);

    bool * enabledGroup = extensionSupportedCheck_Optional(optionalDeviceExtensionCount, (char **)vmaExtension, physicalDeviceExtensionCount, physicalDeviceExtension);
    SDL_free(physicalDeviceExtension);
    char ** enabledExtension = (char**)SDL_malloc(sizeof(char**) * (optionalDeviceExtensionCount + requiredDeviceExtensionCount));

    uint32_t enabledExtensionCount = 0;
    enabledExtension[0] = (char *)requiredDeviceExtensions[0];
    enabledExtensionCount++;

    for (int i = 0;i < optionalDeviceExtensionCount;i++)
    {
        if (enabledGroup[i])
        {
            enabledExtension[enabledExtensionCount] = (char*)vmaExtension[i];
            enabledExtensionCount++;
            logMessage("device extension:%s supported", vmaExtension[i]);
        }
        else
        {
            logMessage("device extension:%s not supported by device", vmaExtension[i]);
        }
    }
    SDL_free(enabledGroup);

    QueueFamily indices[3] = {indice.graphicsFamily, indice.presentFamily, indice.computeFamily};

    VkDeviceQueueCreateInfo queueCreateInfo[3];

    float queuePriority = 1.0f;
    uint32_t queueFamilyCount = configureQueueCreateInfo(queueCreateInfo, indices, &queuePriority);

    uint32_t layersCount = 1;
    const char * validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pQueueCreateInfos = queueCreateInfo;
    createInfo.enabledLayerCount = layersCount;
    createInfo.ppEnabledLayerNames = validationLayers;
    createInfo.enabledExtensionCount = enabledExtensionCount;
    createInfo.ppEnabledExtensionNames = (const char* const *)enabledExtension;
    createInfo.pEnabledFeatures = &deviceFeatures;

    resultVulkan(vkCreateDevice(*pPhysicalDevice, &createInfo, NULL, pDevice), code, 0);

    SDL_free(enabledExtension);
    //printf("logical device created\n");
}