#include "vk_all.h"

#ifndef VK_FRAMEBUFFER_H
#define VK_FRAMEBUFFER_H

void createFrameBuffer(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t imageCount, VkImageView * pSwapchainImageView, VkImageView * pDepthImageView, VkRenderPass * pRenderPass, VkFramebuffer ** pSwapchainFramebuffer);
void destroyedFrameBuffer(VkDevice * pDevice, uint32_t imageCount, VkFramebuffer * pSwapchainFramebuffer);

#endif