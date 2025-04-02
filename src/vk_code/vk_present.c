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

static void drawPic(const char * innerName, Uint32 currentFrame)
{
    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL) return;

    vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipelineLayout, 0,
    1, tempTexture->pDescriptorSet + currentFrame, 0, NULL);

    SDL_LockMutex(allSync.renderMutex);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed((*allInOne.ppGraphicCommandBuffer)[currentFrame], tempTexture->offsets[i].count * 6, 1, 0, tempTexture->offsets[i].offset, 0);
        // vkCmdDrawIndexed(CommandBuffer, 6, instanceCount, 0, offset, 0);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
static void recordCommandBuffer_FirstScene()
{
    FuncCode code = recordCommandBufferF;
    uint32_t currentFrame = *allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    resultVulkan(vkBeginCommandBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], &beginInfo), code, 0);
    //printf("record command buffer begin\n");

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)allInOne.pExtent2D->width;
    viewport.height = (float)allInOne.pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    // scissor.extent = (VkExtent2D){200, 200};
    scissor.extent = *allInOne.pExtent2D;

    vkCmdSetScissor((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &scissor);

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

    vkCmdBeginRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipeline);

    vkCmdPushConstants((*allInOne.ppGraphicCommandBuffer)[currentFrame], *allInOne.pGraphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);

    VkBuffer vertexBuffer[] = {(*allInOne.pVertexBuffer2D)[currentFrame]};
    // VkDeviceSize vertexOffsets1[] = {0, allInOne.maxVerticesCount * sizeof(vec3), allInOne.maxVerticesCount * sizeof(vec3) + allInOne.maxVerticesCount * sizeof(vec3)};
    vkCmdBindVertexBuffers((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, vertexBuffer, offsets);

    vkCmdBindIndexBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], (*allInOne.pIndexBuffer2D)[0], 0, VK_INDEX_TYPE_UINT16);

    // tile map
    drawPic(TEXTURE_TILE_SET, currentFrame);

    //loading1 png
    drawPic(TEXTURE_LOADING, currentFrame);

    //circle
    drawPic(TEXTURE_CIRCLE, currentFrame);

    // main font png
    drawPic(TEXTURE_FONT, currentFrame);


    vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipeline);

    vkCmdBindVertexBuffers((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &(*allInOne.ppShaderStorageBuffers)[currentFrame], offsets);

    vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipelineLayout, 0,
    1, &(*allInOne.ppParticleDescriptorSets)[currentFrame], 0, NULL);

    vkCmdDraw((*allInOne.ppGraphicCommandBuffer)[currentFrame], PARTICLE_COUNT, 1, 0, 0);

    vkCmdEndRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame]);
}
static void recordCommandBufferCombine2D(Uint32 imageIndex)
{
    FuncCode code = recordCommandBufferF;
    uint32_t currentFrame = *allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    resultVulkan(vkBeginCommandBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], &beginInfo), code, 0);
    //printf("record command buffer begin\n");

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    // viewport.width = 200;
    // viewport.height = 200;
    viewport.width = (float)allInOne.pExtent2D->width;
    viewport.height = (float)allInOne.pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    // scissor.extent = (VkExtent2D){200, 200};
    scissor.extent = *allInOne.pExtent2D;

    vkCmdSetScissor((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &scissor);

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

    vkCmdBeginRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombine2DPipeline);

    vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombine2DPipelineLayout, 0, 1, (*allInOne.ppCombine2dDescriptorSets) + currentFrame, 0, NULL);

    vkCmdDraw((*allInOne.ppGraphicCommandBuffer)[currentFrame], 6, 1, 0, 0);

    vkCmdEndRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame]);
}
static void drawShadow(const char * innerName, Uint32 currentFrame)
{
    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL) return;

    Uint32 firstInstance, instanceCount;
    getStaticModelDrawInfo(allInOne.pStaticModelPool, &firstInstance, &instanceCount, innerName);

    SDL_LockMutex(allSync.renderMutex);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed((*allInOne.ppGraphicCommandBuffer)[currentFrame], tempTexture->offsets[i].count, instanceCount, 0, tempTexture->offsets[i].offset, firstInstance);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
static void recordCommandBufferShadow(void)
{
    FuncCode code = recordCommandBufferF;
    uint32_t currentFrame = *allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    resultVulkan(vkBeginCommandBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], &beginInfo), code, 0);
    //printf("record command buffer begin\n");

    VkExtent2D shadow = {SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT};

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    // viewport.width = 200;
    // viewport.height = 200;
    viewport.width = (float)shadow.width;
    viewport.height = (float)shadow.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    // scissor.extent = (VkExtent2D){200, 200};
    scissor.extent = shadow; 

    vkCmdSetScissor((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &scissor);

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

    vkCmdBeginRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pShadowPipeline);

    VkBuffer vertex3DBuffer[] = {(*allInOne.pVertexBuffer3D)[currentFrame], allInOne.pStaticModelPool->instanceBuffer[0], allInOne.pStaticModelPool->instanceBuffer[0]};
    VkDeviceSize offsets[] = {0, 0, allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 3, vertex3DBuffer, offsets);

    vkCmdBindIndexBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], (*allInOne.pIndexBuffer3D)[currentFrame], 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pShadowPipelineLayout, 0,
    1, (*allInOne.ppShadowDescriptorSets) + currentFrame, 0, NULL);

    // model
    drawShadow(TEXTURE_BOTTOM, currentFrame);

    drawShadow(TEXTURE_MODEL, currentFrame);

    vkCmdEndRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame]);
}
static void drawModel(const char * innerName, Uint32 currentFrame)
{
    G_Texture_P * tempTexture = getTexture(innerName);
    if (tempTexture == NULL) return;

    Uint32 firstInstance, instanceCount;
    getStaticModelDrawInfo(allInOne.pStaticModelPool, &firstInstance, &instanceCount, innerName);

    vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pModelPipelineLayout, 0,
    1, tempTexture->pDescriptorSet + currentFrame, 0, NULL);

    SDL_LockMutex(allSync.renderMutex);
    
    for (int i = 0;i < tempTexture->refCount;i++)
    {
        vkCmdDrawIndexed((*allInOne.ppGraphicCommandBuffer)[currentFrame], tempTexture->offsets[i].count, instanceCount, 0, tempTexture->offsets[i].offset, firstInstance);
    }

    SDL_UnlockMutex(allSync.renderMutex);
}
static void recordCommandBuffer_3D(void)
{
    FuncCode code = recordCommandBufferF;
    uint32_t currentFrame = *allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    resultVulkan(vkBeginCommandBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], &beginInfo), code, 0);
    //printf("record command buffer begin\n");

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    // viewport.width = 200;
    // viewport.height = 200;
    viewport.width = (float)allInOne.pExtent2D->width;
    viewport.height = (float)allInOne.pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    // scissor.extent = (VkExtent2D){200, 200};
    scissor.extent = *allInOne.pExtent2D;

    vkCmdSetScissor((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &scissor);

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

    vkCmdBeginRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pModelPipeline);

    VkBuffer vertex3DBuffer[] = {(*allInOne.pVertexBuffer3D)[currentFrame], allInOne.pStaticModelPool->instanceBuffer[0], allInOne.pStaticModelPool->instanceBuffer[0]};
    VkDeviceSize offsets[] = {0, 0, allInOne.pStaticModelPool->totalInstanceCount * sizeof(mat4)};
    vkCmdBindVertexBuffers((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 3, vertex3DBuffer, offsets);

    vkCmdBindIndexBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], (*allInOne.pIndexBuffer3D)[currentFrame], 0, VK_INDEX_TYPE_UINT32);

    // model
    drawModel(TEXTURE_BOTTOM, currentFrame);

    drawModel(TEXTURE_MODEL, currentFrame);

    vkCmdEndRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame]);
}
static void recordComputeCommandBuffer(void)
{
    uint32_t * pCurrentFrame = allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo computeBeginInfo;
    computeBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    computeBeginInfo.pNext = NULL;
    computeBeginInfo.flags = 0;
    computeBeginInfo.pInheritanceInfo = NULL;
    
    vkBeginCommandBuffer((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], &computeBeginInfo);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)allInOne.pExtent2D->width;
    viewport.height = (float)allInOne.pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    // scissor.extent = (VkExtent2D){200, 200};
    scissor.extent = *allInOne.pExtent2D;

    vkCmdSetScissor((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], 0, 1, &scissor);

    vkCmdBindPipeline((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pComputePipeline);

    vkCmdBindDescriptorSets((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pComputePipelineLayout, 0, 1, &(*allInOne.ppComputeDescriptorSets)[*pCurrentFrame], 0, NULL);

    vkCmdDispatch((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT / 256, 1, 1);
}
static void recordSSGICommandBuffer(void)
{
    uint32_t * pCurrentFrame = allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo computeBeginInfo;
    computeBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    computeBeginInfo.pNext = NULL;
    computeBeginInfo.flags = 0;
    computeBeginInfo.pInheritanceInfo = NULL;
    
    vkBeginCommandBuffer((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], &computeBeginInfo);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)allInOne.pExtent2D->width;
    viewport.height = (float)allInOne.pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    // scissor.extent = (VkExtent2D){200, 200};
    scissor.extent = *allInOne.pExtent2D;

    vkCmdSetScissor((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], 0, 1, &scissor);

    vkCmdBindPipeline((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pSSGIPipeline);

    VkDescriptorSet descriptorSets[] = {(*allInOne.ppSSGIDescriptorSets)[*pCurrentFrame], (*allInOne.ppSSGIDescriptorSets + 2)[*pCurrentFrame]};
    vkCmdBindDescriptorSets((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pSSGIPipelineLayout, 0, 2, descriptorSets, 0, NULL);

    vkCmdDispatch((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], (allInOne.pExtent2D->width + 8 - 1) / 8, (allInOne.pExtent2D->height + 8 - 1) / 8, 1);
}
static void recordCommandBufferCombine(Uint32 imageIndex)
{
    FuncCode code = recordCommandBufferF;
    uint32_t currentFrame = *allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    resultVulkan(vkBeginCommandBuffer((*allInOne.ppGraphicCommandBuffer)[currentFrame], &beginInfo), code, 0);
    //printf("record command buffer begin\n");

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    // viewport.width = 200;
    // viewport.height = 200;
    viewport.width = (float)allInOne.pExtent2D->width;
    viewport.height = (float)allInOne.pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    // scissor.extent = (VkExtent2D){200, 200};
    scissor.extent = *allInOne.pExtent2D;

    vkCmdSetScissor((*allInOne.ppGraphicCommandBuffer)[currentFrame], 0, 1, &scissor);

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

    vkCmdBeginRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombinePipeline);

    vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pCombinePipelineLayout, 0, 1, (*allInOne.ppCombineDescriptorSets) + currentFrame, 0, NULL);

    vkCmdDraw((*allInOne.ppGraphicCommandBuffer)[currentFrame], 6, 1, 0, 0);

    vkCmdEndRenderPass((*allInOne.ppGraphicCommandBuffer)[currentFrame]);
}
static void drawFirstScene(void)
{
    VkSemaphore * timelineSemaphore = (*allInOne.ppTimelineSemaphore1) + *allInOne.pCurrentFrame;
    Uint64 waitValue[] = {0, 0};
    vkGetSemaphoreCounterValue(*allInOne.pDevice, *timelineSemaphore, &waitValue[0]);
    Uint64 signalValue[] = {1, 0};
    signalValue[0] = waitValue[0] + 1;

    // shadow
    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), resetFencesF, 0);
    //printf("reset fences\n");

    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), resetCommandBufferF, 0);
    recordCommandBufferShadow();
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), endCommandBufferF, 0);

    SDL_WaitSemaphore(allSync.vertexSemaphore);

    VkTimelineSemaphoreSubmitInfo timelineSemaphoreInfo = {};
    timelineSemaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timelineSemaphoreInfo.pNext = NULL;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo submitInfoShadow = {};
    submitInfoShadow.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfoShadow.pNext = &timelineSemaphoreInfo;
    submitInfoShadow.waitSemaphoreCount = 0;
    submitInfoShadow.pWaitSemaphores = NULL;
    submitInfoShadow.pWaitDstStageMask = NULL;
    submitInfoShadow.commandBufferCount = 1;
    submitInfoShadow.pCommandBuffers = &(*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame];
    submitInfoShadow.signalSemaphoreCount = 1;
    submitInfoShadow.pSignalSemaphores = timelineSemaphore;
    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfoShadow, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), queueSumbitF, 0);
    signalValue[0]++;


    G_Texture_P * shadowTexture = getTexture(TEXTURE_SHADOW);
    transitionImageLayout(&shadowTexture->image, shadowTexture->format, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    // 3d object
    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), resetFencesF, 0);
    //printf("reset fences\n");

    //printf("acquire next image index\n"); 
    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), resetCommandBufferF, 0);
    recordCommandBuffer_3D();
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), endCommandBufferF, 0);

    // VkSemaphore waitSemaphore_3D[] = {(*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame]};
    VkPipelineStageFlags waitStage_3D[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo submitInfo_3D = {};
    submitInfo_3D.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo_3D.pNext = &timelineSemaphoreInfo;
    submitInfo_3D.waitSemaphoreCount = 1;
    submitInfo_3D.pWaitSemaphores = timelineSemaphore;
    submitInfo_3D.pWaitDstStageMask = waitStage_3D;
    submitInfo_3D.commandBufferCount = 1;
    submitInfo_3D.pCommandBuffers = &(*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame];
    submitInfo_3D.signalSemaphoreCount = 1;
    submitInfo_3D.pSignalSemaphores = timelineSemaphore;
    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfo_3D, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), queueSumbitF, 0);
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
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), resetFencesF, 0);

    // resultVulkan(vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX), waitForFencesF, 0);
    // vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame]);

    vkResetCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame], 0);
    recordSSGICommandBuffer();
    vkEndCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame]);

    VkPipelineStageFlags SSGIWaitStage[] = {VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};
    VkSemaphore SSGISignaleSemaphore[] = {*timelineSemaphore, (*allInOne.ppRenderFinishedSemaphore)[*allInOne.pCurrentFrame]};

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo SSGISubmitInfo = {};
    SSGISubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SSGISubmitInfo.pNext = &timelineSemaphoreInfo;
    SSGISubmitInfo.waitSemaphoreCount = 1;
    SSGISubmitInfo.pWaitSemaphores = timelineSemaphore;
    SSGISubmitInfo.pWaitDstStageMask = SSGIWaitStage;
    SSGISubmitInfo.commandBufferCount = 1;
    SSGISubmitInfo.pCommandBuffers = &(*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame];
    SSGISubmitInfo.signalSemaphoreCount = 1;
    SSGISubmitInfo.pSignalSemaphores = SSGISignaleSemaphore;
    vkQueueSubmit(*allInOne.pComputeQueue, 1, &SSGISubmitInfo, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]);
    signalValue[0]++;

    ssgiTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    transitionImageLayout(&ssgiTexture->image, ssgiTexture->format, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // combine and present
    Uint32 imageIndex_3D = 0;
    resultVulkan(vkAcquireNextImageKHR(*allInOne.pDevice, *allInOne.pSwapchain3D, UINT64_MAX, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame], NULL, &imageIndex_3D), acquireNextImageF, 0);

    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), resetFencesF, 0);

    vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0);
    recordCommandBufferCombine(imageIndex_3D);
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), endCommandBufferF, 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSemaphore combineWaitSemaphores[] = {*timelineSemaphore, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame]};
    VkPipelineStageFlags combineWaitStage[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore combineSignalSemaphores[] = {*timelineSemaphore, (*allInOne.ppRenderFinishedSemaphore)[*allInOne.pCurrentFrame]};

    VkSubmitInfo combinbeSubmitInfo = {};
    combinbeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    combinbeSubmitInfo.pNext = &timelineSemaphoreInfo;
    combinbeSubmitInfo.waitSemaphoreCount = 2;
    combinbeSubmitInfo.pWaitSemaphores = combineWaitSemaphores;
    combinbeSubmitInfo.pWaitDstStageMask = combineWaitStage;
    combinbeSubmitInfo.commandBufferCount = 1;
    combinbeSubmitInfo.pCommandBuffers = &(*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame];
    combinbeSubmitInfo.signalSemaphoreCount = 2;
    combinbeSubmitInfo.pSignalSemaphores = combineSignalSemaphores;
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
    resultVulkan(vkQueuePresentKHR(*allInOne.pPresentQueue, &presentInfo_3D), queuePresentF, 0);


    // particle
    resultVulkan(vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX), waitForFencesF, 0);
    vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame]);

    vkResetCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame], 0);
    recordComputeCommandBuffer();
    vkEndCommandBuffer((*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame]);

    timelineSemaphoreInfo.waitSemaphoreValueCount = 0;
    timelineSemaphoreInfo.pWaitSemaphoreValues = NULL;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo computeSubmitInfo = {};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.pNext = &timelineSemaphoreInfo;
    computeSubmitInfo.waitSemaphoreCount = 0;
    computeSubmitInfo.pWaitSemaphores = NULL;
    computeSubmitInfo.pWaitDstStageMask = NULL;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &(*allInOne.ppComputeCommandBuffer)[*allInOne.pCurrentFrame];
    computeSubmitInfo.signalSemaphoreCount = 1;
    computeSubmitInfo.pSignalSemaphores = timelineSemaphore;
    vkQueueSubmit(*allInOne.pComputeQueue, 1, &computeSubmitInfo, (*allInOne.ppComputeInFlightFence)[*allInOne.pCurrentFrame]);
    signalValue[0]++;


    // 2d and present
    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), resetFencesF, 0);
    //printf("reset fences\n");

    //printf("acquire next image index\n"); 
    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), resetCommandBufferF, 0);
    //printf("reset command bufer\n");
    recordCommandBuffer_FirstScene();
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), endCommandBufferF, 0);

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 1;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkPipelineStageFlagBits graphic2dWaitStage[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = &timelineSemaphoreInfo;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = timelineSemaphore;
    submitInfo.pWaitDstStageMask = graphic2dWaitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = timelineSemaphore;


    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfo, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), queueSumbitF, 0);
    signalValue[0]++;


    G_Texture_P * shadow2dTexture = getTexture(TEXTURE_SHADOW_MAP);
    G_Texture_P * graphic2dTexture = getTexture(TEXTURE_2D_COLOR);
    transitionImageLayout(&shadow2dTexture->image, shadow2dTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    transitionImageLayout(&graphic2dTexture->image, graphic2dTexture->format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    uint32_t imageIndex;
    resultVulkan(vkAcquireNextImageKHR(*allInOne.pDevice, *allInOne.pSwapchain2D, UINT64_MAX, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame], NULL, &imageIndex), acquireNextImageF, 0);

    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame], VK_TRUE, UINT64_MAX);
    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), resetFencesF, 0);

    resultVulkan(vkResetCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame], 0), resetCommandBufferF, 0);
    //printf("reset command bufer\n");
    recordCommandBufferCombine2D(imageIndex);
    resultVulkan(vkEndCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame]), endCommandBufferF, 0);

    VkSemaphore waitSemaphore[2] = {*timelineSemaphore, (*allInOne.ppImageAvailableSemaphore)[*allInOne.pCurrentFrame]};
    VkPipelineStageFlags waitStage[2] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore signalSemaphore[2] = {*timelineSemaphore, (*allInOne.ppRenderFinishedSemaphore)[*allInOne.pCurrentFrame]};

    waitValue[0] = signalValue[0] - 1;
    timelineSemaphoreInfo.waitSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pWaitSemaphoreValues = waitValue;
    timelineSemaphoreInfo.signalSemaphoreValueCount = 2;
    timelineSemaphoreInfo.pSignalSemaphoreValues = signalValue;

    VkSubmitInfo combineSubmitInfo = {};
    combineSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    combineSubmitInfo.pNext = &timelineSemaphoreInfo;
    combineSubmitInfo.waitSemaphoreCount = 2;
    combineSubmitInfo.pWaitSemaphores = waitSemaphore;
    combineSubmitInfo.pWaitDstStageMask = waitStage;
    combineSubmitInfo.commandBufferCount = 1;
    combineSubmitInfo.pCommandBuffers = &(*allInOne.ppGraphicCommandBuffer)[*allInOne.pCurrentFrame];
    combineSubmitInfo.signalSemaphoreCount = 2;
    combineSubmitInfo.pSignalSemaphores = signalSemaphore;

    SDL_WaitSemaphore(allSync.vertexSemaphore);
    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &combineSubmitInfo, (*allInOne.ppGraphicInFlightFence)[*allInOne.pCurrentFrame]), queueSumbitF, 0);
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
    resultVulkan(vkQueuePresentKHR(*allInOne.pPresentQueue, &presentInfo), queuePresentF, 0);
    //printf("present queue\n");
}
static void recoreCommandBuffer_MenuScene(void)
{
    // FuncCode code = recordCommandBufferF;
    // uint32_t * pCurrentFrame = allInOne.pCurrentFrame;

    // VkCommandBufferBeginInfo beginInfo = {};
    // beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // beginInfo.pNext = NULL;
    // beginInfo.flags = 0;
    // beginInfo.pInheritanceInfo = NULL;

    // resultVulkan(vkBeginCommandBuffer((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], &beginInfo), code, 0);
    // //printf("record command buffer begin\n");

    // VkViewport viewport = {};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float)allInOne.pExtent2D->width;
    // viewport.height = (float)allInOne.pExtent2D->height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;

    // vkCmdSetViewport((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], 0, 1, &viewport);

    // VkRect2D scissor = {};
    // scissor.offset = (VkOffset2D){0, 0};
    // // scissor.extent = (VkExtent2D){200, 200};
    // scissor.extent = *allInOne.pExtent2D;

    // vkCmdSetScissor((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], 0, 1, &scissor);

    // VkOffset2D offset = {0, 0};
    // VkRect2D renderArea = {offset, *allInOne.pExtent2D};

    // VkClearValue clearValue[2];
    // clearValue[0].color= (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    // clearValue[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    // VkRenderPassBeginInfo renderBeginInfo = {};
    // renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    // renderBeginInfo.pNext = NULL;
    // renderBeginInfo.renderPass = *allInOne.pRenderPass;
    // renderBeginInfo.framebuffer = (*allInOne.ppSwapchainFramebuffer)[imageIndex];
    // renderBeginInfo.renderArea = renderArea;
    // renderBeginInfo.clearValueCount = 2;
    // renderBeginInfo.pClearValues = clearValue;

    // vkCmdBeginRenderPass((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // VkDeviceSize offsets[] = {0};

    // vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipeline);

    // vkCmdBindVertexBuffers((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], 0, 1, &(*allInOne.ppShaderStorageBuffers)[*pCurrentFrame], offsets);

    // vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipelineLayout, 0,
    // 1, &(*allInOne.ppParticleDescriptorSets)[*pCurrentFrame], 0, NULL);

    // vkCmdDraw((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT, 1, 0, 0);

    // vkCmdBindPipeline((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipeline);

    // vkCmdPushConstants((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], *allInOne.pGraphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), allInOne.pPushConstants);

    // VkBuffer vertexBuffer[] = {*allInOne.pVertexBuffer, *allInOne.pVertexBuffer, *allInOne.pVertexBuffer};
    // VkDeviceSize vertexOffsets1[] = {0, allInOne.maxVerticesCount * sizeof(vec3), allInOne.maxVerticesCount * sizeof(vec3) + allInOne.maxVerticesCount * sizeof(vec3)};
    // //loading1 png
    // vkCmdBindVertexBuffers((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], 0, 3, vertexBuffer, vertexOffsets1);

    // vkCmdBindIndexBuffer((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], *allInOne.pIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

    // vkCmdBindDescriptorSets((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipelineLayout, 0,
    // 1, &(*allInOne.ppGraphicDescriptorSets)[*pCurrentFrame], 0, NULL);

    // vkCmdDrawIndexed((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame], 6, 1, 0, 0, 0);

    // vkCmdEndRenderPass((*allInOne.ppGraphicCommandBuffer)[*pCurrentFrame]);
}
static void drawMenuScene(void)
{
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
        drawMenuScene();
        break;
    }
}


/*void updateUniformBuffer(uint32_t currentImage, VkExtent2D * pExtent2D, UniformBufferObject * pUbo, void *** pUniformBuffersMapped, float camera_X, float camera_Y, ComputeUniformBufferObject * pComputeUbo, void *** pppComputeUniformBufferMapped, float deltaTime)
{
    //printf("time: %.2f\n", time);

    glm_mat4_identity(pUbo->model);
    //glm_rotate(pUbo->model, time * glm_rad(90.0f), (vec3){0.0f, 0.0f, 1.0f});

    glm_lookat((vec3){camera_X, camera_Y, 1.5f}, (vec3){camera_X, camera_Y, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, pUbo->view);

    float aspect = (float)pExtent2D->width / pExtent2D->height;
    //printf("aspect : %.2f\n", aspect);
    glm_mat4_identity(pUbo->proj);
    //glm_perspective(glm_rad(45.0f), aspect, 0.1f, 10.0f, pUbo->proj);
    glm_ortho_vulkan(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f, pUbo->proj);

    pUbo->proj[1][1] *= -1;

    memcpy((*pUniformBuffersMapped)[currentImage], pUbo, sizeof(UniformBufferObject));

    pComputeUbo->deltaTime = deltaTime;

    //printf("time: %f, preTime: %f\n", time, preTime);
    //printf("delta time = %f\n", pComputeUbo->deltaTime);

    memcpy((*pppComputeUniformBufferMapped)[currentImage], pComputeUbo, sizeof(ComputeUniformBufferObject));
}*/