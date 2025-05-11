#include "G_constants.h"

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

    VkBufferMemoryBarrier bufferBarrierGet = {};
    _setBufferMemoryBarrier(NULL, 0, VK_ACCESS_SHADER_READ_BIT, allInOne.queueFamilyIndices.computeFamily.familyIndice, allInOne.queueFamilyIndices.graphicsFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[currentFrame], 0, sizeof(Particle) * PARTICLE_COUNT, &bufferBarrierGet);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, NULL, 1, &bufferBarrierGet, 0, NULL);

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

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shapePipeline);
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.shapePipelineLayout, 0, 1, allInOne.pShapeDescriptorSets + currentFrame, 0, NULL);
    vkCmdPushConstants(currentCommandBuffer, allInOne.shapePipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShapeConstants), allInOne.pShapeConstants);
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, &allInOne.tempBuffer, offsets);
    vkCmdDraw(currentCommandBuffer, 5, 1, 0, 0);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.graphicPipeline);
    vkCmdPushConstants(currentCommandBuffer, allInOne.graphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);
    VkBuffer vertexBuffer[] = {allInOne.vertexBuffer2D[currentFrame]};
    // VkDeviceSize vertexOffsets1[] = {0, allInOne.maxVerticesCount * sizeof(vec3), allInOne.maxVerticesCount * sizeof(vec3) + allInOne.maxVerticesCount * sizeof(vec3)};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffer, offsets);
    vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer2D, 0, VK_INDEX_TYPE_UINT16);
    //loading1 png
    drawPic(TEXTURE_LOADING, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);
    //circle
    // drawPic(TEXTURE_CIRCLE, currentFrame, currentCommandBuffer);
    // box
    // drawPic(TEXTURE_BOX, currentFrame, currentCommandBuffer);
    // main font png
    drawPic(TEXTURE_FONT, currentFrame, currentCommandBuffer, allInOne.graphicPipelineLayout);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.particlePipeline);
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, allInOne.pShaderStorageBuffer + currentFrame, offsets);
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.particlePipelineLayout, 0,\
    1, &allInOne.pParticleDescriptorSets[currentFrame], 0, NULL);
    vkCmdDraw(currentCommandBuffer, PARTICLE_COUNT, 1, 0, 0);

    vkCmdEndRenderPass(currentCommandBuffer);

    VkBufferMemoryBarrier bufferBarrierRelease = {};
    _setBufferMemoryBarrier(NULL, VK_ACCESS_SHADER_READ_BIT, 0, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.queueFamilyIndices.computeFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[currentFrame], 0, sizeof(Particle) * PARTICLE_COUNT, &bufferBarrierRelease);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 1, &bufferBarrierRelease, 0, NULL);
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

    setTextureImageLayout(getTexture(TEXTURE_SHADOW), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 0, 1);
}
// static void recordCommandBuffer_Bottom(Uint32 currentFrame, bool bottomMoved)
// {
//     VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

//     VkExtent2D bottomExtent2D = {BOTTOM_WIDTH, BOTTOM_HEIGHT};
//     G_Texture_P * imageArray = getTexture(TEXTURE_MAP_ARRAY);
//     G_Texture_P * tileSetTexture = getTexture(TEXTURE_TILE_SET);

//     DrawHere tempDrawHere = {};
//     Uint32 i = 0;
//     Uint32 count = allInOne.bottomImageDrawStack.top + 1;

//     VkOffset2D offset = {0, 0};
//     VkRect2D renderArea = {offset, bottomExtent2D};

//     VkClearValue clearValue[1];
//     clearValue[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};

//     beginCommandBuffer(currentCommandBuffer);

//     setViewport(bottomExtent2D, currentCommandBuffer);
//     setScissor(bottomExtent2D, currentCommandBuffer);

//     VkDeviceSize singleUVBufferSize = sizeof(vec2) * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D;

//     VkBuffer vertexBuffers[] = {allInOne.tileMapVertexBuffer, allInOne.tileMapTexCoordBuffer[currentFrame]};
//     VkDeviceSize offsets[] = {0, singleUVBufferSize * (count - 1)};

//     while (StackIsEmpty(allInOne.bottomImageDrawStack) == false)
//     {
//         allInOne.bottomImageDrawStack.popFn(&allInOne.bottomImageDrawStack, &tempDrawHere);

//         VkRenderPassBeginInfo renderBeginInfo = {};
//         renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//         renderBeginInfo.pNext = NULL;
//         renderBeginInfo.renderPass = allInOne.offscreenRenderPass;
//         renderBeginInfo.framebuffer = allInOne.pBottomImageArrayFramebuffers[tempDrawHere.BottomID];
//         renderBeginInfo.renderArea = renderArea;
//         renderBeginInfo.clearValueCount = 1;
//         renderBeginInfo.pClearValues = clearValue;

//         vkCmdBeginRenderPass(currentCommandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

//         vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.tilemapPipeline);

//         vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.graphicPipelineLayout, 0, 1, tileSetTexture->pDescriptorSet, 0, NULL);

//         vkCmdBindVertexBuffers(currentCommandBuffer, 0, 2, vertexBuffers, offsets);
//         vkCmdBindIndexBuffer(currentCommandBuffer, allInOne.indexBuffer2D, 0, VK_INDEX_TYPE_UINT16);

//         vkCmdPushConstants(currentCommandBuffer, allInOne.graphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);

//         vkCmdDrawIndexed(currentCommandBuffer, MAX_TILES_IN_GROUP * INDEX_COUNT_IN_UNIT_2D, 1, 0, 0, 0);

//         vkCmdEndRenderPass(currentCommandBuffer);

//         setTextureImageLayout(imageArray, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i, 1);
//         i++;
//         offsets[1] -= singleUVBufferSize;
//     }
// }
static void recordCommandBuffer_3D(Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

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
    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, allInOne.modelPipeline);

    drawModel(TEXTURE_MODEL, currentFrame, currentCommandBuffer);

    // drawModel(TEXTURE_BOTTOM, currentFrame, currentCommandBuffer);

    vkCmdEndRenderPass(currentCommandBuffer);
}
static void recordComputeCommandBuffer(Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pComputeCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

    VkBufferMemoryBarrier bufferMemoryBarrierGet = {};
    _setBufferMemoryBarrier(NULL, 0, VK_ACCESS_SHADER_READ_BIT, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.queueFamilyIndices.computeFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[(currentFrame + 1) % 2], 0, sizeof(Particle) * PARTICLE_COUNT, &bufferMemoryBarrierGet);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 1, &bufferMemoryBarrierGet, 0, NULL);

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.computePipeline);

    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, allInOne.computePipelineLayout, 0, 1, &allInOne.pComputeDescriptorSets[currentFrame], 0, NULL);

    vkCmdDispatch(currentCommandBuffer, PARTICLE_COUNT / 256, 1, 1);

    VkBufferMemoryBarrier bufferMemoryBarrierRelease = {};
    _setBufferMemoryBarrier(NULL, VK_ACCESS_SHADER_WRITE_BIT, 0, allInOne.queueFamilyIndices.computeFamily.familyIndice, allInOne.queueFamilyIndices.graphicsFamily.familyIndice\
    , allInOne.pShaderStorageBuffer[currentFrame], 0, sizeof(Particle) * PARTICLE_COUNT, &bufferMemoryBarrierRelease);
    vkCmdPipelineBarrier(currentCommandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 1, &bufferMemoryBarrierRelease, 0, NULL);
}
static void recordSSGICommandBuffer(Uint32 currentFrame, Uint32 width, Uint32 height)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

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
static void recordCommandBufferCombine(Uint32 imageIndex, Uint32 currentFrame)
{
    VkCommandBuffer currentCommandBuffer = allInOne.pGraphicCommandBuffer[currentFrame];

    beginCommandBuffer(currentCommandBuffer);

    G_Texture_P * ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(currentCommandBuffer, ssgiTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);

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
static void drawFirstScene(Uint32 currentFrame, Uint32 width, Uint32 height, bool bottomMoved)
{
    VkSemaphore * timelineSemaphore2d = allInOne.pTimelineSemaphore2d + currentFrame;
    Uint64 waitValue2D[] = {0, 0};
    vkGetSemaphoreCounterValue(allInOne.device, *timelineSemaphore2d, &waitValue2D[0]);
    Uint64 signalValue2D[] = {1, 0};
    signalValue2D[0] = waitValue2D[0] + 1;

    VkSemaphore * timelineSemaphore3d = allInOne.pTimelineSemaphore3d + currentFrame;
    Uint64 waitValue3D[] = {0, 0};
    vkGetSemaphoreCounterValue(allInOne.device, *timelineSemaphore3d, &waitValue3D[0]);
    Uint64 signalValue3D[] = {1, 0};
    signalValue3D[0] = waitValue3D[0] + 1;

    // particle
    resultVulkan(vkWaitForFences(allInOne.device, 1, &allInOne.pComputeInFlightFence[currentFrame], VK_TRUE, UINT64_MAX), 0);
    vkResetFences(allInOne.device, 1, &allInOne.pComputeInFlightFence[currentFrame]);

    vkResetCommandBuffer(allInOne.pComputeCommandBuffer[currentFrame], 0);
    recordComputeCommandBuffer(currentFrame);
    vkEndCommandBuffer(allInOne.pComputeCommandBuffer[currentFrame]);

    VkSubmitInfo particleSubmitInfo = {};
    setSubmitInfo(NULL, 0, NULL, NULL, 1, allInOne.pComputeCommandBuffer + currentFrame, 1, allInOne.pComputeSemaphore + currentFrame, &particleSubmitInfo);
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    vkQueueSubmit(*allInOne.pComputeQueue, 1, &particleSubmitInfo, allInOne.pComputeInFlightFence[currentFrame]);

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
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue3D;

    VkSubmitInfo submitInfoShadow = {};
    setSubmitInfo(&timelineSemaphoreInfo, 0, NULL, NULL, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore3d, &submitInfoShadow);
    // print("semaphore value: %u", SDL_GetSemaphoreValue(allSync.vertexSemaphore));
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkQueueSubmit(getGraphic3dQueue(), 1, &submitInfoShadow, allInOne.pGraphicInFlightFence[currentFrame]), 0);
    signalValue3D[0]++;

    // if (StackIsEmpty(allInOne.bottomImageDrawStack) == false)
    // {
    //     if (StackIsEmpty(allInOne.bottomImageMoveStack))
    //     {
    //         if (bottomMoved == true)
    //         {
    //             vkWaitForFences(allInOne.device, 1, &allInOne.pTransferInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    //         }
    //         vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    //         vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]);
    //         resultVulkan(vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0), 0);
    //         recordCommandBuffer_Bottom(currentFrame, bottomMoved);
    //         resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);

    //         bottomMoved = false;

    //         waitValue3D[0] = signalValue3D[0] - 1;
    //         timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    //         timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue3D;
    //         timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    //         timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue3D;

    //         VkPipelineStageFlags waitStage_Bottom[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    //         VkSubmitInfo submitInfoBottom = {};
    //         setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore3d, waitStage_Bottom, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore3d, &submitInfoBottom);
    //         resultVulkan(vkQueueSubmit(getGraphic3dQueue(), 1, &submitInfoBottom, allInOne.pGraphicInFlightFence[currentFrame]), 0);
    //         signalValue3D[0]++;
    //     }
    // }
 

    // 3d object
    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    resultVulkan(vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0), 0);
    recordCommandBuffer_3D(currentFrame);
    resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);


    waitValue3D[0] = signalValue3D[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue3D;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue3D;

    VkPipelineStageFlags waitStage_3D[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo3D = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore3d, waitStage_3D, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore3d, &submitInfo3D);
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkQueueSubmit(getGraphic3dQueue(), 1, &submitInfo3D, allInOne.pGraphicInFlightFence[currentFrame]), 0);
    signalValue3D[0]++;
    
    // SSGI
    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0);
    recordSSGICommandBuffer(currentFrame, width, height);
    vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]);

    VkPipelineStageFlags SSGIWaitStage[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};

    waitValue3D[0] = signalValue3D[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue3D;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue3D;

    VkSubmitInfo SSGISubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 1, timelineSemaphore3d, SSGIWaitStage, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore3d, &SSGISubmitInfo);
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    vkQueueSubmit(getGraphic3dQueue(), 1, &SSGISubmitInfo, allInOne.pGraphicInFlightFence[currentFrame]);
    signalValue3D[0]++;

    // 2d and present
    Uint32 imageIndex_3D = 0;
    resultVulkan(vkAcquireNextImageKHR(allInOne.device, allInOne.swapchain3D, UINT64_MAX, allInOne.pImageAvailableSemaphore[currentFrame], NULL, &imageIndex_3D), 0);

    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0);
    recordCommandBuffer2D(imageIndex_3D, currentFrame);
    resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);

    waitValue2D[0] = signalValue2D[0] - 1;

    Uint64 graphic2dWaitValue[] = {waitValue2D[0], waitValue2D[1], 0};
    timelineSemaphoreInfo.waitSemaphoreValueCount = 3;
    timelineSemaphoreInfo.pWaitSemaphoreValues = graphic2dWaitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue2D;

    VkSemaphore graphic2dWaitSemaphores[] = {*timelineSemaphore2d, allInOne.pImageAvailableSemaphore[currentFrame], allInOne.pComputeSemaphore[currentFrame]};
    VkPipelineStageFlagBits graphic2dWaitStage[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};

    VkSubmitInfo combinbeSubmitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 3, graphic2dWaitSemaphores, graphic2dWaitStage, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, timelineSemaphore2d, &combinbeSubmitInfo);
    SDL_WaitSemaphore(allSync.vertexSemaphore);
    vkQueueSubmit(getGraphic2dQueue(), 1, &combinbeSubmitInfo, allInOne.pGraphicInFlightFence[currentFrame]);
    signalValue2D[0]++;

    // combine and present
    vkWaitForFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(allInOne.device, 1, &allInOne.pGraphicInFlightFence[currentFrame]), 0);

    resultVulkan(vkResetCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame], 0), 0);
    recordCommandBufferCombine(imageIndex_3D, currentFrame);
    resultVulkan(vkEndCommandBuffer(allInOne.pGraphicCommandBuffer[currentFrame]), 0);

    Uint64 tempWaitValue[3] = {signalValue2D[0] - 1, signalValue3D[0] - 1, 0};
    Uint64 tempSignalValue[3] = {signalValue2D[0], 0};

    timelineSemaphoreInfo.waitSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pWaitSemaphoreValues = tempWaitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pSignalSemaphoreValues = tempSignalValue;

    VkSemaphore combineWaitSemaphores[] = {*timelineSemaphore2d, *timelineSemaphore3d};
    VkPipelineStageFlags combineWaitStage[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};
    VkSemaphore graphic2dSignalSemaphores[] = {allInOne.pRenderFinishedSemaphore[currentFrame]};

    VkSubmitInfo submitInfo = {};
    setSubmitInfo(&timelineSemaphoreInfo, 2, combineWaitSemaphores, combineWaitStage, 1, allInOne.pGraphicCommandBuffer + currentFrame, 1, graphic2dSignalSemaphores, &submitInfo);
    resultVulkan(vkQueueSubmit(getGraphic2dQueue(), 1, &submitInfo, allInOne.pGraphicInFlightFence[currentFrame]), 0);

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
void drawFrame(Scene scene, Uint32 currentFrame, Uint32 width, Uint32 height, bool bottomMoved)
{
    switch (scene)
    {
        case First_Scene:
        drawFirstScene(currentFrame, width, height, bottomMoved);
        break;
        
        case Pause_Scene:
        break;

        case Menu_Scene:
        break;
    }
}