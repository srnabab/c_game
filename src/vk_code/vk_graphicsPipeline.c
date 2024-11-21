#include "vk_graphicsPipeline.h"

void configureDynamicsState(VkPipelineDynamicStateCreateInfo * pDynamicStateCreateInfo)
{
    uint32_t dynamicCount = 2;
    VkDynamicState * dynamicStates = (VkDynamicState *)SDL_malloc(dynamicCount * sizeof(VkDynamicState));
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    (*pDynamicStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    (*pDynamicStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pDynamicStateCreateInfo).flags = 0;
    (*pDynamicStateCreateInfo).dynamicStateCount = dynamicCount;
    (*pDynamicStateCreateInfo).pDynamicStates = dynamicStates;
}
static void getBlindingDescription(VkVertexInputBindingDescription ** pBindingDescription)
{
    (*pBindingDescription) = (VkVertexInputBindingDescription *)SDL_malloc(1 * sizeof(VkVertexInputBindingDescription));
    (*pBindingDescription)[0].binding = 0;
    (*pBindingDescription)[0].stride = sizeof(Vertex);
    (*pBindingDescription)[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}
static void getAttributeDescription(VkVertexInputAttributeDescription ** pAttributeDescription)
{
    *pAttributeDescription = (VkVertexInputAttributeDescription*)SDL_malloc(3 * sizeof(VkVertexInputAttributeDescription));
    (*pAttributeDescription)[0].location = 0;
    (*pAttributeDescription)[0].binding = 0;
    (*pAttributeDescription)[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    (*pAttributeDescription)[0].offset = offsetof(Vertex, pos);

    (*pAttributeDescription)[1].location = 1;
    (*pAttributeDescription)[1].binding = 0;
    (*pAttributeDescription)[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    (*pAttributeDescription)[1].offset = offsetof(Vertex, color);

    (*pAttributeDescription)[2].location = 2;
    (*pAttributeDescription)[2].binding = 0;
    (*pAttributeDescription)[2].format = VK_FORMAT_R32G32_SFLOAT;
    (*pAttributeDescription)[2].offset = offsetof(Vertex, texCoord);
}
void configurePipelineVertexInputState(VkPipelineVertexInputStateCreateInfo * pPipelineVertexInputStateCreateInfo)
{
    VkVertexInputBindingDescription * pBindingDescription = VK_NULL_HANDLE;
    getBlindingDescription(&pBindingDescription);

    VkVertexInputAttributeDescription * pAttributeDescriptions = VK_NULL_HANDLE;
    getAttributeDescription(&pAttributeDescriptions);

    (*pPipelineVertexInputStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    (*pPipelineVertexInputStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pPipelineVertexInputStateCreateInfo).flags = 0;
    (*pPipelineVertexInputStateCreateInfo).vertexBindingDescriptionCount = 1;
    (*pPipelineVertexInputStateCreateInfo).pVertexBindingDescriptions = pBindingDescription;
    (*pPipelineVertexInputStateCreateInfo).vertexAttributeDescriptionCount = 3;
    (*pPipelineVertexInputStateCreateInfo).pVertexAttributeDescriptions = pAttributeDescriptions;
}
void configurePipelineInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo * pPipelineInputAssemblyStateCreateInfo)
{
    (*pPipelineInputAssemblyStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    (*pPipelineInputAssemblyStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pPipelineInputAssemblyStateCreateInfo).flags = 0;
    (*pPipelineInputAssemblyStateCreateInfo).topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    (*pPipelineInputAssemblyStateCreateInfo).primitiveRestartEnable = VK_FALSE;
}
void configurePipelineViewportsStateCreateInfo(VkPipelineViewportStateCreateInfo * pPipelineViewportStateCreateInfo, VkExtent2D * pExtent2D)
{
    VkViewport * viewport = (VkViewport*)SDL_malloc(sizeof(VkViewport));
    viewport->x = 0.0f;
    viewport->y = 0.0f;
    viewport->width = (float)pExtent2D->width;
    viewport->height = (float)pExtent2D->height;
    viewport->minDepth = 0.0f;
    viewport->maxDepth = 1.0f;

    VkRect2D * scissor = (VkRect2D *)SDL_malloc(sizeof(VkRect2D));
    scissor->offset = (VkOffset2D){0, 0};
    scissor->extent = *pExtent2D;

    (*pPipelineViewportStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    (*pPipelineViewportStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pPipelineViewportStateCreateInfo).flags = 0;
    (*pPipelineViewportStateCreateInfo).viewportCount = 1;
    (*pPipelineViewportStateCreateInfo).pViewports = viewport;
    (*pPipelineViewportStateCreateInfo).scissorCount =1;
    (*pPipelineViewportStateCreateInfo).pScissors = scissor;
}
void configurePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo * pPipelineRasterizationStateCreateInfo)
{
    (*pPipelineRasterizationStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    (*pPipelineRasterizationStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pPipelineRasterizationStateCreateInfo).flags = 0;
    (*pPipelineRasterizationStateCreateInfo).depthBiasClamp = VK_FALSE;
    (*pPipelineRasterizationStateCreateInfo).rasterizerDiscardEnable = VK_FALSE; 
    (*pPipelineRasterizationStateCreateInfo).polygonMode = VK_POLYGON_MODE_FILL;
    (*pPipelineRasterizationStateCreateInfo).cullMode = VK_CULL_MODE_BACK_BIT;
    (*pPipelineRasterizationStateCreateInfo).frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    (*pPipelineRasterizationStateCreateInfo).depthBiasEnable = VK_FALSE;
    (*pPipelineRasterizationStateCreateInfo).depthBiasConstantFactor = 0.0f;
    (*pPipelineRasterizationStateCreateInfo).depthBiasClamp = 0.0f;
    (*pPipelineRasterizationStateCreateInfo).depthBiasSlopeFactor = 0.0f;
    (*pPipelineRasterizationStateCreateInfo).lineWidth = 1.0f;
}
void configurePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo * pPipelineMultisampleStateCreateInfo)
{
    (*pPipelineMultisampleStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    (*pPipelineMultisampleStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pPipelineMultisampleStateCreateInfo).flags = 0;
    (*pPipelineMultisampleStateCreateInfo).rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    (*pPipelineMultisampleStateCreateInfo).sampleShadingEnable = VK_FALSE;
    (*pPipelineMultisampleStateCreateInfo).minSampleShading = 1.0f; // Optional
    (*pPipelineMultisampleStateCreateInfo).pSampleMask = VK_NULL_HANDLE; // Optional
    (*pPipelineMultisampleStateCreateInfo).alphaToCoverageEnable = VK_FALSE; // Optional
    (*pPipelineMultisampleStateCreateInfo).alphaToOneEnable = VK_FALSE; // Optional
}
void configurePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo * pPipelineDepthStencilStateCreateInfo)
{
    (*pPipelineDepthStencilStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    (*pPipelineDepthStencilStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pPipelineDepthStencilStateCreateInfo).flags = 0;
    (*pPipelineDepthStencilStateCreateInfo).depthTestEnable = VK_TRUE;
    (*pPipelineDepthStencilStateCreateInfo).depthWriteEnable = VK_TRUE;
    (*pPipelineDepthStencilStateCreateInfo).depthCompareOp = VK_COMPARE_OP_LESS;
    (*pPipelineDepthStencilStateCreateInfo).depthBoundsTestEnable = VK_FALSE;
    (*pPipelineDepthStencilStateCreateInfo).stencilTestEnable = VK_FALSE;
    VkStencilOpState empty = {};
    (*pPipelineDepthStencilStateCreateInfo).front = empty;
    (*pPipelineDepthStencilStateCreateInfo).back = empty;
    (*pPipelineDepthStencilStateCreateInfo).minDepthBounds = 0.0f;
    (*pPipelineDepthStencilStateCreateInfo).maxDepthBounds = 1.0f;
}
void configurePipelineColorBlendStateCreateInfo(uint32_t attachmentCount, VkPipelineColorBlendStateCreateInfo * pPipelineColorBlendStateCreateInfo)
{
    VkPipelineColorBlendAttachmentState * colorBlendAttachmentState = (VkPipelineColorBlendAttachmentState *)SDL_malloc(attachmentCount * sizeof(VkPipelineColorBlendAttachmentState));
    
    for (uint32_t i = 0;i < attachmentCount;i++)
    {
        colorBlendAttachmentState[i].blendEnable = VK_TRUE;
        colorBlendAttachmentState[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState[i].colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState[i].alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    }

    (*pPipelineColorBlendStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    (*pPipelineColorBlendStateCreateInfo).pNext = VK_NULL_HANDLE;
    (*pPipelineColorBlendStateCreateInfo).flags = 0;
    (*pPipelineColorBlendStateCreateInfo).logicOpEnable = VK_FALSE;
    (*pPipelineColorBlendStateCreateInfo).logicOp = VK_LOGIC_OP_COPY; // Optional
    (*pPipelineColorBlendStateCreateInfo).attachmentCount = attachmentCount;
    (*pPipelineColorBlendStateCreateInfo).pAttachments = colorBlendAttachmentState;
    (*pPipelineColorBlendStateCreateInfo).blendConstants[0] = 0.0f; // Optional
    (*pPipelineColorBlendStateCreateInfo).blendConstants[1] = 0.0f; // Optional
    (*pPipelineColorBlendStateCreateInfo).blendConstants[2] = 0.0f; // Optional
    (*pPipelineColorBlendStateCreateInfo).blendConstants[3] = 0.0f; // Optional
}
void createPushConstantRange(VkShaderStageFlags flag, uint32_t offset, uint32_t size, VkPushConstantRange * pConstantRange)
{
    pConstantRange->stageFlags = flag;
    pConstantRange->offset = offset;
    pConstantRange->size = size;
}
void createPipelineLayout(VkDevice * pDevice, uint32_t setLayoutCount, VkDescriptorSetLayout ** ppDescriptorSetLayout, uint32_t pushConstantRangeCount, VkPushConstantRange * pPushConstantRange, VkPipelineLayout * pPipelineLayout)
{
    FuncCode code = createPipelineLayoutF;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = VK_NULL_HANDLE;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
    pipelineLayoutCreateInfo.pSetLayouts = *ppDescriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
    pipelineLayoutCreateInfo.pPushConstantRanges = pPushConstantRange;

    resultVulkan(vkCreatePipelineLayout(*pDevice, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, pPipelineLayout), code, 0);

    //printf("pipeLayout created\n");
}
void createRenderPass(VkDevice * pDevice, VkFormat * pFormat, VkFormat * pDepthFormat, VkRenderPass * pRenderPass)
{
    FuncCode code = createRenderPassF;
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.flags = 0;
    colorAttachment.format = *pFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.flags = 0;
    depthAttachment.format = *pDepthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass1 = {};
    subpass1.flags = 0;
    subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass1.inputAttachmentCount = 0;
    subpass1.pInputAttachments = VK_NULL_HANDLE;
    subpass1.colorAttachmentCount = 1;
    subpass1.pColorAttachments = &colorAttachmentRef;
    subpass1.pResolveAttachments = VK_NULL_HANDLE;
    subpass1.pDepthStencilAttachment = &depthAttachmentRef;
    subpass1.preserveAttachmentCount = 0;
    subpass1.pPreserveAttachments = VK_NULL_HANDLE;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    VkAttachmentDescription attachments[3] = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = VK_NULL_HANDLE;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 2;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass1;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    resultVulkan(vkCreateRenderPass(*pDevice, &renderPassCreateInfo, VK_NULL_HANDLE, pRenderPass), code, 0);

    //printf("renderPass created\n");
}
void createGraphicsPipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline)
{
    FuncCode code = createGraphicsPipelineF;

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    configurePipelineVertexInputState(&pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    configurePipelineInputAssemblyState(&pipelineInputAssemblyStateCreateInfo);

    //VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo = {};
    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    configurePipelineViewportsStateCreateInfo(&pipelineViewportStateCreateInfo, pExtent2D);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    configurePipelineRasterizationStateCreateInfo(&pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    configurePipelineMultisampleStateCreateInfo(&pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    configurePipelineDepthStencilStateCreateInfo(&pipelineDepthStencilStateCreateInfo);
    pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    configurePipelineColorBlendStateCreateInfo(1, &pipelineColorBlendStateCreateInfo);
    
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    configureDynamicsState(&pipelineDynamicStateCreateInfo);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = VK_NULL_HANDLE;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = shaderCount;
    pipelineCreateInfo.pStages = pPipelineShaderStageCreateInfo;
    pipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    pipelineCreateInfo.pTessellationState = VK_NULL_HANDLE;
    pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
    pipelineCreateInfo.layout = *pPipelineLayout;
    pipelineCreateInfo.renderPass = *pRenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    resultVulkan(vkCreateGraphicsPipelines(*pDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, VK_NULL_HANDLE, pGraphicsPipeline),
                                        code, 4,
                                        (void*)pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions,
                                        (void*)pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions,
                                        (void*)pipelineColorBlendStateCreateInfo.pAttachments,
                                        (void*)pipelineDynamicStateCreateInfo.pDynamicStates);

    SDL_free((void*)pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions);
    SDL_free((void*)pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions);
    SDL_free((void*)pipelineColorBlendStateCreateInfo.pAttachments);
    SDL_free((void*)pipelineDynamicStateCreateInfo.pDynamicStates);
    //printf("graphicsPipelinecreated\n");
}
void createParticlePipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline)
{
    FuncCode code = createGraphicsPipelineF;

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Particle);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescription[2];
    attributeDescription[0].binding = 0;
    attributeDescription[0].location = 0;
    attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription[0].offset = offsetof(Particle, position);

    attributeDescription[1].binding = 0;
    attributeDescription[1].location = 1;
    attributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescription[1].offset = offsetof(Particle, color);

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.pNext = VK_NULL_HANDLE;
    pipelineVertexInputStateCreateInfo.flags = 0;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription;
    //configurePipelineVertexInputState(&pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.pNext = VK_NULL_HANDLE;
    pipelineInputAssemblyStateCreateInfo.flags = 0;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    //configurePipelineInputAssemblyState(&pipelineInputAssemblyStateCreateInfo);

    //VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo = {};
    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    configurePipelineViewportsStateCreateInfo(&pipelineViewportStateCreateInfo, pExtent2D);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    configurePipelineRasterizationStateCreateInfo(&pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    configurePipelineMultisampleStateCreateInfo(&pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    configurePipelineDepthStencilStateCreateInfo(&pipelineDepthStencilStateCreateInfo);
    /*pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;*/

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    configurePipelineColorBlendStateCreateInfo(1, &pipelineColorBlendStateCreateInfo);
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;
    
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    configureDynamicsState(&pipelineDynamicStateCreateInfo);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = VK_NULL_HANDLE;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = shaderCount;
    pipelineCreateInfo.pStages = pPipelineShaderStageCreateInfo;
    pipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    pipelineCreateInfo.pTessellationState = VK_NULL_HANDLE;
    pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
    pipelineCreateInfo.layout = *pPipelineLayout;
    pipelineCreateInfo.renderPass = *pRenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    resultVulkan(vkCreateGraphicsPipelines(*pDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, VK_NULL_HANDLE, pGraphicsPipeline),
                                        code, 0);

    //free((void*)pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions);
    //free((void*)pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions);
    //free((void*)pipelineColorBlendStateCreateInfo.pAttachments);
    //free((void*)pipelineDynamicStateCreateInfo.pDynamicStates);
    //printf("graphicsPipelinecreated\n");
}
void createComputePipeline(VkDevice * pDevice, VkPipelineLayout * pComputePipelineLayout, VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo, VkPipeline * pComputePipeline)
{
    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = VK_NULL_HANDLE;
    pipelineInfo.flags = 0;
    pipelineInfo.stage = *pShaderStageCreateInfo;
    pipelineInfo.layout = *pComputePipelineLayout;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;

    vkCreateComputePipelines(*pDevice, VK_NULL_HANDLE, 1, &pipelineInfo, VK_NULL_HANDLE, pComputePipeline);
}