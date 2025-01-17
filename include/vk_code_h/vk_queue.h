#include "vk_struct.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL findQueueFamilies(VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface, QueueFamilyIndices * pQueueFamilyIndices);
extern void SDLCALL createGraphicsQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pGraphicsQueue);
extern void SDLCALL createPresentQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pPresentQueue);
extern void SDLCALL createComputeQueue(VkDevice * pDevice, uint32_t indice, VkQueue * pComputeQueue);

#include "SDL3/SDL_close_code.h"

#endif //vk_queue.h