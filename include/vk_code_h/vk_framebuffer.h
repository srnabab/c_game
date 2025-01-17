#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_FRAMEBUFFER_H
#define VK_FRAMEBUFFER_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createFrameBuffer(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t imageCount, VkImageView * pSwapchainImageView, VkImageView * pDepthImageView, VkRenderPass * pRenderPass, VkFramebuffer ** pSwapchainFramebuffer);
extern void SDLCALL destroyedFrameBuffer(VkDevice * pDevice, uint32_t imageCount, VkFramebuffer * pSwapchainFramebuffer);

#include "SDL3/SDL_close_code.h"

#endif