#include "G_constants.h"

#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_all_struct.h"

#include "SDL3/SDL_timer.h"

#include "G_log.h"
#include "G_struct.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;

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
static void recordCommandBuffer2D(Uint32 imageIndex, Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

    setViewport(allInOne.extent2D, currentCommandBuffer);

    setScissor(allInOne.extent2D, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, allInOne.extent2D};

    VkClearValue clearValue[2];
    clearValue[0].color= (VkClearColorValue){{0.0f, 0.0f, 0.0f, 0.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = allInOne.renderPass;
    renderBeginInfo.framebuffer = allInOne.pGraphic2dFramebuffer[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.graphicPipeline);

    vkCmdPushConstants(currentCommandBuffer, allInOne.graphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);

    VkBuffer vertexBuffer[] = {allInOne.vertexBuffer2D[currentFrame]};
    // VkDeviceSize vertexOffsets1[] = {0, allInOne.maxVerticesCount * sizeof(vec3), allInOne.maxVerticesCount * sizeof(vec3) + allInOne.maxVerticesCount * sizeof(vec3)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffer, offsets);

    vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer2D, 0, VK_INDEX_TYPE_UINT16);

    // tile map
    // drawPic(TEXTURE_TILE_SET, currentFrame, currentCommandBuffer);

    //loading1 png
    drawPic(TEXTURE_LOADING, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);

    //circle
    // drawPic(TEXTURE_CIRCLE, currentFrame, currentCommandBuffer);

    // box
    // drawPic(TEXTURE_BOX, currentFrame, currentCommandBuffer);

    // main font png
    drawPic(TEXTURE_FONT, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);

    // vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.particlePipeline);

    // vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, allInOne.pShaderStorageBuffer + currentFrame, offsets);

    // vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.particlePipelineLayout, 0,\
    1, &allInOne.pParticleDescriptorSets[currentFrame], 0, NULL);

    // vkCmdDraw(currentCommandBuffer, PARTICLE_COUNT, 1, 0, 0);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordCommandBufferShadow(Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

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
    renderBeginInfo.renderPass = allInOne.shadowRenderPass;
    renderBeginInfo.framebuffer = allInOne.pShadowFramebuffer[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shadowPipeline);

    VkBuffer vertex3DBuffer[] = {allInOne.vertexBuffer3D[currentFrame], allInOne.pStaticModelPool->instanceBuffer[0], allInOne.pStaticModelPool->instanceBuffer[0]};
    VkDeviceSize offsets[] = {0, 0, allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 3, vertex3DBuffer, offsets);

    vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer3D[currentFrame], 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shadowPipelineLayout, 0,
    1, allInOne.pShadowDescriptorSets + currentFrame, 0, NULL);

    // model
    drawShadow(TEXTURE_BOTTOM, currentCommandBuffer);

    drawShadow(TEXTURE_MODEL, currentCommandBuffer);

    vkCmdEndRenderPass(currentCommandBuffer);
}

static void recordCommandBuffer_3D(Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(allInOne.extent2D, currentCommandBuffer);
    setScissor(allInOne.extent2D, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, allInOne.extent2D};

    VkClearValue clearValue[4];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[1].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[2].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[3].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = allInOne.modelRenderPass;
    renderBeginInfo.framebuffer = allInOne.pDirectColorFramebuffer[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 4;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


    VkBuffer vertex3DBuffer[] = {allInOne.vertexBuffer3D[currentFrame], allInOne.pStaticModelPool->instanceBuffer[0], allInOne.pStaticModelPool->instanceBuffer[0]};
    VkDeviceSize offsets[] = {0, 0, allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 3, vertex3DBuffer, offsets);

    vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer3D[currentFrame], 0, VK_INDEX_TYPE_UINT32);

    // model
    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.bottomPipeline);

    drawModel(TEXTURE_BOTTOM, currentFrame, currentCommandBuffer, true);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.modelPipeline);

    drawModel(TEXTURE_MODEL, currentFrame, currentCommandBuffer, false);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordComputeCommandBuffer(Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pComputeCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(allInOne.extent2D, currentCommandBuffer);
    setScissor(allInOne.extent2D, currentCommandBuffer);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.computePipeline);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.computePipelineLayout, 0, 1, &allInOne.pComputeDescriptorSets[currentFrame], 0, NULL);

    vkCmdDispatch(currentCommandBuffer, PARTICLE_COUNT / 256, 1, 1);
}
static void recordSSGICommandBuffer(Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(allInOne.extent2D, currentCommandBuffer);
    setScissor(allInOne.extent2D, currentCommandBuffer);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.SSGIPipeline);

    VkDescriptorSet descriptorSets[] = {allInOne.pSSGIDescriptorSets[currentFrame], (allInOne.pSSGIDescriptorSets + 2)[currentFrame]};
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.SSGIPipelineLayout, 0, 2, descriptorSets, 0, NULL);

    vkCmdDispatch(currentCommandBuffer, (allInOne.extent2D.width + 8 - 1) / 8, (allInOne.extent2D.height + 8 - 1) / 8, 1);
}
static void recordCommandBufferCombine(Uint32 imageIndex, Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);
    setViewport(allInOne.extent2D, currentCommandBuffer);
    setScissor(allInOne.extent2D, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, allInOne.extent2D};

    VkClearValue clearValue[1];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = allInOne.combineRenderPass;
    renderBeginInfo.framebuffer = allInOne.pCombineFramebuffer[imageIndex];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.combinePipeline);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.combinePipelineLayout, 0, 1, allInOne.pCombineDescriptorSets + currentFrame, 0, NULL);

    vkCmdDraw(currentCommandBuffer, 6, 1, 0, 0);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void drawFirstScene(Uint32 currentFrame, bool bottomDrawed)
{
    VkSemaphore * timelineSemaphore = allInOne.pTimelineSemaphore1 + currentFrame;
    Uint64 waitValue[] = {0, 0};
    vkGetSemaphoreCounterValue(allInOne.device, *timelineSemaphore, &waitValue[0]);
    Uint64 signalValue[] = {1, 0};
    signalValue[0] = waitValue[0] + 1;

    // particle
    // resultVulkan(vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[currentFrame], VK_TRUE, UINT64_MAX), 0);
    // vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[currentFrame]);

    // vkResetCommandBufferallInOne.pComputeCommandBuffer[currentFrame], 0);
    // recordComputeCommandBuffer();
    // vkEndCommandBufferallInOne.pComputeCommandBuffer[currentFrame]);

    // timelineSemaphoreInfo.pNext = NULL;
    // timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    // timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    // timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    // timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    // VkSubmitInfo particleSubmitInfo = {};
    // setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, allInOne.pComputeCommandBuffer + currentFrame, 1, timelineSemaphore, &particleSubmitInfo);
    // vkQueueSubmit(*allInOne.pComputeQueue, 1, &particleSubmitInfo, (*allInOne.ppComputeInFlightFence)[currentFrame]);
    // signalValue[0]++;

    // shadow
    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);
    //printf("reset fences\n");

    resultVulkan(vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0), 0);
    recordCommandBufferShadow(currentFrame);
    resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);

    VkTimelineSemaphoreSubmitInfo timelineSemaphoreInfo = {};
    timelineSemaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo submitInfoShadow = {};
    setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore, &submitInfoShadow);
    // print("semaphore value: %u", SDL_GetSemaphoreValue(allSync.vertexSemaphore));
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkQueueSubmit(getGraphic3dQueue(), 1, &submitInfoShadow, allInOne.pGraphicInFlightFence[currentFrame]), 0);
    signalValue[0]++;

    G_Texture_P * shadowTexture = getTexture(TEXTURE_SHADOW);
    transitionImageLayout(NULL, shadowTexture->image, shadowTexture->format, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);

    // 3d object
    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    resultVulkan(vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0), 0);
    recordCommandBuffer_3D(currentFrame);
    resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);


    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkPipelineStageFlags waitStage_3D[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo3D = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore, waitStage_3D, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore, &submitInfo3D);
    resultVulkan(vkQueueSubmit(getGraphic3dQueue(), 1, &submitInfo3D, allInOne.pGraphicInFlightFence[currentFrame]), 0);
    signalValue[0]++;
    

    G_Texture_P * depthTexture = getTexture(TEXTURE_MODEL_DEPTH);
    transitionImageLayout(NULL, depthTexture->image, depthTexture->format, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
    G_Texture_P * normalTexture = getTexture(TEXTURE_NORMAL);
    transitionImageLayout(NULL, normalTexture->image, normalTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
    G_Texture_P * colorTexture = getTexture(TEXTURE_MODEL_COLOR);
    transitionImageLayout(NULL, colorTexture->image, colorTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
    G_Texture_P * ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(NULL, ssgiTexture->image, ssgiTexture->format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 0, 1);

    // SSGI
    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0);
    recordSSGICommandBuffer(currentFrame);
    vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]);

    VkPipelineStageFlags SSGIWaitStage[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo SSGISubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore, SSGIWaitStage, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore, &SSGISubmitInfo);
    vkQueueSubmit(getGraphic3dQueue(), 1, &SSGISubmitInfo, allInOne.pGraphicInFlightFence[currentFrame]);
    signalValue[0]++;

    ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(NULL, ssgiTexture->image, ssgiTexture->format, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);

    // 2d and present
    Uint32 imageIndex_3D = 0;
    resultVulkan(vkAcquireNextImageKHR(allInOne.device, allInOne.swapchain3D, UINT64_MAX, allInOne.pImageAvailableSemaphore[currentFrame], NULL, &imageIndex_3D), 0);

    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0);
    recordCommandBuffer2D(imageIndex_3D, currentFrame);
    resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSemaphore graphic2dWaitSemaphores[] = {*timelineSemaphore, allInOne.pImageAvailableSemaphore[currentFrame]};
    VkPipelineStageFlags combineWaitStage[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo combinbeSubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 2, graphic2dWaitSemaphores, combineWaitStage, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore, &combinbeSubmitInfo);
    vkQueueSubmit(getGraphic2dQueue(), 1, &combinbeSubmitInfo, allInOne.pGraphicInFlightFence[currentFrame]);
    signalValue[0]++;

    // transitionImageLayout(NULL, allInOne.pSwapchain3DImages[imageIndex_3D], allInOne.surface3DFormat.format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, 1);

    // combine and present
    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    resultVulkan(vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0), 0);
    recordCommandBufferCombine(imageIndex_3D, currentFrame);
    resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkPipelineStageFlagBits graphic2dWaitStage[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT};
    VkSemaphore graphic2dSignalSemaphores[] = {*timelineSemaphore, allInOne.pRenderFinishedSemaphore[currentFrame]};

    VkSubmitInfo submitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore, graphic2dWaitStage, 1, allInOne.pGraphicCommandBuffer + currentFrame, 2, graphic2dSignalSemaphores, &submitInfo);
    resultVulkan(vkQueueSubmit(getGraphic2dQueue(), 1, &submitInfo, allInOne.pGraphicInFlightFence[currentFrame]), 0);
    signalValue[0]++;

    VkPresentInfoKHR presentInfo_3D = {};
    presentInfo_3D.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo_3D.pNext = NULL;
    presentInfo_3D.waitSemaphoreCount = 1;
    presentInfo_3D.pWaitSemaphores = allInOne.pRenderFinishedSemaphore + currentFrame;
    presentInfo_3D.swapchainCount = 1;
    presentInfo_3D.pSwapchains = &allInOne.swapchain3D;
    presentInfo_3D.pImageIndices = &imageIndex_3D;
    presentInfo_3D.pResults = NULL;
    resultVulkan(vkQueuePresentKHR(getPresentQueue(), &presentInfo_3D), 0);
}
void drawFrame(Scene scene, Uint32 currentFrame, bool bottomDrawed)
{
    switch (scene)
    {
        case First_Scene:
        drawFirstScene(currentFrame, bottomDrawed);
        break;
        
        case Pause_Scene:
        break;

        case Menu_Scene:
        break;
    }
}