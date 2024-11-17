#include "core.h"

#ifndef VK_DEPTH_H
#define VK_DEPTH_H

void findSupportFormat(VkFormat * candiates, VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat);
void findDepthFormat(VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat);
void createDepthResoures(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkExtent2D * pExtent2D, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkImage * pDepthImage, VkDeviceMemory * pDepthImageMem, VkImageView * pDepthImageView);

#endif