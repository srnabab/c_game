#include "vk_code_h/vk_offscreenImage.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

VkImageView drawSingleImageView(Uint32 width, Uint32 height, const char * innerName, VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkPipeline pipeline, VkPipelineLayout pipelineLayout, Uint32 currentFrame)
{
    Uint32 frameBufferIndex = getFreeFramebufferIndex();
    VkFramebuffer frameBuffer = getFrameBuffer(frameBufferIndex);

    drawImageView(width, height, innerName, commandBuffer, renderPass, framebuffer, pipeline, pipelineLayout);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    setSubmitInfo(NULL, 0, NULL, NULL, 1, &commandBuffer, 1, (*allInOne.ppTimelineSemaphore1) + currentFrame, &submitInfo);
    vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfo, NULL);

    return getFrameBufferImageView(frameBufferIndex);
}
VkResult drawImageView(Uint32 width, Uint32 height, const char * innerName, VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkPipeline pipeline, VkPipelineLayout pipelineLayout)
{
    VkResult result = VK_SUCCESS;
    result = beginCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS) return result;

    setViewport((VkExtent2D){width, height}, commandBuffer);
    setScissor((VkExtent2D){width, height}, commandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, (VkExtent2D){width, height}};

    VkClearValue clearValue[1];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = renderPass;
    renderBeginInfo.framebuffer = framebuffer;
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    drawPic(innerName, 0, commandBuffer, pipelineLayout);

    vkCmdEndRenderPass(commandBuffer);
    
    return result;
}