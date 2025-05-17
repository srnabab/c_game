#include "vulkan/vulkan.h"
#include "SDL3/SDL_stdinc.h"
#include "G_texture.h"

#ifndef VK_IMAGE_H
#define VK_IMAGE_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult SDLCALL createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage * pImage, VkDeviceMemory * pImageMem);
extern VkResult SDLCALL createImageArray(uint32_t width, uint32_t height, uint32_t arrayLayers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage * pImage, VkDeviceMemory * pImageMem);
extern VkResult SDLCALL createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pImageView);
extern VkResult SDLCALL createImageViews(VkImage * pImages, uint32_t imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView);
extern VkResult SDLCALL createImageViewsForImageArray(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, Uint32 imageViewCount, VkImageView ** ppImageView);
extern VkResult SDLCALL createImageViewArray(VkImage image, Uint32 layerCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pImageView);
extern void SDLCALL destroyImageViews(VkImageView * pImageView, uint32_t imageCount);
extern VkResult SDLCALL _transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Uint32 baseArrayLayer, Uint32 layerCount);
extern VkResult SDLCALL transitionImageLayout(VkCommandBuffer commandBuffer, G_Texture_P * pTexture, VkImageLayout newLayout, Uint32 baseArrayLayer, Uint32 layerCount);
extern bool SDLCALL getImageFlags(VkImageLayout oldLayout, VkImageLayout newLayout, VkFormat format, VkImageAspectFlags * pAspectMask, VkAccessFlags * pSrcAccessMask, VkAccessFlags * pDstAccessMask, VkPipelineStageFlags * pSourceStage, VkPipelineStageFlags * pDestinationStage);
extern bool SDLCALL hasStencilComponent(VkFormat format);

#include "SDL3/SDL_close_code.h"

#endif //vk_image.h