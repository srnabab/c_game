#include "vk_struct.h"

#ifndef VK_DEVICE_H
#define VK_DEVICE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL pickPhysicalDevice(VkInstance * pInstance, VkPhysicalDevice * pPhysicalDevice);
extern Uint64 SDLCALL getPhysicalDeviceTotalMemory(VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties);
extern int SDLCALL getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, Uint32 physicalDeviceNumber);
extern void SDLCALL createLogicalDevice(VkPhysicalDevice * pPhysicalDevice, QueueFamilyIndices indice, VkDevice * pDevice);

#include "SDL3/SDL_close_code.h"

#endif