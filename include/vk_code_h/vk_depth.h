#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_DEPTH_H
#define VK_DEPTH_H 1

void findSupportFormat(VkFormat * candiates, VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat);
void findDepthFormat(VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat);
void createDepthResoures(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkExtent2D * pExtent2D, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkImage * pDepthImage, VkDeviceMemory * pDepthImageMem, VkImageView * pDepthImageView);

#endif