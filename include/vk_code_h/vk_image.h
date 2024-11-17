#include "core.h"

#ifndef VK_IMAGE_H
#define VK_IMAGE_H

VkResult createImage(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage * pImage, VkDeviceMemory * pImageMem);
VkResult createImageView(VkDevice * pDevice, VkImage * pImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pImageView);
VkResult createImageViews(VkDevice * pDevice, VkImage ** ppImages, uint32_t imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView);
void destroyImageViews(VkDevice * pDevice, VkImageView * pImageView, uint32_t imageCount);
VkResult transitionImageLayout(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphcisQueue, VkImage * pImage, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
bool hasStencilComponent(VkFormat format);

#endif //vk_image.h