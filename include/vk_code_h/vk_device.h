#include "core.h"

#ifndef VK_DEVICE_H
#define VK_DEVICE_H

void pickPhysicalDevice(VkInstance * pInstance, VkPhysicalDevice * pPhysicalDevice);
uint64_t getPhysicalDeviceTotalMemory(VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties);
int getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, uint32_t physicalDeviceNumber);
void createLogicalDevice(VkPhysicalDevice * pPhysicalDevice, QueueFamilyIndices indice, VkDevice * pDevice);

#endif