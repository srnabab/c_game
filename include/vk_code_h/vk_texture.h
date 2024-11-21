#include "core.h"
#include "SDL3_image/SDL_image.h"

#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

void createTextureImage(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, PathType type, VkFormat format, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem);
void createTextureImageView(VkDevice * pDevice, VkImage * pTextureImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pTextureImageView);
unsigned char * readPNG(PathType type, uint32_t * pWidth, uint32_t * pHeight, uint8_t * pChannel);
VkResult copyBufferToImage(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphcisQueue, VkImage * pImage, uint32_t width, uint32_t height, VkBuffer * pBuffer);
void createTextureSampler(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkSampler * pTextureSampler);

#endif