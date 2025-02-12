#include "vulkan/vulkan.h"
#include "SDL3_image/SDL_image.h"
#include "G_file/path_compare.h"

#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createTextureImage(VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, PathType type, VkFormat format, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem);
extern void SDLCALL createTextureImageView(VkImage * pTextureImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pTextureImageView);
extern unsigned char * SDLCALL readPNG(PathType type, uint32_t * pWidth, uint32_t * pHeight, uint8_t * pChannel);
extern VkResult SDLCALL copyBufferToImage(VkCommandPool * pCommandPool, VkQueue * pGraphcisQueue, VkImage * pImage, uint32_t width, uint32_t height, VkBuffer * pBuffer);
extern void SDLCALL createTextureSampler(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkSampler * pTextureSampler);

#include "SDL3/SDL_close_code.h"

#endif