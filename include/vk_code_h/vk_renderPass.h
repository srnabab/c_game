#include "vulkan/vulkan.h"

#ifndef VK_RENDER_PASS_H
#define VK_RENDER_PASS_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createGraphicRenderPass(VkFormat surfaceFormat, VkFormat depthFormat, VkRenderPass * pRenderPass);
extern void SDLCALL createModelRenderPass(VkFormat colorFormat, VkFormat normalFormat, VkFormat shadowFormat, VkFormat depthFormat, VkRenderPass * pRenderPass);
extern void SDLCALL createShadowRenderPass(VkFormat depthFormat, VkRenderPass * pRenderPass);
extern void SDLCALL createCombineRenderPass(VkFormat colorFormat, VkRenderPass * pRenderPass);

#include "SDL3/SDL_close_code.h"

#endif //vk_renderPass.h