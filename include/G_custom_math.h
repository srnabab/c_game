#include "vk_struct.h"

#ifndef G_CUSTOM_CGLM_H
#define G_CUSTOM_CHLM_H 1

#include "SDL3/SDL_begin_code.h"

CGLM_INLINE void SDLCALL glm_ortho_vulkan(float left, float right, float bottom, float top, float nearZ, float farZ, mat4 dest)
{
    float rl, tb, fn;

    glm_mat4_zero(dest);

    rl = 1.0f / (right  - left);
    tb = 1.0f / (top    - bottom);
    fn = 1.0f / (farZ - nearZ);  // 修改：Vulkan 使用 [0, 1] 范围

    dest[0][0] = 2.0f * rl;
    dest[1][1] = -2.0f * tb;
    dest[2][2] = fn;  // 修改：Z 值映射到 [0, 1]
    dest[3][0] = -(right  + left) * rl;
    dest[3][1] = -(top    + bottom) * tb;
    dest[3][2] = -nearZ * fn;  // 修改：适配 Vulkan 深度范围
    dest[3][3] = 1.0f;
}
SDL_FORCE_INLINE float SDLCALL SDL_sinf_flip(float a)
{
    float value = SDL_fmodf(a, 2 * SDL_PI_F);
    int neg = 1;
    if (value > M_PI_2 && value < M_PI_2 * 3) neg = -1;

    return neg * SDL_sinf(value);
}

#include "SDL3/SDL_close_code.h"

#endif