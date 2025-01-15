#include "G_begin_code.h"
#include "vulkan/vulkan.h"
#include "vk_struct.h"

#ifndef VK_DEVICE_H
#define VK_DEVICE_H 1

void pickPhysicalDevice(VkInstance * pInstance, VkPhysicalDevice * pPhysicalDevice);
uint64_t getPhysicalDeviceTotalMemory(VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties);
int getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, uint32_t physicalDeviceNumber);
void createLogicalDevice(VkPhysicalDevice * pPhysicalDevice, QueueFamilyIndices indice, VkDevice * pDevice);

#endif