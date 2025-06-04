#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_staticModel.h"
#include "G_struct.h"

extern G_SYNC allSync;
extern VK_ALL allInOne;

static VkResult _beginCommandBuffer(VkCommandBuffer commandBuffer, void * pNext, VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo * pInheritanceInfo)
{
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = pNext;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = pInheritanceInfo;

    return vkBeginCommandBuffer(commandBuffer, &beginInfo);
}
VkResult beginPrimaryCommandBuffer(VkCommandBuffer commandBuffer)
{
    return _beginCommandBuffer(commandBuffer, NULL, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, NULL);
}
VkResult beginSecondaryCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo * pInheritanceInfo)
{
    return _beginCommandBuffer(commandBuffer, NULL, flags, pInheritanceInfo);
}
void setViewport(VkExtent2D extent2D, VkCommandBuffer commandBuffer)
{
    VkViewport viewport = {0};
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
    VkRect2D scissor = {0};
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
    if (tempTexture == NULL || !tempTexture->draw)
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
void drawShadow(const char * innerName, VkCommandBuffer commandBuffer)
{
    Uint32 firstInstance, instanceCount;

    SDL_LockMutex(allSync.renderMutex);

    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL || !tempTexture->draw)
    {
        SDL_UnlockMutex(allSync.renderMutex);
        return;
    }

    getStaticModelDrawInfo(allInOne.pStaticModelPool, &firstInstance, &instanceCount, innerName);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed(commandBuffer, tempTexture->offsets[i].count, instanceCount, tempTexture->offsets[i].offset, tempTexture->offsets[i].offset, firstInstance);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
void drawModel(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer)
{
    Uint32 firstInstance, instanceCount;

    SDL_LockMutex(allSync.renderMutex);

    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL || !tempTexture->draw)
    {
        SDL_UnlockMutex(allSync.renderMutex);
        return;
    }

    getStaticModelDrawInfo(allInOne.pStaticModelPool, &firstInstance, &instanceCount, innerName);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.modelPipelineLayout, 0, 1, tempTexture->pDescriptorSet + currentFrame, 0, NULL);
   
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed(commandBuffer, tempTexture->offsets[i].count, instanceCount, tempTexture->offsets[i].offset, tempTexture->offsets[i].offset, firstInstance);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
void G_vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, G_Buffer ** pBuffers)
{
    Uint32 i;
    VkBuffer buffers[10];
    VkDeviceSize offsets[10];

    for (i = 0;i < bindingCount;i++)
    {
        buffers[i] = pBuffers[i]->pBufferPool->buffer;
        offsets[i] = pBuffers[i]->startOffset;
    }

    vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, buffers, offsets);
}
void G_vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, G_Buffer * pBuffers, VkIndexType indexType)
{
    vkCmdBindIndexBuffer(commandBuffer, pBuffers->pBufferPool->buffer, pBuffers->startOffset, indexType);
}