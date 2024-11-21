#include "cglm/cglm.h"
#include "vk_struct.h"
#include <math.h>
#include "log.h"

#ifndef CUSTOM_CGLM_H
#define CUSTOM_CHLM_H

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
CGLM_INLINE void glm_scale_self(Vertex ** ppVertices, float scale, uint32_t pictureSequence)
{
    uint32_t index = pictureSequence * 4;

    float actural_scale = scale - 1.0f;

    float half_width_scaled = (SDL_fabsf((*ppVertices)[index].pos[0] - (*ppVertices)[index + 1].pos[0]) / 2) * actural_scale;
    logMessage("x1: %f, x2: %f", (*ppVertices)[index].pos[0], (*ppVertices)[index + 1].pos[0]);
    float half_height_scaled = (SDL_fabsf((*ppVertices)[index].pos[1] - (*ppVertices)[index + 3].pos[1]) / 2) * actural_scale;

    (*ppVertices)[index].pos[1] -= half_height_scaled;
    (*ppVertices)[index + 1].pos[1] -= half_height_scaled;
    (*ppVertices)[index + 2].pos[1] += half_height_scaled;
    (*ppVertices)[index + 3].pos[1] += half_height_scaled;

    (*ppVertices)[index].pos[0] -= half_width_scaled;
    (*ppVertices)[index + 3].pos[0] -= half_width_scaled;
    (*ppVertices)[index + 2].pos[0] += half_width_scaled;
    (*ppVertices)[index + 1].pos[0] += half_width_scaled;
}
CGLM_INLINE void fix_ratio(Vertex ** ppVertices, VkExtent2D oldExtent2D, VkExtent2D extent2D, uint32_t pictureSequence)
{
    uint32_t index = pictureSequence * 4;

    float scale = extent2D.width / (float)oldExtent2D.width;

    float aspect_width = (oldExtent2D.width / (float)extent2D.width) * scale;
    float aspect_height = (oldExtent2D.height / (float)extent2D.height) * scale;

    (*ppVertices)[index].pos[0] *= aspect_width;
    (*ppVertices)[index].pos[1] *= aspect_height;
    (*ppVertices)[index + 1].pos[0] *= aspect_width;
    (*ppVertices)[index + 1].pos[1] *= aspect_height;
    (*ppVertices)[index + 2].pos[0] *= aspect_width;
    (*ppVertices)[index + 2].pos[1] *= aspect_height;
    (*ppVertices)[index + 3].pos[0] *= aspect_width;
    (*ppVertices)[index + 3].pos[1] *= aspect_height;
}

#endif