#include "SDL3/SDL_stdinc.h"

#include "G_buffer.h"

#include "vulkan/vulkan.h"

#ifndef VK_DRAW_TOOL_H
#define VK_DRAW_TOOL_H 1

#include "SDL3/SDL_begin_code.h"

extern VkResult SDLCALL beginPrimaryCommandBuffer(VkCommandBuffer commandBuffer);
extern VkResult SDLCALL beginSecondaryCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo * pInheritanceInfo);
extern void SDLCALL setViewport(VkExtent2D extent2D, VkCommandBuffer commandBuffer);
extern void SDLCALL setScissor(VkExtent2D extent2D, VkCommandBuffer commandBuffer);
extern void SDLCALL setSubmitInfo(void * pNext, Uint32 waitSeamphoreCount, const VkSemaphore * pWaitSemaphores, VkPipelineStageFlagBits * pWaitDstStageMask, Uint32 commandBufferCount\
, VkCommandBuffer * pCommadnBuffers, Uint32 singnalSemaphoreCount, const VkSemaphore * pSignalSemaphores, VkSubmitInfo * pSubmitInfo);
extern void SDLCALL drawPic(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
extern void SDLCALL drawShadow(const char * innerName, VkCommandBuffer commandBuffer);
extern void SDLCALL drawModel(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer);
extern void SDLCALL G_vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, G_Buffer ** pBuffers);
extern void SDLCALL G_vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, G_Buffer * pBuffers, VkIndexType indexType);

#include "SDL3/SDL_close_code.h"

#endif // vk_drawTool.h