#include "G_begin_code.h"
#include "vulkan/vulkan.h"
#include "vk_struct.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H 1

void findQueueFamilies(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, QueueFamilyIndices * pQueueFamilyIndices);
void createGraphicsQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pGraphicsQueue);
void createPresentQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pPresentQueue);
void createComputeQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pComputeQueue);

#endif //vk_queue.h