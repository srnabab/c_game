#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_staticModel.h"
#include "G_struct.h"

extern G_SYNC allSync;
extern VK_ALL allInOne;

VkResult beginCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    return vkBeginCommandBuffer(commandBuffer, &beginInfo);
}
void setViewport(VkExtent2D extent2D, VkCommandBuffer commandBuffer)
{
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent2D.width;
    viewport.height = (float)extent2D.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}
void setScissor(VkExtent2D extent2D, VkCommandBuffer commandBuffer)
{
    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = extent2D;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void setSubmitInfo(void * pNext, Uint32 waitSeamphoreCount, const VkSemaphore * pWaitSemaphores, VkPipelineStageFlagBits * pWaitDstStageMask, Uint32 commandBufferCount\
, VkCommandBuffer * pCommadnBuffers, Uint32 singnalSemaphoreCount, const VkSemaphore * pSignalSemaphores, VkSubmitInfo * pSubmitInfo)
{
    pSubmitInfo->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    pSubmitInfo->pNext = pNext;
    pSubmitInfo->waitSemaphoreCount = waitSeamphoreCount;
    pSubmitInfo->pWaitSemaphores = pWaitSemaphores;
    pSubmitInfo->pWaitDstStageMask = pWaitDstStageMask;
    pSubmitInfo->commandBufferCount = commandBufferCount;
    pSubmitInfo->pCommandBuffers = pCommadnBuffers;
    pSubmitInfo->signalSemaphoreCount = singnalSemaphoreCount;
    pSubmitInfo->pSignalSemaphores = pSignalSemaphores;
}
void drawPic(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    SDL_LockMutex(allSync.renderMutex);
 
    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL)
    {
        SDL_UnlockMutex(allSync.renderMutex);
        return;
    }

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
    1, tempTexture->pDescriptorSet + currentFrame, 0, NULL);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed(commandBuffer, tempTexture->offsets[i].count * 6, 1, 0, tempTexture->offsets[i].offset, 0);
        // vkCmdDrawIndexed(CommandBuffer, 6, instanceCount, 0, offset, 0);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
void drawModel(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer, bool bottom)
{
    Uint32 firstInstance, instanceCount;
    getStaticModelDrawInfo(allInOne.pStaticModelPool, &firstInstance, &instanceCount, innerName);

    SDL_LockMutex(allSync.renderMutex);

    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL)
    {
        SDL_UnlockMutex(allSync.renderMutex);
        return;
    }

    if (bottom)
    {
        VkDescriptorSet descriptorSet[] = {tempTexture->pDescriptorSet[currentFrame], tempTexture->pDescriptorSet[currentFrame + 2]};
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.bottomPipelineLayout, 0,
        2, descriptorSet, 0, NULL);
    }
    else
    {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.modelPipelineLayout, 0,
        1, tempTexture->pDescriptorSet + currentFrame, 0, NULL);
    }
   
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed(commandBuffer, tempTexture->offsets[i].count, instanceCount, 0, tempTexture->offsets[i].offset, firstInstance);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}