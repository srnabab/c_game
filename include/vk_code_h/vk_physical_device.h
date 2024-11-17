#include "core.h"

#ifndef VK_PHYSICAL_DEVICE_H
#define VK_PHYSICAL_DEVICE_H

void pickPhysicalDevice(VkInstance * pInstance, VkPhysicalDevice * pPhysicalDevice);
uint64_t getPhysicalDeviceTotalMemory(VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties);
uint32_t getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, uint32_t physicalDeviceNumber);

#endif //vk_physical_device.h