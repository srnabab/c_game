#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_FRAMEBUFFER_H
#define VK_FRAMEBUFFER_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createFrameBuffer(uint32_t imageCount, Uint32 width, Uint32 height, Uint32 attachmentCount, VkImageView * pImageViews, VkImageView * pSwapchainImageView, VkRenderPass renderPass, VkFramebuffer ** pSwapchainFramebuffer);
extern void SDLCALL createFrameBufferByImageArray(Uint32 imageCount, Uint32 width, Uint32 height, VkImageView * pImageViews, VkRenderPass renderPass, VkFramebuffer ** pFrameBuffer);
extern void SDLCALL destroyedFrameBuffer(uint32_t imageCount, VkFramebuffer * pSwapchainFramebuffer);

#include "SDL3/SDL_close_code.h"

#endif