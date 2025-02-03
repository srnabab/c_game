#include "vulkan/vulkan.h"
#include "G_file/path_compare.h"

#ifndef VK_SHADER_H
#define VK_SHADER_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createShaderModuleFromFile(PathType type, VkShaderModule * pShaderModule);
extern VkResult SDLCALL createShaderModuleFromMem(size_t codeSize, const Uint32 * pCode, VkShaderModule * pShaderModule);
extern void SDLCALL addShaderStageCreateInfo(VkShaderModule * pShaderModule, VkPipelineStageFlags stage, uint32_t * pShaderCount, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo);

#include "SDL3/SDL_close_code.h"

#endif //vk_shader.h