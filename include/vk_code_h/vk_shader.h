#include "core.h"

#ifndef VK_SHADER_H
#define VK_SHADER_H

void createShaderModule(VkDevice * pDevice, PathType type, VkShaderModule * pShaderModule);
void addShaderStageCreateInfo(VkShaderModule * pShaderModule, VkPipelineStageFlags stage, uint32_t * pShaderCount, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo);

#endif //vk_shader.h

/*void configureShaderStageCreateInfo(VkShaderModule * vertShaderModule, VkShaderModule * fragShaderModule, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo);*/