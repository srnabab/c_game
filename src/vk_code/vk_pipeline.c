#include "vk_code_h/vk_pipeline.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_collection.h"

#undef offsetof
#define offsetof(s, m) (size_t) & (((s *)0)->m)

#define VERTEX_LAYOUT_IN {\
    {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},\
    {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},\
    {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)}\
}

#warning "graphics pipeline create info count has limitation 10"
static VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfos[10];
static VkPipeline * graphicPipelinePtr[10];
static Uint32 graphicsPipelineCreateInfoCount = 0;

#warning "compute pipeline create info count has limitation 10"
static VkComputePipelineCreateInfo computePipelineCreateInfos[10];
static VkPipeline * computePipelinePtr[10];
static Uint32 computePipelineCreateInfoCount = 0;

extern VK_ALL allInOne;

static void addVertexBinding(Uint32 binding, Uint32 stride, VkVertexInputRate inputRate, Uint32 index, VkVertexInputBindingDescription * pBinding)
{
    pBinding[index].binding = binding;
    pBinding[index].stride = stride;
    pBinding[index].inputRate = inputRate;
}
static void addVertexAttribute(Uint32 * pLocation, Uint32 binding, VkFormat format, Uint32 offset, Uint32 * pIndex, VkVertexInputAttributeDescription * pAttribute)
{
    Uint32 location = *pLocation;
    Uint32 index = *pIndex;

    pAttribute[index].location = location;
    pAttribute[index].binding = binding;
    pAttribute[index].format = format;
    pAttribute[index].offset = offset;

    (*pLocation)++;
    (*pIndex)++;
}
static void configurePipelineVertexInputState(Uint32 bindingCount, VkVertexInputBindingDescription * pBindings, Uint32 attributeCount, VkVertexInputAttributeDescription * pAttributies\
, VkPipelineVertexInputStateCreateInfo * pPipelineVertexInputStateCreateInfo)
{
    pPipelineVertexInputStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pPipelineVertexInputStateCreateInfo->pNext = NULL;
    pPipelineVertexInputStateCreateInfo->flags = 0;
    pPipelineVertexInputStateCreateInfo->vertexBindingDescriptionCount = bindingCount;
    pPipelineVertexInputStateCreateInfo->pVertexBindingDescriptions = pBindings;
    pPipelineVertexInputStateCreateInfo->vertexAttributeDescriptionCount = attributeCount;
    pPipelineVertexInputStateCreateInfo->pVertexAttributeDescriptions = pAttributies;
}
static void configurePipelineInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable, VkPipelineInputAssemblyStateCreateInfo * pPipelineInputAssemblyStateCreateInfo)
{
    pPipelineInputAssemblyStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pPipelineInputAssemblyStateCreateInfo->pNext = NULL;
    pPipelineInputAssemblyStateCreateInfo->flags = 0;
    pPipelineInputAssemblyStateCreateInfo->topology = topology;
    pPipelineInputAssemblyStateCreateInfo->primitiveRestartEnable = primitiveRestartEnable;
}
static void configurePipelineTessellationStateCreateInfo(Uint32 patchControlPoints, VkPipelineTessellationStateCreateInfo * pPipelineTessellationStateCreateInfo)
{
    pPipelineTessellationStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    pPipelineTessellationStateCreateInfo->pNext = NULL;
    pPipelineTessellationStateCreateInfo->flags = 0;
    pPipelineTessellationStateCreateInfo->patchControlPoints = patchControlPoints;
}
static void addPipelineViewport(float x, float y, float width, float height, float minDepth, float maxDepth, Uint32 index, VkViewport * pViewport)
{
    pViewport[index].x = x;
    pViewport[index].y = y;
    pViewport[index].width = width;
    pViewport[index].height = height;
    pViewport[index].minDepth = minDepth;
    pViewport[index].maxDepth = maxDepth;
}
static void addPipelineScissor(VkOffset2D offset, VkExtent2D extent2D, Uint32 index, VkRect2D * pScissor)
{
    pScissor[index].offset = offset;
    pScissor[index].extent = extent2D;
}
static void configurePipelineViewportsStateCreateInfo(Uint32 viewportCount, VkViewport * pViewport, Uint32 scissorCount, VkRect2D * pScissor, VkPipelineViewportStateCreateInfo * pPipelineViewportStateCreateInfo)
{
    pPipelineViewportStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pPipelineViewportStateCreateInfo->pNext = NULL;
    pPipelineViewportStateCreateInfo->flags = 0;
    pPipelineViewportStateCreateInfo->viewportCount = viewportCount;
    pPipelineViewportStateCreateInfo->pViewports = pViewport;
    pPipelineViewportStateCreateInfo->scissorCount = scissorCount;
    pPipelineViewportStateCreateInfo->pScissors = pScissor;
}
static void configurePipelineRasterizationStateCreateInfo(VkBool32 depthClampEnable, VkBool32 rasterizeDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBiasEnable\
, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth, VkPipelineRasterizationStateCreateInfo * pPipelineRasterizationStateCreateInfo)
{
    pPipelineRasterizationStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pPipelineRasterizationStateCreateInfo->pNext = NULL;
    pPipelineRasterizationStateCreateInfo->flags = 0;
    pPipelineRasterizationStateCreateInfo->depthClampEnable = depthClampEnable;
    pPipelineRasterizationStateCreateInfo->rasterizerDiscardEnable = rasterizeDiscardEnable; 
    pPipelineRasterizationStateCreateInfo->polygonMode = polygonMode;
    pPipelineRasterizationStateCreateInfo->cullMode = cullMode;
    pPipelineRasterizationStateCreateInfo->frontFace = frontFace;
    pPipelineRasterizationStateCreateInfo->depthBiasEnable = depthBiasEnable;
    pPipelineRasterizationStateCreateInfo->depthBiasConstantFactor = depthBiasConstantFactor;
    pPipelineRasterizationStateCreateInfo->depthBiasClamp = depthBiasClamp;
    pPipelineRasterizationStateCreateInfo->depthBiasSlopeFactor = depthBiasSlopeFactor;
    pPipelineRasterizationStateCreateInfo->lineWidth = lineWidth;
}
static void configurePipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples, VkBool32 sampleShadingEnable, float minSampleShading, VkSampleMask * pSampleMask\
, VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable, VkPipelineMultisampleStateCreateInfo * pPipelineMultisampleStateCreateInfo)
{
    pPipelineMultisampleStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pPipelineMultisampleStateCreateInfo->pNext = NULL;
    pPipelineMultisampleStateCreateInfo->flags = 0;
    pPipelineMultisampleStateCreateInfo->rasterizationSamples = rasterizationSamples;
    pPipelineMultisampleStateCreateInfo->sampleShadingEnable = sampleShadingEnable;
    pPipelineMultisampleStateCreateInfo->minSampleShading = minSampleShading;
    pPipelineMultisampleStateCreateInfo->pSampleMask = pSampleMask;
    pPipelineMultisampleStateCreateInfo->alphaToCoverageEnable = alphaToCoverageEnable;
    pPipelineMultisampleStateCreateInfo->alphaToOneEnable = alphaToOneEnable;
}
static void configurePipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOP, VkBool32 depthBoundsTestEnable\
, VkBool32 stencilTestEnable, VkStencilOpState front, VkStencilOpState back, float minDepthBounds, float maxDepthBounds, VkPipelineDepthStencilStateCreateInfo * pPipelineDepthStencilStateCreateInfo)
{
    pPipelineDepthStencilStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pPipelineDepthStencilStateCreateInfo->pNext = NULL;
    pPipelineDepthStencilStateCreateInfo->flags = 0;
    pPipelineDepthStencilStateCreateInfo->depthTestEnable = depthTestEnable;
    pPipelineDepthStencilStateCreateInfo->depthWriteEnable = depthWriteEnable;
    pPipelineDepthStencilStateCreateInfo->depthCompareOp = depthCompareOP;
    pPipelineDepthStencilStateCreateInfo->depthBoundsTestEnable = depthBoundsTestEnable;
    pPipelineDepthStencilStateCreateInfo->stencilTestEnable = stencilTestEnable;
    pPipelineDepthStencilStateCreateInfo->front = front;
    pPipelineDepthStencilStateCreateInfo->back = back;
    pPipelineDepthStencilStateCreateInfo->minDepthBounds = minDepthBounds;
    pPipelineDepthStencilStateCreateInfo->maxDepthBounds = maxDepthBounds;
}
static void addColorBlendAttachmentState(VkBool32 blendEnable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor\
, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask, Uint32 index, VkPipelineColorBlendAttachmentState * pAttachment)
{
    pAttachment[index].blendEnable = blendEnable;
    pAttachment[index].srcColorBlendFactor = srcColorBlendFactor;
    pAttachment[index].dstColorBlendFactor = dstColorBlendFactor;
    pAttachment[index].colorBlendOp = colorBlendOp;
    pAttachment[index].srcAlphaBlendFactor = srcAlphaBlendFactor;
    pAttachment[index].dstAlphaBlendFactor = dstAlphaBlendFactor;
    pAttachment[index].alphaBlendOp = alphaBlendOp;
    pAttachment[index].colorWriteMask = colorWriteMask;
}
static void configurePipelineColorBlendStateCreateInfo(VkBool32 logicOpEnable, VkLogicOp logicOp, Uint32 attachmentCount, VkPipelineColorBlendAttachmentState * pAttachments\
, float constant1, float constant2, float constant3, float constant4, VkPipelineColorBlendStateCreateInfo * pPipelineColorBlendStateCreateInfo)
{
    pPipelineColorBlendStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pPipelineColorBlendStateCreateInfo->pNext = NULL;
    pPipelineColorBlendStateCreateInfo->flags = 0;
    pPipelineColorBlendStateCreateInfo->logicOpEnable = logicOpEnable;
    pPipelineColorBlendStateCreateInfo->logicOp = logicOp;
    pPipelineColorBlendStateCreateInfo->attachmentCount = attachmentCount;
    pPipelineColorBlendStateCreateInfo->pAttachments = pAttachments;
    pPipelineColorBlendStateCreateInfo->blendConstants[0] = constant1;
    pPipelineColorBlendStateCreateInfo->blendConstants[1] = constant2;
    pPipelineColorBlendStateCreateInfo->blendConstants[2] = constant3;
    pPipelineColorBlendStateCreateInfo->blendConstants[3] = constant4;
}
void configureDynamicsState(Uint32 dynamicCount, VkDynamicState * pDynamicStates, VkPipelineDynamicStateCreateInfo * pDynamicStateCreateInfo)
{
    pDynamicStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pDynamicStateCreateInfo->pNext = NULL;
    pDynamicStateCreateInfo->flags = 0;
    pDynamicStateCreateInfo->dynamicStateCount = dynamicCount;
    pDynamicStateCreateInfo->pDynamicStates = pDynamicStates;
}
void createTileMapPipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline)
{
    VkVertexInputBindingDescription pBindingDescription[1];
    addVertexBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, 0, pBindingDescription);

    VkVertexInputAttributeDescription pAttributeDescriptions[2];
}
void createModelPipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline)
{
    VkVertexInputBindingDescription pBindingDescription[3];
    addVertexBinding(0, sizeof(Vertex4), VK_VERTEX_INPUT_RATE_VERTEX, 0, pBindingDescription);
    addVertexBinding(1, sizeof(mat4), VK_VERTEX_INPUT_RATE_INSTANCE, 1, pBindingDescription);
    addVertexBinding(2, sizeof(mat4), VK_VERTEX_INPUT_RATE_INSTANCE, 2, pBindingDescription);

    VkVertexInputAttributeDescription pAttributeDescriptions[12];
    Uint32 attributeLocation = 0;
    Uint32 attributeIndex = 0;
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex4, pos), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex4, color), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex4, texCoord), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex4, normal), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 0, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 1, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 2, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 3, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 2, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 0, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 2, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 1, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 2, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 2, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 2, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 3, &attributeIndex, pAttributeDescriptions);

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    configurePipelineVertexInputState(3, pBindingDescription, 12, pAttributeDescriptions, &pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    configurePipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE, &pipelineInputAssemblyStateCreateInfo);

    VkViewport pViewport[1];
    addPipelineViewport(0.0f, 0.0f, extent2D.width, extent2D.height, 0.0f, 1.0f, 0, pViewport);

    VkRect2D pScissor[1];
    addPipelineScissor((VkOffset2D){0, 0}, extent2D, 0, pScissor);

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    configurePipelineViewportsStateCreateInfo(1, pViewport, 1, pScissor, &pipelineViewportStateCreateInfo);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    configurePipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f, &pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    configurePipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE, &pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    VkStencilOpState empty = {};
    configurePipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, empty, empty, 0.0f, 1.0f, &pipelineDepthStencilStateCreateInfo);

    
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState[3];
    addColorBlendAttachmentState(VK_TRUE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD\
        , VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, 0, colorBlendAttachmentState);
    addColorBlendAttachmentState(VK_TRUE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD\
        , VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, 1, colorBlendAttachmentState);
    addColorBlendAttachmentState(VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD\
        , VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, 2, colorBlendAttachmentState);

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    configurePipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, 3, colorBlendAttachmentState, 0.0f, 0.0f, 0.0f, 0.0f, &pipelineColorBlendStateCreateInfo);
    
    VkDynamicState dynamicStates[2];
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    configureDynamicsState(2, dynamicStates, &pipelineDynamicStateCreateInfo);

    addGraphicPipelineCreateInfo(shaderCount, pPipelineShaderStageCreateInfo, &pipelineVertexInputStateCreateInfo, &pipelineInputAssemblyStateCreateInfo, NULL, &pipelineViewportStateCreateInfo\
        , &pipelineRasterizationStateCreateInfo, &pipelineMultisampleStateCreateInfo, &pipelineDepthStencilStateCreateInfo, &pipelineColorBlendStateCreateInfo, &pipelineDynamicStateCreateInfo, pipelineLayout, renderPass\
        , 0, NULL, -1, pGraphicsPipeline);
}
void createGraphicsPipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline)
{
    VkVertexInputBindingDescription pBindingDescription[1];
    addVertexBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, 0, pBindingDescription);

    VkVertexInputAttributeDescription pAttributeDescriptions[3] = VERTEX_LAYOUT_IN;
    Uint32 attributeLocation = 0;
    Uint32 attributeIndex = 0;
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord), &attributeIndex, pAttributeDescriptions);

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    configurePipelineVertexInputState(1, pBindingDescription, 3, pAttributeDescriptions, &pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    configurePipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE, &pipelineInputAssemblyStateCreateInfo);

    VkViewport pViewport[1];
    addPipelineViewport(0.0f, 0.0f, extent2D.width, extent2D.height, 0.0f, 1.0f, 0, pViewport);

    VkRect2D pScissor[1];
    addPipelineScissor((VkOffset2D){0, 0}, extent2D, 0, pScissor);

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    configurePipelineViewportsStateCreateInfo(1, pViewport, 1, pScissor, &pipelineViewportStateCreateInfo);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    configurePipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f, &pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    configurePipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE, &pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    VkStencilOpState empty = {};
    configurePipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, empty, empty, 0.0f, 1.0f, &pipelineDepthStencilStateCreateInfo);

    
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState[1];
    addColorBlendAttachmentState(VK_TRUE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD\
        , VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, 0, colorBlendAttachmentState);

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    configurePipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, 1, colorBlendAttachmentState, 0.0f, 0.0f, 0.0f, 0.0f, &pipelineColorBlendStateCreateInfo);
    
    VkDynamicState dynamicStates[2];
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    configureDynamicsState(2, dynamicStates, &pipelineDynamicStateCreateInfo);

    addGraphicPipelineCreateInfo(shaderCount, pPipelineShaderStageCreateInfo, &pipelineVertexInputStateCreateInfo, &pipelineInputAssemblyStateCreateInfo, NULL, &pipelineViewportStateCreateInfo\
        , &pipelineRasterizationStateCreateInfo, &pipelineMultisampleStateCreateInfo, &pipelineDepthStencilStateCreateInfo, &pipelineColorBlendStateCreateInfo, &pipelineDynamicStateCreateInfo, pipelineLayout, renderPass\
        , 0, NULL, -1, pGraphicsPipeline);
}
void createParticlePipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline)
{
    VkVertexInputBindingDescription pBindingDescription[1];
    addVertexBinding(0, sizeof(Particle), VK_VERTEX_INPUT_RATE_VERTEX, 0, pBindingDescription);

    VkVertexInputAttributeDescription pAttributeDescriptions[2];
    Uint32 attributeLocation = 0;
    Uint32 attributeIndex = 0;
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, position), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, color), &attributeIndex, pAttributeDescriptions);

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    configurePipelineVertexInputState(1, pBindingDescription, 2, pAttributeDescriptions, &pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    configurePipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_FALSE, &pipelineInputAssemblyStateCreateInfo);

    VkViewport pViewport[1];
    addPipelineViewport(0.0f, 0.0f, extent2D.width, extent2D.height, 0.0f, 1.0f, 0, pViewport);

    VkRect2D pScissor[1];
    addPipelineScissor((VkOffset2D){0, 0}, extent2D, 0, pScissor);

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    configurePipelineViewportsStateCreateInfo(1, pViewport, 1, pScissor, &pipelineViewportStateCreateInfo);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    configurePipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f, &pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    configurePipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE, &pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    VkStencilOpState empty = {};
    configurePipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, empty, empty, 0.0f, 1.0f, &pipelineDepthStencilStateCreateInfo);

    
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState[1];
    addColorBlendAttachmentState(VK_TRUE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD\
        , VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, 0, colorBlendAttachmentState);

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    configurePipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, 1, colorBlendAttachmentState, 0.0f, 0.0f, 0.0f, 0.0f, &pipelineColorBlendStateCreateInfo);
    
    VkDynamicState dynamicStates[2];
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    configureDynamicsState(2, dynamicStates, &pipelineDynamicStateCreateInfo);

    addGraphicPipelineCreateInfo(shaderCount, pPipelineShaderStageCreateInfo, &pipelineVertexInputStateCreateInfo, &pipelineInputAssemblyStateCreateInfo, NULL, &pipelineViewportStateCreateInfo\
        , &pipelineRasterizationStateCreateInfo, &pipelineMultisampleStateCreateInfo, &pipelineDepthStencilStateCreateInfo, &pipelineColorBlendStateCreateInfo, &pipelineDynamicStateCreateInfo, pipelineLayout, renderPass\
        , 0, NULL, -1, pGraphicsPipeline);
}
void createCombinePipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline)
{
    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    configurePipelineVertexInputState(0, NULL, 0, NULL, &pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    configurePipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE, &pipelineInputAssemblyStateCreateInfo);

    VkViewport pViewport[1];
    addPipelineViewport(0.0f, 0.0f, extent2D.width, extent2D.height, 0.0f, 1.0f, 0, pViewport);

    VkRect2D pScissor[1];
    addPipelineScissor((VkOffset2D){0, 0}, extent2D, 0, pScissor);

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    configurePipelineViewportsStateCreateInfo(1, pViewport, 1, pScissor, &pipelineViewportStateCreateInfo);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    configurePipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f, &pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    configurePipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE, &pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    VkStencilOpState empty = {};
    configurePipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, empty, empty, 0.0f, 1.0f, &pipelineDepthStencilStateCreateInfo);

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState[1];
    addColorBlendAttachmentState(VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD\
        , VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, 0, colorBlendAttachmentState);

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    configurePipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, 1, colorBlendAttachmentState, 0.0f, 0.0f, 0.0f, 0.0f, &pipelineColorBlendStateCreateInfo);
    
    VkDynamicState dynamicStates[2];
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    configureDynamicsState(2, dynamicStates, &pipelineDynamicStateCreateInfo);

    addGraphicPipelineCreateInfo(shaderCount, pPipelineShaderStageCreateInfo, &pipelineVertexInputStateCreateInfo, &pipelineInputAssemblyStateCreateInfo, NULL, &pipelineViewportStateCreateInfo\
        , &pipelineRasterizationStateCreateInfo, &pipelineMultisampleStateCreateInfo, &pipelineDepthStencilStateCreateInfo, &pipelineColorBlendStateCreateInfo, &pipelineDynamicStateCreateInfo, pipelineLayout, renderPass\
        , 0, NULL, -1, pGraphicsPipeline);
}
void createShadowPipeline(VkExtent2D extent2D, Uint32 shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline * pGraphicsPipeline)
{
    VkVertexInputBindingDescription pBindingDescription[2];
    addVertexBinding(0, sizeof(Vertex4), VK_VERTEX_INPUT_RATE_VERTEX, 0, pBindingDescription);
    addVertexBinding(1, sizeof(mat4), VK_VERTEX_INPUT_RATE_INSTANCE, 1, pBindingDescription);

    VkVertexInputAttributeDescription pAttributeDescriptions[5];
    Uint32 attributeLocation = 0;
    Uint32 attributeIndex = 0;
    addVertexAttribute(&attributeLocation, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex4, pos), &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, 0, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 1, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 2, &attributeIndex, pAttributeDescriptions);
    addVertexAttribute(&attributeLocation, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(vec4) * 3, &attributeIndex, pAttributeDescriptions);

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    configurePipelineVertexInputState(2, pBindingDescription, 5, pAttributeDescriptions, &pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    configurePipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE, &pipelineInputAssemblyStateCreateInfo);

    VkViewport pViewport[1];
    addPipelineViewport(0.0f, 0.0f, extent2D.width, extent2D.height, 0.0f, 1.0f, 0, pViewport);

    VkRect2D pScissor[1];
    addPipelineScissor((VkOffset2D){0, 0}, extent2D, 0, pScissor);

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    configurePipelineViewportsStateCreateInfo(1, pViewport, 1, pScissor, &pipelineViewportStateCreateInfo);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    configurePipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f, &pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    configurePipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, NULL, VK_FALSE, VK_FALSE, &pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    VkStencilOpState empty = {};
    configurePipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL, VK_FALSE, VK_FALSE, empty, empty, 0.0f, 1.0f, &pipelineDepthStencilStateCreateInfo);

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    configurePipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, 0, NULL, 0.0f, 0.0f, 0.0f, 0.0f, &pipelineColorBlendStateCreateInfo);
    
    VkDynamicState dynamicStates[2];
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    configureDynamicsState(2, dynamicStates, &pipelineDynamicStateCreateInfo);

    addGraphicPipelineCreateInfo(shaderCount, pPipelineShaderStageCreateInfo, &pipelineVertexInputStateCreateInfo, &pipelineInputAssemblyStateCreateInfo, NULL, &pipelineViewportStateCreateInfo\
        , &pipelineRasterizationStateCreateInfo, &pipelineMultisampleStateCreateInfo, &pipelineDepthStencilStateCreateInfo, &pipelineColorBlendStateCreateInfo, &pipelineDynamicStateCreateInfo, pipelineLayout, renderPass\
        , 0, NULL, -1, pGraphicsPipeline);
}
bool addGraphicPipelineCreateInfo(Uint32 stageCount, VkPipelineShaderStageCreateInfo * pStage, VkPipelineVertexInputStateCreateInfo * pVertexInputState, VkPipelineInputAssemblyStateCreateInfo * pInputAssemblyState\
, VkPipelineTessellationStateCreateInfo * pTessellationState, VkPipelineViewportStateCreateInfo * pViewportState, VkPipelineRasterizationStateCreateInfo * pRasterizationState\
, VkPipelineMultisampleStateCreateInfo * pMultisampleState, VkPipelineDepthStencilStateCreateInfo * pDepthStencilState, VkPipelineColorBlendStateCreateInfo * pColorBlendState\
, VkPipelineDynamicStateCreateInfo * pDynamicState, VkPipelineLayout layout, VkRenderPass renderPass, Uint32 subpass, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkPipeline * pPipeline)
{
    if (graphicsPipelineCreateInfoCount == 10) return false;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = NULL;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = stageCount;
    pipelineCreateInfo.pStages = pStage;

    VkVertexInputBindingDescription * pBindingDescription = SDL_malloc(pVertexInputState->vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription));
    memcpy(pBindingDescription, pVertexInputState->pVertexBindingDescriptions, pVertexInputState->vertexBindingDescriptionCount * sizeof(VkVertexInputBindingDescription));

    VkVertexInputAttributeDescription * pAttributeDescription = SDL_malloc(pVertexInputState->vertexAttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription));
    memcpy(pAttributeDescription, pVertexInputState->pVertexAttributeDescriptions, pVertexInputState->vertexAttributeDescriptionCount * sizeof(VkVertexInputAttributeDescription));

    VkPipelineVertexInputStateCreateInfo * vertexInputState = SDL_malloc(sizeof(VkPipelineVertexInputStateCreateInfo));
    memcpy(vertexInputState, pVertexInputState, sizeof(VkPipelineVertexInputStateCreateInfo));
    vertexInputState->pVertexBindingDescriptions = pBindingDescription;
    vertexInputState->pVertexAttributeDescriptions = pAttributeDescription;
    pipelineCreateInfo.pVertexInputState = vertexInputState;

    VkPipelineInputAssemblyStateCreateInfo * inputAssemblyState = SDL_malloc(sizeof(VkPipelineInputAssemblyStateCreateInfo));
    memcpy(inputAssemblyState, pInputAssemblyState, sizeof(VkPipelineInputAssemblyStateCreateInfo));
    pipelineCreateInfo.pInputAssemblyState = inputAssemblyState;

    if (pTessellationState != NULL)
    {
        VkPipelineTessellationStateCreateInfo * tessellationState = SDL_malloc(sizeof(VkPipelineTessellationStateCreateInfo));
        memcpy(tessellationState, pTessellationState, sizeof(VkPipelineTessellationStateCreateInfo));
        pipelineCreateInfo.pTessellationState = tessellationState;
    }
    else pipelineCreateInfo.pTessellationState = NULL;

    VkViewport * pViewport = SDL_malloc(pViewportState->viewportCount * sizeof(VkViewport));
    memcpy(pViewport, pViewportState->pViewports, pViewportState->viewportCount * sizeof(VkViewport));

    VkRect2D * pScissor = SDL_malloc(pViewportState->scissorCount * sizeof(VkRect2D));
    memcpy(pScissor, pViewportState->pScissors, pViewportState->scissorCount * sizeof(VkRect2D));

    VkPipelineViewportStateCreateInfo * viewportState = SDL_malloc(sizeof(VkPipelineViewportStateCreateInfo));
    memcpy(viewportState, pViewportState, sizeof(VkPipelineViewportStateCreateInfo));
    viewportState->pViewports = pViewport;
    viewportState->pScissors = pScissor;
    pipelineCreateInfo.pViewportState = viewportState;

    VkPipelineRasterizationStateCreateInfo * rasterizationState = SDL_malloc(sizeof(VkPipelineRasterizationStateCreateInfo));
    memcpy(rasterizationState, pRasterizationState, sizeof(VkPipelineRasterizationStateCreateInfo));
    pipelineCreateInfo.pRasterizationState = rasterizationState;

    VkPipelineMultisampleStateCreateInfo * multisampleState = SDL_malloc(sizeof(VkPipelineMultisampleStateCreateInfo));
    memcpy(multisampleState, pMultisampleState, sizeof(VkPipelineMultisampleStateCreateInfo));
    pipelineCreateInfo.pMultisampleState = multisampleState;

    VkPipelineDepthStencilStateCreateInfo * depthStencilState = SDL_malloc(sizeof(VkPipelineDepthStencilStateCreateInfo));
    memcpy(depthStencilState, pDepthStencilState, sizeof(VkPipelineDepthStencilStateCreateInfo));
    pipelineCreateInfo.pDepthStencilState = depthStencilState;

    VkPipelineColorBlendAttachmentState * colorBlendAttachmentState = SDL_malloc(pColorBlendState->attachmentCount * sizeof(VkPipelineColorBlendAttachmentState));
    memcpy(colorBlendAttachmentState, pColorBlendState->pAttachments, pColorBlendState->attachmentCount * sizeof(VkPipelineColorBlendAttachmentState));

    VkPipelineColorBlendStateCreateInfo * colorBlendState = SDL_malloc(sizeof(VkPipelineColorBlendStateCreateInfo));
    memcpy(colorBlendState, pColorBlendState, sizeof(VkPipelineColorBlendStateCreateInfo));
    colorBlendState->pAttachments = colorBlendAttachmentState;
    pipelineCreateInfo.pColorBlendState = colorBlendState;

    VkDynamicState * dynamicStates = SDL_malloc(pDynamicState->dynamicStateCount * sizeof(VkDynamicState));
    memcpy(dynamicStates, pDynamicState->pDynamicStates, pDynamicState->dynamicStateCount * sizeof(VkDynamicState));

    VkPipelineDynamicStateCreateInfo * dynamicState = SDL_malloc(sizeof(VkPipelineDynamicStateCreateInfo));
    memcpy(dynamicState, pDynamicState, sizeof(VkPipelineDynamicStateCreateInfo));
    dynamicState->pDynamicStates = dynamicStates;
    pipelineCreateInfo.pDynamicState = dynamicState;

    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = subpass;
    
    if (basePipelineHandle != NULL)
    {
        // need process //
    }
    else pipelineCreateInfo.basePipelineHandle = basePipelineHandle;

    pipelineCreateInfo.basePipelineIndex = basePipelineIndex;

    graphicsPipelineCreateInfos[graphicsPipelineCreateInfoCount] = pipelineCreateInfo;
    graphicPipelinePtr[graphicsPipelineCreateInfoCount] = pPipeline;
    graphicsPipelineCreateInfoCount++;

    return true;
}
VkResult executeCreateGraphicsPipelines(VkPipelineCache pipelinesCache)
{
    VkPipeline tempGraphicsPipeline[10];
    VkResult result = vkCreateGraphicsPipelines(allInOne.device, NULL, graphicsPipelineCreateInfoCount, graphicsPipelineCreateInfos, allInOne.pAllocationCallbacks, tempGraphicsPipeline);

    for  (Uint32 i = 0;i < graphicsPipelineCreateInfoCount;i++)
    {
        *graphicPipelinePtr[i] = tempGraphicsPipeline[i];
        SDL_free((void*)graphicsPipelineCreateInfos[i].pVertexInputState->pVertexBindingDescriptions);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pVertexInputState->pVertexAttributeDescriptions);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pInputAssemblyState);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pViewportState->pViewports);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pViewportState->pScissors);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pViewportState);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pRasterizationState);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pMultisampleState);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pDepthStencilState);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pColorBlendState->pAttachments);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pColorBlendState);
        SDL_free((void*)graphicsPipelineCreateInfos[i].pDynamicState);

        CO_addPiepline(tempGraphicsPipeline[i]);
    }

    graphicsPipelineCreateInfoCount = 0;

    return result;
}
bool addComputePipeline(VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo, VkPipelineLayout computePipelineLayout, VkPipeline basePipelineHandle, Uint32 basePipelineIndex, VkPipeline * pComputePipeline)
{
    if (computePipelineCreateInfoCount == 10) return false;

    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = NULL;
    pipelineInfo.flags = 0;
    pipelineInfo.stage = *pShaderStageCreateInfo;
    pipelineInfo.layout = computePipelineLayout;
    pipelineInfo.basePipelineHandle = basePipelineHandle;
    pipelineInfo.basePipelineIndex = basePipelineIndex;

    computePipelineCreateInfos[computePipelineCreateInfoCount] = pipelineInfo;
    computePipelinePtr[computePipelineCreateInfoCount] = pComputePipeline;
    computePipelineCreateInfoCount++;

    return true;

    vkCreateComputePipelines(allInOne.device, NULL, 1, &pipelineInfo, allInOne.pAllocationCallbacks, pComputePipeline);
}
VkResult executeCreateComputePipelines(VkPipelineCache pipelinesCache)
{
    VkPipeline tempComputePipelines[10];
    VkResult result = vkCreateComputePipelines(allInOne.device, NULL, computePipelineCreateInfoCount, computePipelineCreateInfos, allInOne.pAllocationCallbacks, tempComputePipelines);

    for (Uint32 i = 0;i < computePipelineCreateInfoCount;i++)
    {
        *computePipelinePtr[i] = tempComputePipelines[i];
        CO_addPiepline(tempComputePipelines[i]);
    }

    computePipelineCreateInfoCount = 0;

    return result;
}