#include "vk_all.h"
#include "vk_buffer.h"
#include "libpng/png.h"
#include "zlib/zlib.h"

#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

void createTextureImage(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, const char * pFileName, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem);
void createTextureImageView(VkDevice * pDevice, VkImage * pTextureImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pTextureImageView);
png_bytep readPNG(const char * fileName, uint32_t * pWidth, uint32_t * pHeight, uint8_t * pChannel);
VkResult copyBufferToImage(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphcisQueue, VkImage * pImage, uint32_t width, uint32_t height, VkBuffer * pBuffer);
void createTextureSampler(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkSampler * pTextureSampler);

#endif