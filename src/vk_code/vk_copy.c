#include "vk_code_h/vk_copy.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_commandPool.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_queue.h"

extern VK_ALL allInOne;

bool addBufferCopy(G_Buffer * srcBuffer, VkDeviceSize srcOffset, G_Buffer * dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, int32_t queueFamilyindex, Uint32 currentFrame)
{
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    Uint32 index = allInOne.bufferCopyCount[currentFrame];

    allInOne.bufferCopy[currentFrame][index].used = 1;
    allInOne.bufferCopy[currentFrame][index].srcBuffer = srcBuffer->pBufferPool->buffer;
    allInOne.bufferCopy[currentFrame][index].dstBuffer = dstBuffer->pBufferPool->buffer;
    _setBufferCopyRegion(srcOffset + srcBuffer->startOffset, dstOffset + dstBuffer->startOffset, size, &allInOne.bufferCopy[currentFrame][index].regions);
    allInOne.bufferCopy[currentFrame][index].queueFamilyindex = queueFamilyindex;

    allInOne.bufferCopyCount[currentFrame]++;

    return true;
}
Uint8 recordBufferCopy(Uint32 currentFrame)
{
#warning error processing needed
    Uint32 i;
    Uint32 totalCount = allInOne.bufferCopyCount[currentFrame];
    Uint32 usedCount = allInOne.bufferCopyCount[currentFrame];
    Uint32 flag = 0;
    if (totalCount == 0) return flag;

    VkCommandBuffer graphicCommandBuffer = allInOne.pGraphicCopyCommandBuffer[currentFrame];
    VkCommandBuffer transferCommandBuffer = allInOne.pTransferCopyCommandBuffer[currentFrame];
    VkCommandBuffer computeCommandBuffer = allInOne.pComputeCopyCommandBuffer[currentFrame];

    VkCommandBufferInheritanceInfo inheritanceInfo = {0};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.pNext = NULL;
    inheritanceInfo.renderPass = VK_NULL_HANDLE;
    inheritanceInfo.subpass = 0;
    inheritanceInfo.framebuffer = VK_NULL_HANDLE;
    inheritanceInfo.occlusionQueryEnable = VK_FALSE;
    inheritanceInfo.queryFlags = 0;
    inheritanceInfo.pipelineStatistics = 0;

    vkResetCommandBuffer(graphicCommandBuffer, 0);
    beginSecondaryCommandBuffer(graphicCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, &inheritanceInfo);
    for (i = 0;i < totalCount;i++)
    {
        if (allInOne.bufferCopy[currentFrame][i].queueFamilyindex == allInOne.queueFamilyIndices.graphicsFamily.familyIndice)
        {
            // VkBufferMemoryBarrier Barrier[2];

            // _setBufferMemoryBarrier(NULL, VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, allInOne.bufferCopy[currentFrame][i].srcBuffer, allInOne.bufferCopy[currentFrame][i].regions.srcOffset\
            //     , allInOne.bufferCopy[currentFrame][i].regions.size, &Barrier[0]);
            // _setBufferMemoryBarrier(NULL, allInOne.bufferCopy[currentFrame][i].dstBufferSrcAccessMask, VK_ACCESS_TRANSFER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, allInOne.bufferCopy[currentFrame][i].dstBuffer\
            //     , allInOne.bufferCopy[currentFrame][i].regions.dstOffset, allInOne.bufferCopy[currentFrame][i].regions.size, &Barrier[1]);
            // vkCmdPipelineBarrier(graphicCommandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 1, &Barrier[0], 0, NULL);
            // vkCmdPipelineBarrier(graphicCommandBuffer, allInOne.bufferCopy[currentFrame][i].dstBufferSrcStageMask, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 1, &Barrier[1], 0, NULL);

            vkCmdCopyBuffer(graphicCommandBuffer, allInOne.bufferCopy[currentFrame][i].srcBuffer, allInOne.bufferCopy[currentFrame][i].dstBuffer, 1, &allInOne.bufferCopy[currentFrame][i].regions);

            // _setBufferMemoryBarrier(NULL, VK_ACCESS_TRANSFER_WRITE_BIT, allInOne.bufferCopy[currentFrame][i].dstBufferSrcAccessMask, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, allInOne.bufferCopy[currentFrame][i].dstBuffer\
            //     , allInOne.bufferCopy[currentFrame][i].regions.dstOffset, allInOne.bufferCopy[currentFrame][i].regions.size, &Barrier[1]);
            // vkCmdPipelineBarrier(graphicCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, allInOne.bufferCopy[currentFrame][i].dstBufferSrcStageMask, 0, 0, NULL, 1, &Barrier[1], 0, NULL);

            allInOne.bufferCopy[currentFrame][i].used = 0;
            usedCount--;
        }
    }
    vkEndCommandBuffer(graphicCommandBuffer);

    if (i) flag |= 1;
    if (usedCount == 0) 
    {
        allInOne.bufferCopyCount[currentFrame] = 0;
        return flag;
    }

    vkResetCommandBuffer(computeCommandBuffer, 0);
    beginSecondaryCommandBuffer(computeCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, &inheritanceInfo);
    for (i = 0;i < totalCount;i++)
    {
        if (allInOne.bufferCopy[currentFrame][i].queueFamilyindex == allInOne.queueFamilyIndices.computeFamily.familyIndice)
        {
            vkCmdCopyBuffer(computeCommandBuffer, allInOne.bufferCopy[currentFrame][i].srcBuffer, allInOne.bufferCopy[currentFrame][i].dstBuffer, 1, &allInOne.bufferCopy[currentFrame][i].regions);
            allInOne.bufferCopy[currentFrame][i].used = 0;
        }
    }
    vkEndCommandBuffer(computeCommandBuffer);

    if (i) flag |= 2;
    if (usedCount == 0) 
    {
        allInOne.bufferCopyCount[currentFrame] = 0;
        return flag;
    }

    vkResetCommandBuffer(transferCommandBuffer, 0);
    beginSecondaryCommandBuffer(transferCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, &inheritanceInfo);
    for (i = 0;i < totalCount;i++)
    {
        if (allInOne.bufferCopy[currentFrame][i].queueFamilyindex == allInOne.queueFamilyIndices.transferFamily.familyIndice)
        {
            vkCmdCopyBuffer(transferCommandBuffer, allInOne.bufferCopy[currentFrame][i].srcBuffer, allInOne.bufferCopy[currentFrame][i].dstBuffer, 1, &allInOne.bufferCopy[currentFrame][i].regions);
            allInOne.bufferCopy[currentFrame][i].used = 0;
        }
    }
    vkEndCommandBuffer(transferCommandBuffer);

    if (i) flag |= 4;

    allInOne.bufferCopyCount[currentFrame] = 0;

    return flag;
}
