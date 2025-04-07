#include "SDL3/SDL_stdinc.h"

#include "vulkan/vulkan.h"

#ifndef VK_DRAW_TOOL_H
#define VK_DRAW_TOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult beginCommandBuffer(VkCommandBuffer commandBuffer);
extern void SDLCALL setViewport(VkExtent2D extent2D, VkCommandBuffer commandBuffer);
extern void SDLCALL setScissor(VkExtent2D extent2D, VkCommandBuffer commandBuffer);
extern void SDLCALL setSubmitInfo(void * pNext, Uint32 waitSeamphoreCount, const VkSemaphore * pWaitSemaphores, VkPipelineStageFlagBits * pWaitDstStageMask, Uint32 commandBufferCount\
, VkCommandBuffer * pCommadnBuffers, Uint32 singnalSemaphoreCount, const VkSemaphore * pSignalSemaphores, VkSubmitInfo * pSubmitInfo);
extern void SDLCALL drawPic(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
extern void SDLCALL drawModel(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer, bool bottom);

#include "SDL3/SDL_close_code.h"

#endif // vk_drawTool.h