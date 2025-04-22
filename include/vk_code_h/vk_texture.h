#include "vulkan/vulkan.h"

#include "SDL3_image/SDL_image.h"

#include "vk_code_h/vk_struct.h"

#include "G_file/path_compare.h"

#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H 1

#include "SDL3/SDL_begin_code.h"

extern SDL_PixelFormat SDLCALL getSDL_PixelFormat(Uint8 channel);
extern VkFormat SDLCALL getVulkanFormat(Uint8 channel, FormatQualifier flags);
extern void SDLCALL createTextureImageFromFile(PathType type, VkFormat format, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem);
extern void SDLCALL createTextureImageFromMem(void * pixels, Uint32 width, Uint32 height, VkDeviceSize imageSize, VkFormat format, VkImage * pTextureImage, VkDeviceMemory * pTextureImageMem);
extern void SDLCALL createTextureImageView(VkImage * pTextureImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pTextureImageView);
extern unsigned char * SDLCALL readPNG(PathType type, uint32_t * pWidth, uint32_t * pHeight, uint8_t * pChannel);
extern VkResult SDLCALL copyBufferToImage(VkCommandBuffer commandBuffer, VkImage * pImage, Uint32 width, Uint32 height, VkBuffer * pBuffer);

#include "SDL3/SDL_close_code.h"

#endif