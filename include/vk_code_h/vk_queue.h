#include "vk_all.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H

void findQueueFamilies(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, QueueFamilyIndices * pQueueFamilyIndices);
void createLogicalDevice(VkPhysicalDevice * pPhysicalDevice, QueueFamilyIndices indice, VkDevice * pVkDevice);
void createGraphicsQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pGraphicsQueue);
void createPresentQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pPresentQueue);
void createComputeQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pComputeQueue);
bool isDeviceSuitable(VkPhysicalDevice * pPhysicalDevice);
bool checkDeviceExtensionSupport(VkPhysicalDevice * pDevice);

#endif //vk_queue.h