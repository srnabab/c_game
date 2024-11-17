#include "vk_present.h"
#include "vk_move.h"
#include "vk_buffer.h"
//#include <windows.h>

extern bool pictureMove[4];
extern bool scale;

void recordCommandBuffer(VK_ALL * pAllInOne, uint32_t imageIndex)
{
    FuncCode code = recordCommandBufferF;
    uint32_t * pCurrentFrame = pAllInOne->pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        VK_NULL_HANDLE,
        0,
        VK_NULL_HANDLE
    };

    resultVulkan(vkBeginCommandBuffer((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], &beginInfo), code, 0);
    //printf("record command buffer begin\n");

    VkOffset2D offset = {0, 0};
    VkRect2D renderArea = {offset, *pAllInOne->pExtent2D};

    VkClearValue clearValue[2];
    clearValue[0].color= (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValue[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderBeginInfo = {};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.pNext = VK_NULL_HANDLE;
    renderBeginInfo.renderPass = *pAllInOne->pRenderPass;
    renderBeginInfo.framebuffer = (*pAllInOne->ppSwapchainFramebuffer)[imageIndex];
    renderBeginInfo.renderArea = renderArea;
    renderBeginInfo.clearValueCount = 2;
    renderBeginInfo.pClearValues = clearValue;

    vkCmdBeginRenderPass((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    //printf("render pass begin\n");

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)pAllInOne->pExtent2D->width;
    viewport.height = (float)pAllInOne->pExtent2D->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = *pAllInOne->pExtent2D;

    vkCmdSetScissor((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], 0, 1, &scissor);
    
    //printf("commandBuffer width: %u, height: %u\n", pAllInOne->pExtent2D->width, pAllInOne->pExtent2D->height);
    /*size_t bufferSize = sizeof((*pAllInOne->ppVertices)[0]) * *pAllInOne->pVerticesCount;
    //copyBuffer(pAllInOne->pMovingStagingBuffer, pAllInOne->pVertexBuffer, bufferSize, pAllInOne->pDevice, pAllInOne->pSwapchainCommandPool, pAllInOne->pGraphicQueue);
    memcpy(*pAllInOne->ppVertexBufferMemMapped, *pAllInOne->ppVertices, bufferSize);*/
    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *pAllInOne->pParticlePipeline);

    vkCmdBindVertexBuffers((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], 0, 1, &(*pAllInOne->ppShaderStorageBuffers)[*pCurrentFrame], offsets);

    vkCmdBindDescriptorSets((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *pAllInOne->pParticlePipelineLayout, 0,
    1, &(*pAllInOne->ppParticleDescriptorSets)[*pCurrentFrame], 0, VK_NULL_HANDLE);

    vkCmdDraw((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT, 1, 0, 0);

    vkCmdBindPipeline((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *pAllInOne->pGraphicPipeline);

    vkCmdPushConstants((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], *pAllInOne->pGraphicPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ImageRotate), pAllInOne->pImageRotate);

    VkBuffer vertexBuffer[] = {*pAllInOne->pVertexBuffer};
    vkCmdBindVertexBuffers((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], 0, 1, vertexBuffer, offsets);

    //vkCmdBindVertexBuffers((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], 2, 1, &(*pAllInOne->ppShaderStorageBuffers)[*pCurrentFrame], offsets);

    vkCmdBindIndexBuffer((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], *pAllInOne->pIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, *pAllInOne->pGraphicPipelineLayout, 0,
    1, &(*pAllInOne->ppGraphicDescriptorSets)[*pCurrentFrame], 0, VK_NULL_HANDLE);

    //vkCmdDraw((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT, 1, 0, 0);

    vkCmdDrawIndexed((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], 12600, 1, 0, 0, 0);

    vkCmdEndRenderPass((*pAllInOne->ppCommandBuffer)[*pCurrentFrame]);
}
void recordComputeCommandBuffer(VK_ALL * pAllInOne)
{
    uint32_t * pCurrentFrame = pAllInOne->pCurrentFrame;

    VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        VK_NULL_HANDLE,
        0,
        VK_NULL_HANDLE
    };
    
    vkBeginCommandBuffer((*pAllInOne->ppComputeCommandBuffer)[*pCurrentFrame], &beginInfo);

    vkCmdBindPipeline((*pAllInOne->ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *pAllInOne->pComputePipeline);

    vkCmdBindDescriptorSets((*pAllInOne->ppComputeCommandBuffer)[*pCurrentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, *pAllInOne->pComputePipelineLayout, 0, 1, &(*pAllInOne->ppComputeDescriptorSets)[*pCurrentFrame], 0, VK_NULL_HANDLE);

    vkCmdDispatch((*pAllInOne->ppComputeCommandBuffer)[*pCurrentFrame], PARTICLE_COUNT / 256, 1, 1);
}
void drawFrame(VK_ALL * pAllInOne)
{
    // FuncCode code = drawFrameF;
    uint32_t * pCurrentFrame = pAllInOne->pCurrentFrame;
    //printf("draw width: %u, height: %u\n", pAllInOne->pExtent2D->width, pAllInOne->pExtent2D->height);
    //printf("pDevice: %p\n", pAllInOne->pDevice);


    //printf("sema: %p, %p, %p\n", &(*pAllInOne->ppComputeFinishedSemaphore)[*pCurrentFrame], &(*pAllInOne->ppImageAvailableSemaphore)[*pCurrentFrame], &(*pAllInOne->ppParticleRenderFinishedSemaphore)[*pCurrentFrame]);
    
    resultVulkan(vkWaitForFences(*pAllInOne->pDevice, 1, &(*pAllInOne->ppComputeInFlightFence)[*pCurrentFrame], VK_TRUE, UINT64_MAX), waitForFencesF, 0);

    vkResetFences(*pAllInOne->pDevice, 1, &(*pAllInOne->ppComputeInFlightFence)[*pCurrentFrame]);

    recordComputeCommandBuffer(pAllInOne);
    vkEndCommandBuffer((*pAllInOne->ppComputeCommandBuffer)[*pCurrentFrame]);

    VkSubmitInfo computeSubmitInfo = {};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.pNext = VK_NULL_HANDLE;
    computeSubmitInfo.waitSemaphoreCount = 0;
    computeSubmitInfo.pWaitSemaphores = VK_NULL_HANDLE;
    computeSubmitInfo.pWaitDstStageMask = VK_NULL_HANDLE;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &(*pAllInOne->ppComputeCommandBuffer)[*pCurrentFrame];
    computeSubmitInfo.signalSemaphoreCount = 1;
    computeSubmitInfo.pSignalSemaphores = &(*pAllInOne->ppComputeFinishedSemaphore)[*pCurrentFrame];

    vkQueueSubmit(*pAllInOne->pComputeQueue, 1, &computeSubmitInfo, (*pAllInOne->ppComputeInFlightFence)[*pCurrentFrame]);

    vkWaitForFences(*pAllInOne->pDevice, 1, &(*pAllInOne->ppInFlightFence)[*pCurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    resultVulkan(vkAcquireNextImageKHR(*pAllInOne->pDevice, *pAllInOne->pSwapchain, UINT64_MAX, (*pAllInOne->ppImageAvailableSemaphore)[*pCurrentFrame], VK_NULL_HANDLE, &imageIndex), acquireNextImageF, 0);
    //printf("acquire next image index\n"); 

    resultVulkan(vkResetFences(*pAllInOne->pDevice, 1, &(*pAllInOne->ppInFlightFence)[*pCurrentFrame]), resetFencesF, 0);
    //printf("reset fences\n");

    resultVulkan(vkResetCommandBuffer((*pAllInOne->ppCommandBuffer)[*pCurrentFrame], 0), resetCommandBufferF, 0);
    //printf("reset command bufer\n");

    recordCommandBuffer(pAllInOne, imageIndex);
    resultVulkan(vkEndCommandBuffer((*pAllInOne->ppCommandBuffer)[*pCurrentFrame]), endCommandBufferF, 0);
    //printf("end command buffer\n");

    VkSemaphore waitSemaphore[2] = {(*pAllInOne->ppComputeFinishedSemaphore)[*pCurrentFrame], (*pAllInOne->ppImageAvailableSemaphore)[*pCurrentFrame]};
    VkPipelineStageFlags waitStage[2] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore signalSemaphore[1] = {(*pAllInOne->ppRenderFinishedSemaphore)[*pCurrentFrame]};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = VK_NULL_HANDLE;
    submitInfo.waitSemaphoreCount = 2;
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(*pAllInOne->ppCommandBuffer)[*pCurrentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphore;

    resultVulkan(vkQueueSubmit(*pAllInOne->pGraphicQueue, 1, &submitInfo, (*pAllInOne->ppInFlightFence)[*pCurrentFrame]), queueSumbitF, 0);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = VK_NULL_HANDLE;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &(*pAllInOne->ppRenderFinishedSemaphore)[*pCurrentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = pAllInOne->pSwapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = VK_NULL_HANDLE;

    resultVulkan(vkQueuePresentKHR(*pAllInOne->pPresentQueue, &presentInfo), queuePresentF, 0);
    //printf("present queue\n");

    *pCurrentFrame = (*pCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
void updatePosition(float x, float y, VkExtent2D * pExtent2D, Vertex ** ppVertices, uint32_t pictureSequence)
{
    uint32_t startNumber = pictureSequence * 4;

    float NDCx = x / (pExtent2D->height / 2);
    float NDCy = y / (pExtent2D->height / 2);

    float offSetX = NDCx - (*ppVertices)[startNumber].pos[0];
    float offSetY = NDCy - (*ppVertices)[startNumber].pos[1];

    (*ppVertices)[startNumber].pos[0] += offSetX;
    (*ppVertices)[startNumber].pos[1] += offSetY;

    (*ppVertices)[startNumber + 1].pos[0] += offSetX;
    (*ppVertices)[startNumber + 1].pos[1] += offSetY;

    (*ppVertices)[startNumber + 2].pos[0] += offSetX;
    (*ppVertices)[startNumber + 2].pos[1] += offSetY;

    (*ppVertices)[startNumber + 3].pos[0] += offSetX;
    (*ppVertices)[startNumber + 3].pos[1] += offSetY;
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