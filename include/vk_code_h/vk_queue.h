#include "core.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H

void findQueueFamilies(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, QueueFamilyIndices * pQueueFamilyIndices);
void createGraphicsQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pGraphicsQueue);
void createPresentQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pPresentQueue);
void createComputeQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pComputeQueue);

#endif //vk_queue.h