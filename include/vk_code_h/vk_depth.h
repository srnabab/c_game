#include "vulkan/vulkan.h"

#ifndef VK_DEPTH_H
#define VK_DEPTH_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL findSupportFormat(VkFormat * candiates, VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat);
extern void SDLCALL findDepthFormat(VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat);
extern void SDLCALL createDepthResoures(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkExtent2D * pExtent2D, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkImage * pDepthImage, VkDeviceMemory * pDepthImageMem, VkImageView * pDepthImageView);

#include "SDL3/SDL_close_code.h"

#endif