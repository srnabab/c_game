#include "vk_code_h/vk_offscreenImage.h"
#include "vk_code_h/vk_drawTool.h"

VkResult drawImageView(Uint32 width, Uint32 height, VkImageView imageView, VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer)
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

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombine2DPipeline);

    // drawPic(innerName, )

    vkCmdDraw(commandBuffer, 6, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    
    return result;
}