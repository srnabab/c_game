#include "G_begin_code.h"
#include "vulkan/vulkan.h"
#include "G_file/path_compare.h"

#ifndef VK_SHADER_H
#define VK_SHADER_H 1

void createShaderModule(VkDevice * pDevice, PathType type, VkShaderModule * pShaderModule);
void addShaderStageCreateInfo(VkShaderModule * pShaderModule, VkPipelineStageFlags stage, uint32_t * pShaderCount, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo);

#endif //vk_shader.h

/*void configureShaderStageCreateInfo(VkShaderModule * vertShaderModule, VkShaderModule * fragShaderModule, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo);*/