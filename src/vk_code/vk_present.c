#include "G_constants.h"

#include "SDL_mutex.h"
#include "SDL_stdinc.h"
#include "vk_code_h/vk_buffer.h"
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
#include "G_map.h"
#include "G_scene.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;


static void recordCommandBufferBottom(Uint32 currentFrame, bool graphicCopy, VkCommandBuffer currentCommandBuffer)
{
    beginPrimaryCommandBuffer(currentCommandBuffer);

    if (graphicCopy) vkCmdExecuteCommands(currentCommandBuffer, 1, allInOne.pGraphicCopyCommandBuffer + currentFrame);

    setViewport(allInOne.extent2D, currentCommandBuffer);

    setScissor(allInOne.extent2D, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, allInOne.extent2D};

    VkClearValue clearValue[2];
    clearValue[0].color= (VkClearColorValue){{0.0f, 0.0f, 0.0f, 0.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {0};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = allInOne.renderPass;
    renderBeginInfo.framebuffer = allInOne.pBottomFramebuffer[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.graphicPipeline);
    vkCmdPushConstants(currentCommandBuffer, allInOne.graphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);
    G_Buffer* vertexBuffer[] = {allInOne.vertexBuffer2D[currentFrame]};
    G_vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffer);
    G_vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer2D, VK_INDEX_TYPE_UINT16);

    drawPic(TEXTURE_TILE_SET, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordCommandBuffer2D(Uint32 imageIndex, Uint32 currentFrame, VkCommandBuffer currentCommandBuffer)
{
    beginPrimaryCommandBuffer(currentCommandBuffer);

    VkBufferMemoryBarrier bufferBarrierGet = {0};
    _setBufferMemoryBarrier(NULL, 0, VK_ACCESS_SHADER_READ_BIT, allInOne.queueFamilyIndices.computeFamily.familyIndice, allInOne.queueFamilyIndices.graphicsFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[currentFrame]->pBufferPool->buffer, allInOne.pShaderStorageBuffer[currentFrame]->startOffset, sizeof(Particle) * PARTICLE_COUNT, &bufferBarrierGet);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, NULL, 1, &bufferBarrierGet, 0, NULL);

    setViewport(allInOne.extent2D, currentCommandBuffer);

    setScissor(allInOne.extent2D, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, allInOne.extent2D};

    VkClearValue clearValue[2];
    clearValue[0].color= (VkClearColorValue){{0.0f, 0.0f, 0.0f, 0.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {0};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = allInOne.renderPass;
    renderBeginInfo.framebuffer = allInOne.pGraphic2dFramebuffer[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.particlePipeline);
    G_vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, allInOne.pShaderStorageBuffer + currentFrame);
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.particlePipelineLayout, 0,\
    1, &allInOne.pParticleDescriptorSets[currentFrame], 0, NULL);
    vkCmdDraw(currentCommandBuffer, PARTICLE_COUNT, 1, 0, 0);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shapePipeline);
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shapePipelineLayout, 0, 1, allInOne.pShapeDescriptorSets + currentFrame, 0, NULL);
    vkCmdPushConstants(currentCommandBuffer, allInOne.shapePipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShapeConstants), allInOne.pShapeConstants);
    G_vkCmdBindVertexBuffers (currentCommandBuffer, 0, 1, &allInOne.tempBuffer);
    vkCmdDraw(currentCommandBuffer, 5, 1, 0, 0);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.graphicPipeline);
    vkCmdPushConstants(currentCommandBuffer, allInOne.graphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);
    G_Buffer* vertexBuffer[] = {allInOne.vertexBuffer2D[currentFrame]};
    G_vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffer);
    G_vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer2D, VK_INDEX_TYPE_UINT16);

    //loading1 png
    drawPic(TEXTURE_LOADING, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);
    //circle
    // drawPic(TEXTURE_CIRCLE, currentFrame, currentCommandBuffer);
    // box
    // drawPic(TEXTURE_BOX, currentFrame, currentCommandBuffer);
    // main font png
    drawPic(TEXTURE_FONT, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);

    // drawPic(TEXTURE_TILE_SET, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);
    vkCmdEndRenderPass(currentCommandBuffer);

    VkBufferMemoryBarrier bufferBarrierRelease = {0};
    _setBufferMemoryBarrier(NULL, VK_ACCESS_SHADER_READ_BIT, 0, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.queueFamilyIndices.computeFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[currentFrame]->pBufferPool->buffer, allInOne.pShaderStorageBuffer[currentFrame]->startOffset, sizeof(Particle) * PARTICLE_COUNT, &bufferBarrierRelease);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 1, &bufferBarrierRelease, 0, NULL);
}
static void recordCommandBufferShadow(Uint32 currentFrame, bool graphicCopy, VkCommandBuffer currentCommandBuffer)
{
    beginPrimaryCommandBuffer(currentCommandBuffer);

    if (graphicCopy) vkCmdExecuteCommands(currentCommandBuffer, 1, allInOne.pGraphicCopyCommandBuffer + currentFrame);

    VkExtent2D shadow = {SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT};
    setViewport(shadow, currentCommandBuffer);
    setScissor(shadow, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, shadow};

    VkClearValue clearValue[3];
    clearValue[0].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {0};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = allInOne.shadowRenderPass;
    renderBeginInfo.framebuffer = allInOne.pShadowFramebuffer[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shadowPipeline);

    VkBuffer vertex3DBuffer[] = {allInOne.vertexBuffer3D[currentFrame]->pBufferPool->buffer, allInOne.pStaticModelPool->instanceBuffer->pBufferPool->buffer, allInOne.pStaticModelPool->instanceBuffer->pBufferPool->buffer};
    VkDeviceSize offsets[] = {allInOne.vertexBuffer3D[currentFrame]->startOffset, allInOne.pStaticModelPool->instanceBuffer->startOffset, allInOne.pStaticModelPool->instanceBuffer->startOffset + allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 3, vertex3DBuffer, offsets);

    G_vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer3D[currentFrame], VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shadowPipelineLayout, 0,
    1, allInOne.pShadowDescriptorSets + currentFrame, 0, NULL);

    // model
    drawShadow(TEXTURE_BOTTOM, currentCommandBuffer);

    drawShadow(TEXTURE_MODEL, currentCommandBuffer);

    drawShadow(TEXTURE_VOXEL, currentCommandBuffer);

    vkCmdEndRenderPass(currentCommandBuffer);

    setTextureImageLayout(getTexture(TEXTURE_SHADOW), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 0, 1);
}
static void recordCommandBuffer_3D(Uint32 currentFrame, VkCommandBuffer currentCommandBuffer)
{
    beginPrimaryCommandBuffer(currentCommandBuffer);

    G_Texture_P * shadowTexture = getTexture(TEXTURE_SHADOW);
    transitionImageLayout(currentCommandBuffer, shadowTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);

    setViewport(allInOne.extent2D, currentCommandBuffer);
    setScissor(allInOne.extent2D, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, allInOne.extent2D};

    VkClearValue clearValue[4];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[1].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[2].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[3].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {0};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = allInOne.modelRenderPass;
    renderBeginInfo.framebuffer = allInOne.pDirectColorFramebuffer[currentFrame];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 4;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


    VkBuffer vertex3DBuffer[] = {allInOne.vertexBuffer3D[currentFrame]->pBufferPool->buffer, allInOne.pStaticModelPool->instanceBuffer->pBufferPool->buffer, allInOne.pStaticModelPool->instanceBuffer->pBufferPool->buffer};
    VkDeviceSize offsets[] = {allInOne.vertexBuffer3D[currentFrame]->startOffset, allInOne.pStaticModelPool->instanceBuffer->startOffset, allInOne.pStaticModelPool->instanceBuffer->startOffset + allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 3, vertex3DBuffer, offsets);

    G_vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer3D[currentFrame], VK_INDEX_TYPE_UINT32);

    // model
    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.modelPipeline);

    drawModel(TEXTURE_MODEL, currentFrame, currentCommandBuffer);

    drawModel(TEXTURE_BOTTOM, currentFrame, currentCommandBuffer);

    drawModel(TEXTURE_VOXEL, currentFrame, currentCommandBuffer);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordComputeCommandBuffer(Uint32 currentFrame, bool computeCopy, VkCommandBuffer currentCommandBuffer)
{
    beginPrimaryCommandBuffer(currentCommandBuffer);

    if (computeCopy) vkCmdExecuteCommands(currentCommandBuffer, 1, allInOne.pComputeCopyCommandBuffer + currentFrame);

    VkBufferMemoryBarrier bufferMemoryBarrierGet = {0};
    _setBufferMemoryBarrier(NULL, 0, VK_ACCESS_SHADER_READ_BIT, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.queueFamilyIndices.computeFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[(currentFrame + 1) % 2]->pBufferPool->buffer, allInOne.pShaderStorageBuffer[(currentFrame + 1) % 2]->startOffset, sizeof(Particle) * PARTICLE_COUNT, &bufferMemoryBarrierGet);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 1, &bufferMemoryBarrierGet, 0, NULL);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.computePipeline);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.computePipelineLayout, 0, 1, &allInOne.pComputeDescriptorSets[currentFrame], 0, NULL);

    vkCmdDispatch(currentCommandBuffer, PARTICLE_COUNT / 256, 1, 1);

    VkBufferMemoryBarrier bufferMemoryBarrierRelease = {0};
    _setBufferMemoryBarrier(NULL, VK_ACCESS_SHADER_WRITE_BIT, 0, allInOne.queueFamilyIndices.computeFamily.familyIndice, allInOne.queueFamilyIndices.graphicsFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[currentFrame]->pBufferPool->buffer, allInOne.pShaderStorageBuffer[currentFrame]->startOffset, sizeof(Particle) * PARTICLE_COUNT, &bufferMemoryBarrierRelease);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 1, &bufferMemoryBarrierRelease, 0, NULL);
}
static void recordSSGICommandBuffer(Uint32 currentFrame, Uint32 width, Uint32 height, VkCommandBuffer currentCommandBuffer)
{
    beginPrimaryCommandBuffer(currentCommandBuffer);

    // G_Texture_P * depthTexture = getTexture(TEXTURE_MODEL_DEPTH);
    // transitionImageLayout(currentCommandBuffer, depthTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
    // G_Texture_P * normalTexture = getTexture(TEXTURE_NORMAL);
    // transitionImageLayout(currentCommandBuffer, normalTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
    // G_Texture_P * colorTexture = getTexture(TEXTURE_MODEL_COLOR);
    // transitionImageLayout(currentCommandBuffer, colorTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
    G_Texture_P * ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(currentCommandBuffer, ssgiTexture, VK_IMAGE_LAYOUT_GENERAL, 0, 1);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.SSGIPipeline);

    VkDescriptorSet descriptorSets[] = {allInOne.pSSGIDescriptorSets[currentFrame], (allInOne.pSSGIDescriptorSets + 2)[currentFrame]};
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.SSGIPipelineLayout, 0, 2, descriptorSets, 0, NULL);

    vkCmdDispatch(currentCommandBuffer, (width + 8 - 1) / 8, (height + 8 - 1) / 8, 1);
}
static void recordCommandBufferCombine(Uint32 imageIndex, Uint32 currentFrame, VkCommandBuffer currentCommandBuffer)
{
    beginPrimaryCommandBuffer(currentCommandBuffer);

    G_Texture_P * ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(currentCommandBuffer, ssgiTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);

    setViewport(allInOne.extent2D, currentCommandBuffer);
    setScissor(allInOne.extent2D, currentCommandBuffer);

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, allInOne.extent2D};

    VkClearValue clearValue[1];
    clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderBeginInfo = {0};
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
static void draw3d(void * data)
{
    void ** pData = (void **)((G_Task*)data)->arg;

    G_ThreadCommandPool * pThreadCommandPool;
    Uint32 currentFrame, width, height;
    pThreadCommandPool = (G_ThreadCommandPool*)pData[0];
    currentFrame = *(Uint32 *)pData[1];
    width = *(Uint32 *)pData[2];
    height = *(Uint32 *)pData[3];

    VkCommandBuffer graphicCommandBuffer = pThreadCommandPool->pGraphicCommandBuffer[currentFrame];
    VkFence * pFence = pThreadCommandPool->pFence + currentFrame;
    VkSemaphore * pDoneSemaphore = pThreadCommandPool->pThreadDoneSemaphore + currentFrame;
    VkTimelineSemaphoreSubmitInfo timelineSemaphoreInfo = {0};
    timelineSemaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;

    VkSemaphore * timelineSemaphore = pThreadCommandPool->pSemaphore + currentFrame;
    Uint64 waitValue[] = {0, 0};
    vkGetSemaphoreCounterValue(allInOne.device, *timelineSemaphore, &waitValue[0]);
    Uint64 signalValue[] = {1, 0};
    signalValue[0] = waitValue[0] + 1;

    // bottom
    vkWaitForFences(allInOne.device, 1, pFence, VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetCommandBuffer(graphicCommandBuffer, 0), 0);
    recordCommandBufferBottom(currentFrame, false, graphicCommandBuffer);
    resultVulkan(vkEndCommandBuffer(graphicCommandBuffer), 0);

    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo submitInfoBottom = {0};
    setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, &graphicCommandBuffer, 1, timelineSemaphore, &submitInfoBottom);
    // SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkResetFences(allInOne.device, 1, pFence), 0);
    resultVulkan(G_vkQueueSubmit(&allInOne.pGraphicQueue[GRAPHIC_2D_QUEUE], 1, &submitInfoBottom, *pFence), 0);
    signalValue[0]++;

    // shadow
    vkWaitForFences(allInOne.device, 1, pFence, VK_TRUE, UINT64_MAX);
    //printf("reset fences\n");

    resultVulkan(vkResetCommandBuffer(graphicCommandBuffer, 0), 0);
    recordCommandBufferShadow(currentFrame, false, graphicCommandBuffer);
    resultVulkan(vkEndCommandBuffer(graphicCommandBuffer), 0);

    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    // VkPipelineStageFlags waitStage_Shadow[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfoShadow = {0};
    setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, &graphicCommandBuffer, 1, timelineSemaphore, &submitInfoShadow);
    // print("semaphore value: %u", SDL_GetSemaphoreValue(allSync.vertexSemaphore));
    // SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkResetFences(allInOne.device, 1, pFence), 0);
    resultVulkan(G_vkQueueSubmit(&allInOne.pGraphicQueue[GRAPHIC_3D_QUEUE], 1, &submitInfoShadow, *pFence), 0);
    signalValue[0]++;

    // 3d object
    vkWaitForFences(allInOne.device, 1, pFence, VK_TRUE, UINT64_MAX);

    resultVulkan(vkResetCommandBuffer(graphicCommandBuffer, 0), 0);
    recordCommandBuffer_3D(currentFrame, graphicCommandBuffer);
    resultVulkan(vkEndCommandBuffer(graphicCommandBuffer), 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSemaphore graphicWaitSemaphores[] = {*timelineSemaphore};
    VkPipelineStageFlags waitStage_3D[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};
    VkSubmitInfo submitInfo3D = {0};
    setSubmitInfo(&timelineSemaphoreInfo, 1, graphicWaitSemaphores, waitStage_3D, 1, &graphicCommandBuffer, 1, timelineSemaphore, &submitInfo3D);
    // SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkResetFences(allInOne.device, 1, pFence), 0);
    resultVulkan(G_vkQueueSubmit(&allInOne.pGraphicQueue[GRAPHIC_3D_QUEUE], 1, &submitInfo3D, *pFence), 0);
    signalValue[0]++;

    // SSGI
    vkWaitForFences(allInOne.device, 1, pFence, VK_TRUE, UINT64_MAX);

    vkResetCommandBuffer(graphicCommandBuffer, 0);
    recordSSGICommandBuffer(currentFrame, width, height, graphicCommandBuffer);
    vkEndCommandBuffer(graphicCommandBuffer);

    VkPipelineStageFlags SSGIWaitStage[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue + 1;

    VkSubmitInfo SSGISubmitInfo = {0};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore, SSGIWaitStage, 1, &graphicCommandBuffer, 1, pDoneSemaphore, &SSGISubmitInfo);
    // SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkResetFences(allInOne.device, 1, pFence), 0);
    G_vkQueueSubmit(&allInOne.pGraphicQueue[GRAPHIC_3D_QUEUE], 1, &SSGISubmitInfo, *pFence);
}
static void draw2d(void * data)
{
    void ** pData = (void **)((G_Task*)data)->arg;

    G_ThreadCommandPool * pThreadCommandPool;
    Uint32 currentFrame;
    pThreadCommandPool = (G_ThreadCommandPool*)pData[0];
    currentFrame = *(Uint32 *)pData[1];

    VkCommandBuffer graphicCommandBuffer = pThreadCommandPool->pGraphicCommandBuffer[currentFrame];
    VkCommandBuffer computeCommandBuffer = pThreadCommandPool->pComputeCommandBuffer[currentFrame];
    VkFence * pFence = pThreadCommandPool->pFence + currentFrame;
    VkFence * pComputeFence = &pThreadCommandPool->pComputeFence[currentFrame];
    VkSemaphore * timelineSemaphore = pThreadCommandPool->pSemaphore + currentFrame;
    VkSemaphore * pDoneSemaphore = pThreadCommandPool->pThreadDoneSemaphore + currentFrame;
    VkTimelineSemaphoreSubmitInfo timelineSemaphoreInfo = {0};
    timelineSemaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;

    Uint64 waitValue[] = {0, 0};
    vkGetSemaphoreCounterValue(allInOne.device, *timelineSemaphore, &waitValue[0]);
    Uint64 signalValue[] = {1, 0};
    signalValue[0] = waitValue[0] + 1;

    // particle
    resultVulkan(vkWaitForFences(allInOne.device, 1, pComputeFence, VK_TRUE, UINT64_MAX), 0);

    vkResetCommandBuffer(computeCommandBuffer, 0);
    recordComputeCommandBuffer(currentFrame, false, computeCommandBuffer);
    vkEndCommandBuffer(computeCommandBuffer);

    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo particleSubmitInfo = {0};
    setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, &computeCommandBuffer, 1, timelineSemaphore, &particleSubmitInfo);
    // SDL_WaitSemaphore(allSync.vertexSemaphore);
    vkResetFences(allInOne.device, 1, pComputeFence);
    G_vkQueueSubmit(&allInOne.pComputeQueue[COMPUTE_QUEUE], 1, &particleSubmitInfo, *pComputeFence);
    signalValue[0]++;

    // 2d
    resultVulkan(vkWaitForFences(allInOne.device, 1, pFence, VK_TRUE, UINT64_MAX), 0);

    vkResetCommandBuffer(graphicCommandBuffer, 0);
    recordCommandBuffer2D(0, currentFrame, graphicCommandBuffer);
    resultVulkan(vkEndCommandBuffer(graphicCommandBuffer), 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue + 1;

    VkSemaphore graphic2dWaitSemaphores[] = {*timelineSemaphore};
    VkPipelineStageFlagBits graphic2dWaitStage[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT};

    VkSubmitInfo combinbeSubmitInfo = {0};
    setSubmitInfo(&timelineSemaphoreInfo, 1, graphic2dWaitSemaphores, graphic2dWaitStage, 1, &graphicCommandBuffer, 1, pDoneSemaphore, &combinbeSubmitInfo);
    // SDL_WaitSemaphore(allSync.vertexSemaphore);
    vkResetFences(allInOne.device, 1, pFence);
    G_vkQueueSubmit(&allInOne.pGraphicQueue[GRAPHIC_2D_QUEUE], 1, &combinbeSubmitInfo, *pFence);
}
static void combine(void * data)
{
    // void ** pData = (void **)((G_Task*)data)->arg;
    void ** pData = (void **)data;

    G_ThreadCommandPool * pThreadCommandPool;
    Uint32 currentFrame;
    pThreadCommandPool = (G_ThreadCommandPool*)pData[0];
    currentFrame = *(Uint32 *)pData[1];

    VkCommandBuffer graphicCommandBuffer = pThreadCommandPool->pGraphicCommandBuffer[currentFrame];
    VkFence * pFence = pThreadCommandPool->pFence + currentFrame;
    VkSemaphore * semaphore3d = (VkSemaphore*)pData[2];
    VkSemaphore * semaphore2d = (VkSemaphore*)pData[3];
    Uint32 * imageIndex = (Uint32*)pData[4];
    
    Uint32 imageIndex_3D = 0;
    vkWaitForFences(allInOne.device, 1, pFence, VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, pFence), 0);
    resultVulkan(vkAcquireNextImageKHR(allInOne.device, allInOne.swapchain3D, UINT64_MAX, allInOne.pImageAvailableSemaphore[currentFrame], *pFence, &imageIndex_3D), 0);

    // combine and present
    vkWaitForFences(allInOne.device, 1, pFence, VK_TRUE, UINT64_MAX);

    resultVulkan(vkResetCommandBuffer(graphicCommandBuffer, 0), 0);
    recordCommandBufferCombine(imageIndex_3D, currentFrame, graphicCommandBuffer);
    resultVulkan(vkEndCommandBuffer(graphicCommandBuffer), 0);

    Uint32 waitSemaphoreCount = 3;

    VkSemaphore combineWaitSemaphores[3] = {allInOne.pImageAvailableSemaphore[currentFrame], *semaphore2d, NULL};

    if (semaphore3d) 
    {
        combineWaitSemaphores[2] = *semaphore3d;
    }
    else waitSemaphoreCount--;

    VkPipelineStageFlags combineWaitStage[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};
    VkSemaphore graphic2dSignalSemaphores[] = {allInOne.pRenderFinishedSemaphore[currentFrame]};

    VkSubmitInfo submitInfo = {0};
    setSubmitInfo(NULL, waitSemaphoreCount, combineWaitSemaphores, combineWaitStage, 1, &graphicCommandBuffer, 1, graphic2dSignalSemaphores, &submitInfo);
    resultVulkan(vkResetFences(allInOne.device, 1, pFence), 0);
    resultVulkan(G_vkQueueSubmit(&allInOne.pGraphicQueue[GRAPHIC_2D_QUEUE], 1, &submitInfo, *pFence), 0);
    *imageIndex = imageIndex_3D;
}
static void drawFirstScene(Scene scene, Uint32 currentFrame, Uint32 width, Uint32 height, bool bottomMoved, Uint8 copy, G_Thread_Pool * pThreadPool)
{
    Uint32 imageIndex = 0;
    int * taskIndex1, *taskIndex2, *taskIndex3;

    SceneParameter * scenePack = getSceneParameter(scene);

    bool graphicCopy, computeCopy, transferCopy;
    graphicCopy = computeCopy = transferCopy = false;

    if (copy & 1) graphicCopy = true;
    if (copy & 2) computeCopy = true;
    if (copy & 4) transferCopy = true;

    if (copy)
    {
        VkSubmitInfo submitInfo = {0};
        vkWaitForFences(allInOne.device, 1, &allInOne.mainThreadCommandPool.pFence[currentFrame], VK_TRUE, UINT64_MAX);
        setSubmitInfo(NULL, 0, NULL, NULL, 1, &allInOne.mainThreadCommandPool.pGraphicCommandBuffer[currentFrame], 0, NULL, &submitInfo);
        beginPrimaryCommandBuffer(allInOne.mainThreadCommandPool.pGraphicCommandBuffer[currentFrame]);
        if (graphicCopy) vkCmdExecuteCommands(allInOne.mainThreadCommandPool.pGraphicCommandBuffer[currentFrame], 1, &allInOne.pGraphicCopyCommandBuffer[currentFrame]);
        vkEndCommandBuffer(allInOne.mainThreadCommandPool.pGraphicCommandBuffer[currentFrame]);
        resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.mainThreadCommandPool.pFence[currentFrame]), 0);
        G_vkQueueSubmit(&allInOne.pGraphicQueue[GRAPHIC_2D_QUEUE], 1, &submitInfo, allInOne.mainThreadCommandPool.pFence[currentFrame]);
        graphicCopy = false;
        vkWaitForFences(allInOne.device, 1, &allInOne.mainThreadCommandPool.pFence[currentFrame], VK_TRUE, UINT64_MAX);
    }

    Uint64 semaphore3dValue, semaphore2dValue;

    void * pData1[] = {&allInOne.graphic3dThreadCommandPool, &currentFrame, &width, &height, &semaphore3dValue};
    void * pData2[] = {&allInOne.graphic2dThreadCommandPool, &currentFrame, &semaphore2dValue};

    void * pData3[] = {&allInOne.mainThreadCommandPool, &currentFrame, allInOne.graphic3dThreadCommandPool.pThreadDoneSemaphore + currentFrame, allInOne.graphic2dThreadCommandPool.pThreadDoneSemaphore + currentFrame\
        , &imageIndex};

    G_Task task = {0};
    task.arg = pData1;
    task.executeFunc = draw3d;
    task.func = NULL;
    taskIndex1 = NULL;
    if (scenePack->draw3d) taskIndex1 = G_AddTask(pThreadPool, 1, 1, &task);
    else pData3[2] = NULL;

    task.executeFunc = draw2d;
    task.arg = pData2;
    taskIndex2 = NULL;
    if (scenePack->draw2d) taskIndex2 = G_AddTask(pThreadPool, 1, 1, &task);
    else pData3[3] = NULL;

    G_WaitTask(pThreadPool, taskIndex1);
    G_WaitTask(pThreadPool, taskIndex2);

    combine(pData3);

    VkPresentInfoKHR presentInfo_3D = {0};
    presentInfo_3D.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo_3D.pNext = NULL;
    presentInfo_3D.waitSemaphoreCount = 1;
    presentInfo_3D.pWaitSemaphores = allInOne.pRenderFinishedSemaphore + currentFrame;
    presentInfo_3D.swapchainCount = 1;
    presentInfo_3D.pSwapchains = &allInOne.swapchain3D;
    presentInfo_3D.pImageIndices = &imageIndex;
    presentInfo_3D.pResults = NULL;
    resultVulkan(vkQueuePresentKHR(allInOne.pPresentQueue[PRESENT_QUEUE].queue, &presentInfo_3D), 0);
}
// void drawFrame(Scene scene, Uint32 currentFrame, Uint32 width, Uint32 height, bool bottomMoved, Uint8 copy, G_Thread_Pool * pThreadPool)
void drawFrame(Scene scene, Uint32 currentFrame, Uint32 width, Uint32 height, bool bottomMoved, Uint8 copy, G_Thread_Pool * pThreadPool)
{

    drawFirstScene(scene, currentFrame, width, height, bottomMoved, copy, pThreadPool);
}
