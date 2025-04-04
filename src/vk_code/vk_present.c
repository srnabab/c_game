#include "G_constants.h"

#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_all_struct.h"

#include "SDL3/SDL_timer.h"

#include "G_log.h"
#include "G_struct.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;

static VkResult beginCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    return vkBeginCommandBuffer(commandBuffer, &beginInfo);
}
static void setViewport(VkExtent2D extent2D, VkCommandBuffer commandBuffer)
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
static void setScissor(VkExtent2D extent2D, VkCommandBuffer commandBuffer)
{
    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = extent2D;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
static void drawPic(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer)
{
    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL) return;

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipelineLayout, 0,
    1, tempTexture->pDescriptorSet + currentFrame, 0, NULL);

    SDL_LockMutex(allSync.renderMutex);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed(commandBuffer, tempTexture->offsets[i].count * 6, 1, 0, tempTexture->offsets[i].offset, 0);
        // vkCmdDrawIndexed(CommandBuffer, 6, instanceCount, 0, offset, 0);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
static void drawModel(const char * innerName, Uint32 currentFrame, VkCommandBuffer commandBuffer)
{
    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL) return;

    Uint32 firstInstance, instanceCount;
    getStaticModelDrawInfo(allInOne.pStaticModelPool, &firstInstance, &instanceCount, innerName);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pModelPipelineLayout, 0,
    1, tempTexture->pDescriptorSet + currentFrame, 0, NULL);

    SDL_LockMutex(allSync.renderMutex);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed(commandBuffer, tempTexture->offsets[i].count, instanceCount, 0, tempTexture->offsets[i].offset, firstInstance);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
static void drawShadow(const char * innerName, VkCommandBuffer commandBuffer)
{
    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL) return;

    Uint32 firstInstance, instanceCount;
    getStaticModelDrawInfo(allInOne.pStaticModelPool, &firstInstance, &instanceCount, innerName);

    SDL_LockMutex(allSync.renderMutex);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed(commandBuffer, tempTexture->offsets[i].count, instanceCount, 0, tempTexture->offsets[i].offset, firstInstance);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
static void recordCommandBuffer2D()
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkCommandBuffer currentCommandBuffer = (*allInOne.ppGraphicCommandBuffer)[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

    setViewport(*(allInOne.pExtent2D), currentCommandBuffer);

    setScissor(*(allInOne.pExtent2D), currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, *allInOne.pExtent2D};

    VkClearValue clearValue[2];
    clearValue[0].color= (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = *allInOne.pRenderPass;
    renderBeginInfo.framebuffer = (*allInOne.ppGraphic2dFramebuffer)[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 2;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipeline);

    vkCmdPushConstants(currentCommandBuffer, *allInOne.pGraphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);

    VkBuffer vertexBuffer[] = {(*allInOne.pVertexBuffer2D)[currentFrame]};
    // VkDeviceSize vertexOffsets1[] = {0, allInOne.maxVerticesCount * sizeof(vec3), allInOne.maxVerticesCount * sizeof(vec3) + allInOne.maxVerticesCount * sizeof(vec3)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffer, offsets);

    vkCmdBindIndexBuffer(currentCommandBuffer, (*allInOne.pIndexBuffer2D)[0], 0, VK_INDEX_TYPE_UINT16);

    // tile map
    drawPic(TEXTURE_TILE_SET, currentFrame, currentCommandBuffer);

    //loading1 png
    drawPic(TEXTURE_LOADING, currentFrame, currentCommandBuffer);

    //circle
    drawPic(TEXTURE_CIRCLE, currentFrame, currentCommandBuffer);

    // box
    drawPic(TEXTURE_BOX, currentFrame, currentCommandBuffer);

    // main font png
    drawPic(TEXTURE_FONT, currentFrame, currentCommandBuffer);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipeline);

    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, &(*allInOne.ppShaderStorageBuffers)[currentFrame], offsets);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipelineLayout, 0,
    1, &(*allInOne.ppParticleDescriptorSets)[currentFrame], 0, NULL);

    vkCmdDraw(currentCommandBuffer, PARTICLE_COUNT, 1, 0, 0);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordCommandBufferCombine2D(Uint32 imageIndex)
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkCommandBuffer currentCommandBuffer = (*allInOne.ppGraphicCommandBuffer)[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

    setViewport(*(allInOne.pExtent2D), currentCommandBuffer);
    setScissor(*(allInOne.pExtent2D), currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, *allInOne.pExtent2D};

    VkClearValue clearValue[1];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = *allInOne.pCombine2DRenderPass;
    renderBeginInfo.framebuffer = (*allInOne.ppSwapchain2DFramebuffer)[imageIndex];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombine2DPipeline);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombine2DPipelineLayout, 0, 1, (*allInOne.ppCombine2dDescriptorSets) + currentFrame, 0, NULL);

    vkCmdDraw(currentCommandBuffer, 6, 1, 0, 0);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordCommandBufferShadow(void)
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkCommandBuffer currentCommandBuffer = (*allInOne.ppGraphicCommandBuffer)[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

    VkExtent2D shadow = {SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT};
    setViewport(shadow, currentCommandBuffer);
    setScissor(shadow, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, shadow};

    VkClearValue clearValue[3];
    clearValue[0].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = *allInOne.pShadowRenderPass;
    renderBeginInfo.framebuffer = (*allInOne.ppShadowFramebuffer)[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pShadowPipeline);

    VkBuffer vertex3DBuffer[] = {(*allInOne.pVertexBuffer3D)[currentFrame], allInOne.pStaticModelPool->instanceBuffer[0], allInOne.pStaticModelPool->instanceBuffer[0]};
    VkDeviceSize offsets[] = {0, 0, allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 3, vertex3DBuffer, offsets);

    vkCmdBindIndexBuffer(currentCommandBuffer, (*allInOne.pIndexBuffer3D)[currentFrame], 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pShadowPipelineLayout, 0,
    1, (*allInOne.ppShadowDescriptorSets) + currentFrame, 0, NULL);

    // model
    drawShadow(TEXTURE_BOTTOM, currentCommandBuffer);

    drawShadow(TEXTURE_MODEL, currentCommandBuffer);

    vkCmdEndRenderPass(currentCommandBuffer);
}

static void recordCommandBuffer_3D(void)
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkCommandBuffer currentCommandBuffer = (*allInOne.ppGraphicCommandBuffer)[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(*(allInOne.pExtent2D), currentCommandBuffer);
    setScissor(*(allInOne.pExtent2D), currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, *allInOne.pExtent2D};

    VkClearValue clearValue[4];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[1].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[2].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[3].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = *allInOne.pModelRenderPass;
    renderBeginInfo.framebuffer = (*allInOne.ppDirectColorFramebuffer)[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 4;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pModelPipeline);

    VkBuffer vertex3DBuffer[] = {(*allInOne.pVertexBuffer3D)[currentFrame], allInOne.pStaticModelPool->instanceBuffer[0], allInOne.pStaticModelPool->instanceBuffer[0]};
    VkDeviceSize offsets[] = {0, 0, allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 3, vertex3DBuffer, offsets);

    vkCmdBindIndexBuffer(currentCommandBuffer, (*allInOne.pIndexBuffer3D)[currentFrame], 0, VK_INDEX_TYPE_UINT32);

    // model
    drawModel(TEXTURE_BOTTOM, currentFrame, currentCommandBuffer);

    drawModel(TEXTURE_MODEL, currentFrame, currentCommandBuffer);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordComputeCommandBuffer(void)
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkCommandBuffer currentCommandBuffer = (*allInOne.ppComputeCommandBuffer)[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(*(allInOne.pExtent2D), currentCommandBuffer);
    setScissor(*(allInOne.pExtent2D), currentCommandBuffer);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pComputePipeline);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pComputePipelineLayout, 0, 1, &(*allInOne.ppComputeDescriptorSets)[currentFrame], 0, NULL);

    vkCmdDispatch(currentCommandBuffer, PARTICLE_COUNT / 256, 1, 1);
}
static void recordSSGICommandBuffer(void)
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkCommandBuffer currentCommandBuffer = (*allInOne.ppComputeCommandBuffer)[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(*(allInOne.pExtent2D), currentCommandBuffer);
    setScissor(*(allInOne.pExtent2D), currentCommandBuffer);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pSSGIPipeline);

    VkDescriptorSet descriptorSets[] = {(*allInOne.ppSSGIDescriptorSets)[currentFrame], (*allInOne.ppSSGIDescriptorSets + 2)[currentFrame]};
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pSSGIPipelineLayout, 0, 2, descriptorSets, 0, NULL);

    vkCmdDispatch(currentCommandBuffer, (allInOne.pExtent2D->width + 8 - 1) / 8, (allInOne.pExtent2D->height + 8 - 1) / 8, 1);
}
static void recordCommandBufferCombine(Uint32 imageIndex)
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkCommandBuffer currentCommandBuffer = (*allInOne.ppGraphicCommandBuffer)[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(*(allInOne.pExtent2D), currentCommandBuffer);
    setScissor(*(allInOne.pExtent2D), currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, *allInOne.pExtent2D};

    VkClearValue clearValue[1];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = *allInOne.pCombineRenderPass;
    renderBeginInfo.framebuffer = (*allInOne.ppCombineFramebuffer)[imageIndex];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombinePipeline);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombinePipelineLayout, 0, 1, (*allInOne.ppCombineDescriptorSets) + currentFrame, 0, NULL);

    vkCmdDraw(currentCommandBuffer, 6, 1, 0, 0);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void setSubmitInfo(void * pNext, Uint32 waitSeamphoreCount, const VkSemaphore * pWaitSemaphores, VkPipelineStageFlagBits * pWaitDstStageMask, Uint32 commandBufferCount\
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
static void drawFirstScene(void)
{
    Uint32 currentFrame = *allInOne.pCurrentFrame;
    VkSemaphore * timelineSemaphore = (*allInOne.ppTimelineSemaphore1) + currentFrame;
    Uint64 waitValue[] = {0, 0};
    vkGetSemaphoreCounterValue(*allInOne.pDevice, *timelineSemaphore, &waitValue[0]);
    Uint64 signalValue[] = {1, 0};
    signalValue[0] = waitValue[0] + 1;

    // shadow
    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);
    //printf("reset fences\n");

    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), 0);
    recordCommandBufferShadow();
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), 0);

    VkTimelineSemaphoreSubmitInfo timelineSemaphoreInfo = {};
    timelineSemaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timelineSemaphoreInfo.pNext = NULL;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo submitInfoShadow = {};
    setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, (*allInOne.ppGraphicCommandBuffer) + currentFrame, 1, timelineSemaphore, &submitInfoShadow);
    // print("semaphore value: %u", SDL_GetSemaphoreValue(allSync.vertexSemaphore));
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfoShadow, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);
    signalValue[0]++;

    G_Texture_P * shadowTexture = getTexture(TEXTURE_SHADOW);
    transitionImageLayout(&shadowTexture->image, shadowTexture->format, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // 3d object
    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);

    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), 0);
    recordCommandBuffer_3D();
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), 0);


    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkPipelineStageFlags waitStage_3D[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo3D = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore, waitStage_3D, 1, (*allInOne.ppGraphicCommandBuffer) + currentFrame, 1, timelineSemaphore, &submitInfo3D);
    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfo3D, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);
    signalValue[0]++;
    

    G_Texture_P * depthTexture = getTexture(TEXTURE_MODEL_DEPTH);
    transitionImageLayout(&depthTexture->image, depthTexture->format, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    G_Texture_P * normalTexture = getTexture(TEXTURE_NORMAL);
    transitionImageLayout(&normalTexture->image, normalTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    G_Texture_P * colorTexture = getTexture(TEXTURE_MODEL_COLOR);
    transitionImageLayout(&colorTexture->image, colorTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    G_Texture_P * ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(&ssgiTexture->image, ssgiTexture->format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    // SSGI
    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);

    vkResetCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame], 0);
    recordSSGICommandBuffer();
    vkEndCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame]);

    VkPipelineStageFlags SSGIWaitStage[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo SSGISubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore, SSGIWaitStage, 1, (*allInOne.ppComputeCommandBuffer) + currentFrame, 1, timelineSemaphore, &SSGISubmitInfo);
    vkQueueSubmit(*allInOne.pComputeQueue, 1, &SSGISubmitInfo, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]);
    signalValue[0]++;

    ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(&ssgiTexture->image, ssgiTexture->format, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // combine and present
    Uint32 imageIndex_3D = 0;
    resultVulkan(vkAcquireNextImageKHR(*allInOne.pDevice, *allInOne.pSwapchain3D, UINT64_MAX, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame], NULL, &imageIndex_3D), 0);

    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);

    vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0);
    recordCommandBufferCombine(imageIndex_3D);
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSemaphore combineWaitSemaphores[] = {*timelineSemaphore, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame]};
    VkPipelineStageFlags combineWaitStage[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore combineSignalSemaphores[] = {*timelineSemaphore, (*allInOne.ppRenderFinishedSemaphore)[*allInOne.pCurrentFrame]};

    VkSubmitInfo combinbeSubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 2, combineWaitSemaphores, combineWaitStage, 1, (*allInOne.ppGraphicCommandBuffer) + currentFrame, 2, combineSignalSemaphores, &combinbeSubmitInfo);
    vkQueueSubmit(*allInOne.pGraphicQueue, 1, &combinbeSubmitInfo, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]);
    signalValue[0]++;

    VkPresentInfoKHR presentInfo_3D = {};
    presentInfo_3D.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo_3D.pNext = NULL;
    presentInfo_3D.waitSemaphoreCount = 1;
    presentInfo_3D.pWaitSemaphores = (*allInOne.ppRenderFinishedSemaphore) + *allInOne.pCurrentFrame;
    presentInfo_3D.swapchainCount = 1;
    presentInfo_3D.pSwapchains = allInOne.pSwapchain3D;
    presentInfo_3D.pImageIndices = &imageIndex_3D;
    presentInfo_3D.pResults = NULL;
    resultVulkan(vkQueuePresentKHR(*allInOne.pPresentQueue, &presentInfo_3D), 0);


    // particle
    resultVulkan(vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX), 0);
    vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame]);

    vkResetCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame], 0);
    recordComputeCommandBuffer();
    vkEndCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame]);

    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo particleSubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, (*allInOne.ppComputeCommandBuffer) + currentFrame, 1, timelineSemaphore, &particleSubmitInfo);
    vkQueueSubmit(*allInOne.pComputeQueue, 1, &particleSubmitInfo, (*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame]);
    signalValue[0]++;


    // 2d and present
    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);

    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), 0);
    recordCommandBuffer2D();
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkPipelineStageFlagBits graphic2dWaitStage[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT};

    VkSubmitInfo submitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore, graphic2dWaitStage, 1, (*allInOne.ppGraphicCommandBuffer) + currentFrame, 1, timelineSemaphore, &submitInfo);
    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfo, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);
    signalValue[0]++;

    G_Texture_P * shadow2dTexture = getTexture(TEXTURE_SHADOW_MAP);
    G_Texture_P * graphic2dTexture = getTexture(TEXTURE_2D_COLOR);
    transitionImageLayout(&shadow2dTexture->image, shadow2dTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    transitionImageLayout(&graphic2dTexture->image, graphic2dTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    Uint32 imageIndex;
    resultVulkan(vkAcquireNextImageKHR(*allInOne.pDevice, *allInOne.pSwapchain2D, UINT64_MAX, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame], NULL, &imageIndex), 0);

    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);

    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), 0);
    //printf("reset command bufer\n");
    recordCommandBufferCombine2D(imageIndex);
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), 0);

    VkSemaphore waitSemaphore[2] = {*timelineSemaphore, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame]};
    VkPipelineStageFlags waitStage[2] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore signalSemaphore[2] = {*timelineSemaphore, (*allInOne.ppRenderFinishedSemaphore)[*allInOne.pCurrentFrame]};

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo combineSubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 2, waitSemaphore, waitStage, 1, (*allInOne.ppGraphicCommandBuffer) + currentFrame, 2, signalSemaphore, &combineSubmitInfo);
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &combineSubmitInfo, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), 0);
    signalValue[0]++;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = (*allInOne.ppRenderFinishedSemaphore) + *allInOne.pCurrentFrame;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = allInOne.pSwapchain2D;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL;
    resultVulkan(vkQueuePresentKHR(*allInOne.pPresentQueue, &presentInfo), 0);
    //printf("present queue\n");
}
void drawFrame(Scene scene)
{
    switch (scene)
    {
        case First_Scene:
        drawFirstScene();
        break;
        
        case Pause_Scene:
        break;

        case Menu_Scene:
        break;
    }
}