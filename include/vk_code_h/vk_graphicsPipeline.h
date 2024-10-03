#include "vk_all.h"

#ifndef VK_GRAPHICSPIPELINE_H
#define VK_GRAPHICSPIPELINE_H

void configureDynamicsState(VkPipelineDynamicStateCreateInfo * pDynamicStateCreateInfo);
static void getBlindingDescription(VkVertexInputBindingDescription ** pBindingDescription);
static void getAttributeDescription(VkVertexInputAttributeDescription ** pAttributeDescription);
void configurePipelineVertexInputState(VkPipelineVertexInputStateCreateInfo * pPipelineVertexInputStateCreateInfo);
void configurePipelineInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo * pPipelineInputAssemblyStateCreateInfo);
void configurePipelineViewportsStateCreateInfo(VkPipelineViewportStateCreateInfo * pPipelineViewportStateCreateInfo, VkExtent2D * pExtent2D);
void configurePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo * pPipelineRasterizationStateCreateInfo);
void configurePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo * pPipelineDepthStencilStateCreateInfo);
void configurePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo * pPipelineMultisampleStateCreateInfo);
void configurePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo * pPipelineColorBlendStateCreateInfo);
void createPipelineLayout(VkDevice * pDevice, uint32_t setLayoutCount, VkDescriptorSetLayout ** ppDescriptorSetLayout, VkPipelineLayout * pPipelineLayout);
void createRenderPass(VkDevice * pDevice, VkFormat * pFormat, VkFormat * pDepthFormat, VkRenderPass * pRenderPass);
void createGraphicsPipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline);
void createParticlePipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline);
void createComputePipeline(VkDevice * pDevice, VkPipelineLayout * pComputePipelineLayout, VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo, VkPipeline * pComputePipeline);

#endif //vk_graphicsPipeline.h