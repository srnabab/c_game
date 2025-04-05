#include "SDL3/SDL_stdinc.h"

#include "vulkan/vulkan.h"

#ifndef VK_DRAW_TOOL_H
#define VK_DRAW_TOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult beginCommandBuffer(VkCommandBuffer commandBuffer);
extern void SDLCALL setViewport(VkExtent2D extent2D, VkCommandBuffer commandBuffer);
extern void SDLCALL setScissor(VkExtent2D extent2D, VkCommandBuffer commandBuffer);
extern void SDLCALL drawPic(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer);
extern void SDLCALL drawModel(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer);

#include "SDL3/SDL_close_code.h"

#endif // vk_drawTool.h