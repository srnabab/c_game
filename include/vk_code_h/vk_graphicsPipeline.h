#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_GRAPHICSPIPELINE_H
#define VK_GRAPHICSPIPELINE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL configureDynamicsState(VkPipelineDynamicStateCreateInfo * pDynamicStateCreateInfo);
extern void SDLCALL configurePipelineVertexInputState(VkPipelineVertexInputStateCreateInfo * pPipelineVertexInputStateCreateInfo);
extern void SDLCALL configurePipelineInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo * pPipelineInputAssemblyStateCreateInfo);
extern void SDLCALL configurePipelineViewportsStateCreateInfo(VkPipelineViewportStateCreateInfo * pPipelineViewportStateCreateInfo, VkExtent2D * pExtent2D);
extern void SDLCALL configurePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo * pPipelineRasterizationStateCreateInfo);
extern void SDLCALL configurePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo * pPipelineDepthStencilStateCreateInfo);
extern void SDLCALL configurePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo * pPipelineMultisampleStateCreateInfo);
extern void SDLCALL configurePipelineColorBlendStateCreateInfo(uint32_t attachmentCount, VkPipelineColorBlendStateCreateInfo * pPipelineColorBlendStateCreateInfo);
extern void SDLCALL createPushConstantRange(VkShaderStageFlags flag, uint32_t offset, uint32_t size, VkPushConstantRange * pConstantRange);
extern void SDLCALL createPipelineLayout(VkDevice * pDevice, uint32_t setLayoutCount, VkDescriptorSetLayout ** ppDescriptorSetLayout, uint32_t pushConstantRangeCount, VkPushConstantRange * pPushConstantRange, VkPipelineLayout * pPipelineLayout);
extern void SDLCALL createRenderPass(VkDevice * pDevice, VkFormat * pFormat, VkFormat * pDepthFormat, VkRenderPass * pRenderPass);
extern void SDLCALL createGraphicsPipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createParticlePipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createComputePipeline(VkDevice * pDevice, VkPipelineLayout * pComputePipelineLayout, VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo, VkPipeline * pComputePipeline);

#include "SDL3/SDL_close_code.h"

#endif //vk_graphicsPipeline.h