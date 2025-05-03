#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"

#ifndef VK_GRAPHICSPIPELINE_H
#define VK_GRAPHICSPIPELINE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createGraphicsPipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createModelPipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createShadowPipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createCombinePipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createParticlePipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createShapePipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline);
extern void SDLCALL createTileMapPipeline(Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline);
extern bool SDLCALL addComputePipeline(VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo, VkPipelineLayout computePipelineLayout, VkPipeline basePipelineHandle, Uint32 basePipelineIndex, VkPipeline * pComputePipeline);
extern bool SDLCALL addGraphicPipelineCreateInfo(Uint32 stageCount, VkPipelineShaderStageCreateInfo * pStage, VkPipelineVertexInputStateCreateInfo * pVertexInputState, VkPipelineInputAssemblyStateCreateInfo * pInputAssemblyState\
, VkPipelineTessellationStateCreateInfo * pTessellationState, VkPipelineViewportStateCreateInfo * pViewportState, VkPipelineRasterizationStateCreateInfo * pRasterizationState\
, VkPipelineMultisampleStateCreateInfo * pMultisampleState, VkPipelineDepthStencilStateCreateInfo * pDepthStencilState, VkPipelineColorBlendStateCreateInfo * pColorBlendState\
, VkPipelineDynamicStateCreateInfo * pDynamicState, VkPipelineLayout layout, VkRenderPass renderPass, Uint32 subpass, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkPipeline * pPipeline);
extern VkResult SDLCALL executeCreateGraphicsPipelines(VkPipelineCache pipelinesCache);
extern VkResult SDLCALL executeCreateComputePipelines(VkPipelineCache pipelinesCache);

#include "SDL3/SDL_close_code.h"

#endif //vk_graphicsPipeline.h