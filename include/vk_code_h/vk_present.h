#include "vk_all.h"

#ifndef VK_PRESENT_H
#define VK_PRESENT_H

void recordCommandBuffer(VK_ALL * pAllInOne, uint32_t imageIndex);
void recordComputeCommandBuffer(VK_ALL * pAllInOne);
void updateUniformBuffer(uint32_t currentImage, VkExtent2D * pExtent2D, UniformBufferObject * pUbo, void *** pUniformBuffersMapped, float camera_X, float camera_Y, ComputeUniformBufferObject * pComputeUbo, void *** pppComputeUniformBufferMapped, float deltaTime);
void updateComputeUniformBuffer(uint32_t currentImage, ComputeUniformBufferObject * pUbo, void *** pUniformBuffersMapped);
void drawFrame(VK_ALL * pAllInOne);
void updatePosition(float x, float y, VkExtent2D * pExtent2D, Vertex ** ppVertices, uint32_t pictureSequence);
CGLM_INLINE void glm_ortho_vulkan(float left, float right, float bottom, float top, float nearZ, float farZ, mat4 dest)
{
    float rl, tb, fn;

    glm_mat4_zero(dest);

    rl = 1.0f / (right  - left);
    tb = 1.0f / (top    - bottom);
    fn = 1.0f / (farZ - nearZ);  // 修改：Vulkan 使用 [0, 1] 范围

    dest[0][0] = 2.0f * rl;
    dest[1][1] = 2.0f * tb;
    dest[2][2] = fn;  // 修改：Z 值映射到 [0, 1]
    dest[3][0] = -(right  + left) * rl;
    dest[3][1] = -(top    + bottom) * tb;
    dest[3][2] = (farZ + nearZ) * fn;  // 修改：适配 Vulkan 深度范围
    dest[3][3] = 1.0f;
}

#endif