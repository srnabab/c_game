#include "G_begin_code.h"
#include "vulkan/vulkan.h"

#ifndef VK_GRAPHICSPIPELINE_H
#define VK_GRAPHICSPIPELINE_H 1

void configureDynamicsState(VkPipelineDynamicStateCreateInfo * pDynamicStateCreateInfo);
void configurePipelineVertexInputState(VkPipelineVertexInputStateCreateInfo * pPipelineVertexInputStateCreateInfo);
void configurePipelineInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo * pPipelineInputAssemblyStateCreateInfo);
void configurePipelineViewportsStateCreateInfo(VkPipelineViewportStateCreateInfo * pPipelineViewportStateCreateInfo, VkExtent2D * pExtent2D);
void configurePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo * pPipelineRasterizationStateCreateInfo);
void configurePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo * pPipelineDepthStencilStateCreateInfo);
void configurePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo * pPipelineMultisampleStateCreateInfo);
void configurePipelineColorBlendStateCreateInfo(uint32_t attachmentCount, VkPipelineColorBlendStateCreateInfo * pPipelineColorBlendStateCreateInfo);
void createPushConstantRange(VkShaderStageFlags flag, uint32_t offset, uint32_t size, VkPushConstantRange * pConstantRange);
void createPipelineLayout(VkDevice * pDevice, uint32_t setLayoutCount, VkDescriptorSetLayout ** ppDescriptorSetLayout, uint32_t pushConstantRangeCount, VkPushConstantRange * pPushConstantRange, VkPipelineLayout * pPipelineLayout);
void createRenderPass(VkDevice * pDevice, VkFormat * pFormat, VkFormat * pDepthFormat, VkRenderPass * pRenderPass);
void createGraphicsPipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline);
void createParticlePipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline);
void createComputePipeline(VkDevice * pDevice, VkPipelineLayout * pComputePipelineLayout, VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo, VkPipeline * pComputePipeline);

#endif //vk_graphicsPipeline.h