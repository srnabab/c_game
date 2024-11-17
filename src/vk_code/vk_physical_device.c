#include "vk_physical_device.h"

void pickPhysicalDevice(VkInstance * pInstance, VkPhysicalDevice * pPhysicalDevice)
{
	FuncCode code = pickPhysicalDeviceF;
    uint32_t deviceCount = 0;
    resultVulkan(vkEnumeratePhysicalDevices(*pInstance, &deviceCount, VK_NULL_HANDLE), code, 0);

    if (deviceCount == 0)
    {
        fprintf(stderr, "failed to find GPUs with Vulkan support\n");
    }

    VkPhysicalDevice * devices = (VkPhysicalDevice *)malloc(deviceCount * sizeof(VkPhysicalDevice));
    resultVulkan(vkEnumeratePhysicalDevices(*pInstance, &deviceCount, devices), code, 1, devices);

	VkPhysicalDevice device = devices[getBestPhysicalDeviceIndex(devices, deviceCount)];
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	if (deviceFeatures.geometryShader)
	{
		free(devices);
		//printf("devive picked\n");
		*pPhysicalDevice = device;
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
	logMessage("device memory: %llu\n", physicalDeviceTotalMemory);
	return physicalDeviceTotalMemory;
}
uint32_t getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, uint32_t physicalDeviceNumber)
{
	VkPhysicalDeviceProperties *physicalDeviceProperties = (VkPhysicalDeviceProperties *)malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceProperties));
	VkPhysicalDeviceMemoryProperties *physicalDeviceMemoryProperties = (VkPhysicalDeviceMemoryProperties *)malloc(physicalDeviceNumber * sizeof(VkPhysicalDeviceMemoryProperties));

	uint32_t discreteGPUNumber = 0, integratedGPUNumber = 0;
	uint32_t *discreteGPUIndices = (uint32_t *)malloc(physicalDeviceNumber * sizeof(uint32_t));
	uint32_t *integratedGPUIndices = (uint32_t *)malloc(physicalDeviceNumber * sizeof(uint32_t));

	for(uint32_t i = 0; i < physicalDeviceNumber; i++)
	{
		vkGetPhysicalDeviceProperties(pPhysicalDevices[i], &physicalDeviceProperties[i]);
		
		vkGetPhysicalDeviceMemoryProperties(pPhysicalDevices[i], &physicalDeviceMemoryProperties[i]);

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
			sprintf(pipelineCacheUUID + 2 * j, "%x", physicalDeviceProperties[i].pipelineCacheUUID[j]);
		}
		logMessage(pipelineCacheUUID);

		if(physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			discreteGPUIndices[discreteGPUNumber] = i;
			discreteGPUNumber++;
		}
		if(physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			integratedGPUIndices[integratedGPUNumber] = i;
			integratedGPUNumber++;
		}
	}

	uint32_t bestPhysicalDeviceIndex = 0;
	VkDeviceSize bestPhysicalDeviceMemory = 0;

	if(discreteGPUNumber != 0)
	{
		for(uint32_t i = 0; i < discreteGPUNumber; i++)
		{
			if(bestPhysicalDeviceMemory < getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[discreteGPUIndices[i]]))
			{
				bestPhysicalDeviceMemory = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[discreteGPUIndices[i]]);
				bestPhysicalDeviceIndex = discreteGPUIndices[i];
			}
		}
	}
	else if(integratedGPUNumber != 0)
	{
		for(uint32_t i = 0; i < integratedGPUNumber; i++)
		{
			if(bestPhysicalDeviceMemory < getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[integratedGPUIndices[i]]))
			{
				bestPhysicalDeviceMemory = getPhysicalDeviceTotalMemory(&physicalDeviceMemoryProperties[integratedGPUIndices[i]]);
				bestPhysicalDeviceIndex = integratedGPUIndices[i];
			}
		}
	}

	free(discreteGPUIndices);
	free(integratedGPUIndices);
	free(physicalDeviceMemoryProperties);
	free(physicalDeviceProperties);

	return bestPhysicalDeviceIndex;
}