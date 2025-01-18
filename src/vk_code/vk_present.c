#include "G_constants.h"

#include "vk_code_h/vk_present.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"

#include "G_log.h"

extern bool pictureMove[4];
extern bool scale;

extern VK_ALL allInOne;

static void recordCommandBuffer_FirstScene(uint32_t imageIndex)
{
    FuncCode code = recordCommandBufferF;
    uint32_t * pCurrentFrame = allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    resultVulkan(vkBeginCommandBuffer((*allInOne.ppCommandBuffer)[*pCurrentFrame], &beginInfo), code, 0);
    //printf("record command buffer begin\n");

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, *allInOne.pExtent2D};

    VkClearValue clearValue[2];
    clearValue[0].color= (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = NULL;
    renderBeginInfo.renderPass = *allInOne.pRenderPass;
    renderBeginInfo.framebuffer = (*allInOne.ppSwapchainFramebuffer)[imageIndex];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 2;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass((*allInOne.ppCommandBuffer)[*pCurrentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    //printf("render pass begin\n");

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)allInOne.pExtent2D->width;
    viewport.height = (float)allInOne.pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*allInOne.ppCommandBuffer)[*pCurrentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = *allInOne.pExtent2D;

    vkCmdSetScissor((*allInOne.ppCommandBuffer)[*pCurrentFrame], 0, 1, &scissor);
    
    //printf("commandBuffer width: %u, height: %u\n", allInOne.pExtent2D->width, allInOne.pExtent2D->height);
    /*size_t bufferSize = sizeof((*allInOne.ppVertices)[0]) * *allInOne.pVerticesCount;
    //copyBuffer(allInOne.pMovingStagingBuffer, allInOne.pVertexBuffer, bufferSize, allInOne.pDevice, allInOne.pSwapchainCommandPool, allInOne.pGraphicQueue);
    memcpy(*allInOne.ppVertexBufferMemMapped, *allInOne.ppVertices, bufferSize);*/
    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline((*allInOne.ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipeline);

    vkCmdBindVertexBuffers((*allInOne.ppCommandBuffer)[*pCurrentFrame], 0, 1, &(*allInOne.ppShaderStorageBuffers)[*pCurrentFrame], offsets);

    vkCmdBindDescriptorSets((*allInOne.ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pParticlePipelineLayout, 0,
    1, &(*allInOne.ppParticleDescriptorSets)[*pCurrentFrame], 0, NULL);

    vkCmdDraw((*allInOne.ppCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT, 1, 0, 0);

    vkCmdBindPipeline((*allInOne.ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipeline);

    vkCmdPushConstants((*allInOne.ppCommandBuffer)[*pCurrentFrame], *allInOne.pGraphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ImageRotate), allInOne.pImageRotate);

    VkBuffer vertexBuffer[] = {*allInOne.pVertexBuffer};
    vkCmdBindVertexBuffers((*allInOne.ppCommandBuffer)[*pCurrentFrame], 0, 1, vertexBuffer, offsets);

    //vkCmdBindVertexBuffers((*allInOne.ppCommandBuffer)[*pCurrentFrame], 2, 1, &(*allInOne.ppShaderStorageBuffers)[*pCurrentFrame], offsets);

    vkCmdBindIndexBuffer((*allInOne.ppCommandBuffer)[*pCurrentFrame], *allInOne.pIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets((*allInOne.ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *allInOne.pGraphicPipelineLayout, 0,
    1, &(*allInOne.ppGraphicDescriptorSets)[*pCurrentFrame], 0, NULL);

    //vkCmdDraw((*allInOne.ppCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT, 1, 0, 0);

    vkCmdDrawIndexed((*allInOne.ppCommandBuffer)[*pCurrentFrame], 12600, 1, 0, 0, 0);

    vkCmdEndRenderPass((*allInOne.ppCommandBuffer)[*pCurrentFrame]);
}
static void recordComputeCommandBuffer(void)
{
    uint32_t * pCurrentFrame = allInOne.pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        0,
        NULL
    };
    
    vkBeginCommandBuffer((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], &beginInfo);

    vkCmdBindPipeline((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pComputePipeline);

    vkCmdBindDescriptorSets((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *allInOne.pComputePipelineLayout, 0, 1, &(*allInOne.ppComputeDescriptorSets)[*pCurrentFrame], 0, NULL);

    vkCmdDispatch((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT / 256, 1, 1);
}
static void drawFirstScene(void)
{
    uint32_t * pCurrentFrame = allInOne.pCurrentFrame;

    resultVulkan(vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*pCurrentFrame], VK_TRUE, UINT64_MAX), waitForFencesF, 0);

    vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppComputeInFlightFence)[*pCurrentFrame]);

    recordComputeCommandBuffer();
    vkEndCommandBuffer((*allInOne.ppComputeCommandBuffer)[*pCurrentFrame]);

    VkSubmitInfo computeSubmitInfo = {};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.pNext = NULL;
    computeSubmitInfo.waitSemaphoreCount = 0;
    computeSubmitInfo.pWaitSemaphores = NULL;
    computeSubmitInfo.pWaitDstStageMask = NULL;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &(*allInOne.ppComputeCommandBuffer)[*pCurrentFrame];
    computeSubmitInfo.signalSemaphoreCount = 1;
    computeSubmitInfo.pSignalSemaphores = &(*allInOne.ppComputeFinishedSemaphore)[*pCurrentFrame];

    vkQueueSubmit(*allInOne.pComputeQueue, 1, &computeSubmitInfo, (*allInOne.ppComputeInFlightFence)[*pCurrentFrame]);

    vkWaitForFences(*allInOne.pDevice, 1, &(*allInOne.ppInFlightFence)[*pCurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    resultVulkan(vkAcquireNextImageKHR(*allInOne.pDevice, *allInOne.pSwapchain, UINT64_MAX, (*allInOne.ppImageAvailableSemaphore)[*pCurrentFrame], NULL, &imageIndex), acquireNextImageF, 0);
    //printf("acquire next image index\n"); 

    resultVulkan(vkResetFences(*allInOne.pDevice, 1, &(*allInOne.ppInFlightFence)[*pCurrentFrame]), resetFencesF, 0);
    //printf("reset fences\n");

    resultVulkan(vkResetCommandBuffer((*allInOne.ppCommandBuffer)[*pCurrentFrame], 0), resetCommandBufferF, 0);
    //printf("reset command bufer\n");

    recordCommandBuffer_FirstScene(imageIndex);
    resultVulkan(vkEndCommandBuffer((*allInOne.ppCommandBuffer)[*pCurrentFrame]), endCommandBufferF, 0);
    //printf("end command buffer\n");

    VkSemaphore waitSemaphore[2] = {(*allInOne.ppComputeFinishedSemaphore)[*pCurrentFrame], (*allInOne.ppImageAvailableSemaphore)[*pCurrentFrame]};
    VkPipelineStageFlags waitStage[2] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore signalSemaphore[1] = {(*allInOne.ppRenderFinishedSemaphore)[*pCurrentFrame]};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 2;
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(*allInOne.ppCommandBuffer)[*pCurrentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphore;

    resultVulkan(vkQueueSubmit(*allInOne.pGraphicQueue, 1, &submitInfo, (*allInOne.ppInFlightFence)[*pCurrentFrame]), queueSumbitF, 0);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &(*allInOne.ppRenderFinishedSemaphore)[*pCurrentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = allInOne.pSwapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL;

    resultVulkan(vkQueuePresentKHR(*allInOne.pPresentQueue, &presentInfo), queuePresentF, 0);
    //printf("present queue\n");

    *pCurrentFrame = (*pCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
static void drawMenuScene(void)
{
    uint32_t * pCurrentFrame = allInOne.pCurrentFrame;
    // logMessage("in menu");
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