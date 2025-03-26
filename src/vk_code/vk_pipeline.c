#include "vk_code_h/vk_pipeline.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#undef offsetof
#define offsetof(s, m) (size_t) & (((s *)0)->m)

#define VERTEX_LAYOUT_IN {\
    {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},\
    {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},\
    {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)}\
}

extern VK_ALL allInOne;

void configureDynamicsState(VkPipelineDynamicStateCreateInfo * pDynamicStateCreateInfo)
{
    uint32_t dynamicCount = 2;
    VkDynamicState * dynamicStates = (VkDynamicState *)SDL_malloc(dynamicCount * sizeof(VkDynamicState));
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    (*pDynamicStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    (*pDynamicStateCreateInfo).pNext = NULL;
    (*pDynamicStateCreateInfo).flags = 0;
    (*pDynamicStateCreateInfo).dynamicStateCount = dynamicCount;
    (*pDynamicStateCreateInfo).pDynamicStates = dynamicStates;
}
static void getBlindingDescription(VkVertexInputBindingDescription ** pBindingDescription)
{
    (*pBindingDescription) = (VkVertexInputBindingDescription *)SDL_malloc(3 * sizeof(VkVertexInputBindingDescription));
    (*pBindingDescription)[0].binding = 0;
    (*pBindingDescription)[0].stride = sizeof(vec3);
    (*pBindingDescription)[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    (*pBindingDescription)[1].binding = 1;
    (*pBindingDescription)[1].stride = sizeof(vec3);
    (*pBindingDescription)[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    (*pBindingDescription)[2].binding = 2;
    (*pBindingDescription)[2].stride = sizeof(vec2);
    (*pBindingDescription)[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}
static void getAttributeDescription(VkVertexInputAttributeDescription ** pAttributeDescription)
{
    *pAttributeDescription = (VkVertexInputAttributeDescription*)SDL_malloc(3 * sizeof(VkVertexInputAttributeDescription));
    (*pAttributeDescription)[0].location = 0;
    (*pAttributeDescription)[0].binding = 0;
    (*pAttributeDescription)[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    (*pAttributeDescription)[0].offset = 0;

    (*pAttributeDescription)[1].location = 1;
    (*pAttributeDescription)[1].binding = 1;
    (*pAttributeDescription)[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    (*pAttributeDescription)[1].offset = 0;

    (*pAttributeDescription)[2].location = 2;
    (*pAttributeDescription)[2].binding = 2;
    (*pAttributeDescription)[2].format = VK_FORMAT_R32G32_SFLOAT;
    (*pAttributeDescription)[2].offset = 0;
}
static void configurePipelineVertexInputState(VkPipelineVertexInputStateCreateInfo * pPipelineVertexInputStateCreateInfo)
{
    VkVertexInputBindingDescription * pBindingDescription = NULL;
    getBlindingDescription(&pBindingDescription);

    VkVertexInputAttributeDescription * pAttributeDescriptions = NULL;
    getAttributeDescription(&pAttributeDescriptions);

    (*pPipelineVertexInputStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    (*pPipelineVertexInputStateCreateInfo).pNext = NULL;
    (*pPipelineVertexInputStateCreateInfo).flags = 0;
    (*pPipelineVertexInputStateCreateInfo).vertexBindingDescriptionCount = 3;
    (*pPipelineVertexInputStateCreateInfo).pVertexBindingDescriptions = pBindingDescription;
    (*pPipelineVertexInputStateCreateInfo).vertexAttributeDescriptionCount = 3;
    (*pPipelineVertexInputStateCreateInfo).pVertexAttributeDescriptions = pAttributeDescriptions;
}
static void configurePipelineInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo * pPipelineInputAssemblyStateCreateInfo)
{
    (*pPipelineInputAssemblyStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    (*pPipelineInputAssemblyStateCreateInfo).pNext = NULL;
    (*pPipelineInputAssemblyStateCreateInfo).flags = 0;
    (*pPipelineInputAssemblyStateCreateInfo).topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    (*pPipelineInputAssemblyStateCreateInfo).primitiveRestartEnable = VK_FALSE;
}
static void configurePipelineViewportsStateCreateInfo(VkPipelineViewportStateCreateInfo * pPipelineViewportStateCreateInfo, VkExtent2D * pExtent2D)
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
    (*pPipelineViewportStateCreateInfo).pNext = NULL;
    (*pPipelineViewportStateCreateInfo).flags = 0;
    (*pPipelineViewportStateCreateInfo).viewportCount = 1;
    (*pPipelineViewportStateCreateInfo).pViewports = viewport;
    (*pPipelineViewportStateCreateInfo).scissorCount =1;
    (*pPipelineViewportStateCreateInfo).pScissors = scissor;
}
static void configurePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo * pPipelineRasterizationStateCreateInfo)
{
    (*pPipelineRasterizationStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    (*pPipelineRasterizationStateCreateInfo).pNext = NULL;
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
static void configurePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo * pPipelineMultisampleStateCreateInfo)
{
    (*pPipelineMultisampleStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    (*pPipelineMultisampleStateCreateInfo).pNext = NULL;
    (*pPipelineMultisampleStateCreateInfo).flags = 0;
    (*pPipelineMultisampleStateCreateInfo).rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    (*pPipelineMultisampleStateCreateInfo).sampleShadingEnable = VK_FALSE;
    (*pPipelineMultisampleStateCreateInfo).minSampleShading = 1.0f; // Optional
    (*pPipelineMultisampleStateCreateInfo).pSampleMask = NULL; // Optional
    (*pPipelineMultisampleStateCreateInfo).alphaToCoverageEnable = VK_FALSE; // Optional
    (*pPipelineMultisampleStateCreateInfo).alphaToOneEnable = VK_FALSE; // Optional
}
static void configurePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo * pPipelineDepthStencilStateCreateInfo)
{
    (*pPipelineDepthStencilStateCreateInfo).sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    (*pPipelineDepthStencilStateCreateInfo).pNext = NULL;
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
static void configurePipelineColorBlendStateCreateInfo(uint32_t attachmentCount, VkPipelineColorBlendStateCreateInfo * pPipelineColorBlendStateCreateInfo)
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
    (*pPipelineColorBlendStateCreateInfo).pNext = NULL;
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
void createRenderPass(VkFormat surfaceFormat, VkFormat depthFormat)
{
    FuncCode code = createRenderPassF;
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.flags = 0;
    colorAttachment.format = surfaceFormat;
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
    depthAttachment.format = depthFormat;
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
    subpass1.pInputAttachments = NULL;
    subpass1.colorAttachmentCount = 1;
    subpass1.pColorAttachments = &colorAttachmentRef;
    subpass1.pResolveAttachments = NULL;
    subpass1.pDepthStencilAttachment = &depthAttachmentRef;
    subpass1.preserveAttachmentCount = 0;
    subpass1.pPreserveAttachments = NULL;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 2;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass1;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    resultVulkan(vkCreateRenderPass(*allInOne.pDevice, &renderPassCreateInfo, allInOne.pAllocationCallbacks, allInOne.pRenderPass), code, 0);

    //printf("renderPass created\n");
}
void createModelPipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline)
{
    FuncCode code = createGraphicsPipelineF;

    // VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    // configurePipelineVertexInputState(&pipelineVertexInputStateCreateInfo);
    VkVertexInputBindingDescription pBindingDescription[3];
    pBindingDescription[0].binding = 0;
    pBindingDescription[0].stride = sizeof(Vertex4);
    pBindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    pBindingDescription[1].binding = 1;
    pBindingDescription[1].stride = sizeof(mat4);
    pBindingDescription[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    pBindingDescription[2].binding = 2;
    pBindingDescription[2].stride = sizeof(mat4);
    pBindingDescription[2].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;


    VkVertexInputAttributeDescription pAttributeDescriptions[12];
    pAttributeDescriptions[0].location = 0;
    pAttributeDescriptions[0].binding = 0;
    pAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    pAttributeDescriptions[0].offset = offsetof(Vertex4, pos);

    pAttributeDescriptions[1].location = 1;
    pAttributeDescriptions[1].binding = 0;
    pAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    pAttributeDescriptions[1].offset = offsetof(Vertex4, color);

    pAttributeDescriptions[2].location = 2;
    pAttributeDescriptions[2].binding = 0;
    pAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    pAttributeDescriptions[2].offset = offsetof(Vertex4, texCoord);

    pAttributeDescriptions[3].location = 3;
    pAttributeDescriptions[3].binding = 0;
    pAttributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    pAttributeDescriptions[3].offset = offsetof(Vertex4, normal);

    pAttributeDescriptions[4].location = 4;
    pAttributeDescriptions[4].binding = 1;
    pAttributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[4].offset = 0;
    
    pAttributeDescriptions[5].location = 5;
    pAttributeDescriptions[5].binding = 1;
    pAttributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[5].offset = sizeof(vec4) * 1;

    pAttributeDescriptions[6].location = 6;
    pAttributeDescriptions[6].binding = 1;
    pAttributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[6].offset = sizeof(vec4) * 2;

    pAttributeDescriptions[7].location = 7;
    pAttributeDescriptions[7].binding = 1;
    pAttributeDescriptions[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[7].offset = sizeof(vec4) * 3;

    pAttributeDescriptions[8].location = 8;
    pAttributeDescriptions[8].binding = 2;
    pAttributeDescriptions[8].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[8].offset = 0;
    
    pAttributeDescriptions[9].location = 9;
    pAttributeDescriptions[9].binding = 2;
    pAttributeDescriptions[9].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[9].offset = sizeof(vec4) * 1;

    pAttributeDescriptions[10].location = 10;
    pAttributeDescriptions[10].binding = 2;
    pAttributeDescriptions[10].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[10].offset = sizeof(vec4) * 2;

    pAttributeDescriptions[11].location = 11;
    pAttributeDescriptions[11].binding = 2;
    pAttributeDescriptions[11].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    pAttributeDescriptions[11].offset = sizeof(vec4) * 3;


    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.pNext = NULL;
    pipelineVertexInputStateCreateInfo.flags = 0;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 3;
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = pBindingDescription;

    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 12;
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = pAttributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.pNext = NULL;
    pipelineInputAssemblyStateCreateInfo.flags = 0;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    // configurePipelineInputAssemblyState(&pipelineInputAssemblyStateCreateInfo);

    //VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo = {};
    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    VkViewport pViewport[3];
    pViewport[0].x = 0.0f;
    pViewport[0].y = 0.0f;
    pViewport[0].width = (float)pExtent2D->width;
    pViewport[0].height = (float)pExtent2D->height;
    pViewport[0].minDepth = 0.0f;
    pViewport[0].maxDepth = 1.0f;

    VkRect2D pScissor[3];
    pScissor[0].offset = (VkOffset2D){0, 0};
    pScissor[0].extent = *pExtent2D;

    pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfo.pNext = NULL;
    pipelineViewportStateCreateInfo.flags = 0;
    pipelineViewportStateCreateInfo.viewportCount = 1;
    pipelineViewportStateCreateInfo.pViewports = pViewport;
    pipelineViewportStateCreateInfo.scissorCount = 1;
    pipelineViewportStateCreateInfo.pScissors = pScissor;
    // configurePipelineViewportsStateCreateInfo(&pipelineViewportStateCreateInfo, pExtent2D);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizationStateCreateInfo.pNext = NULL;
    pipelineRasterizationStateCreateInfo.flags = 0;
    pipelineRasterizationStateCreateInfo.depthBiasClamp = VK_FALSE;
    pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE; 
    pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
    // configurePipelineRasterizationStateCreateInfo(&pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleStateCreateInfo.pNext = NULL;
    pipelineMultisampleStateCreateInfo.flags = 0;
    pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
    pipelineMultisampleStateCreateInfo.pSampleMask = NULL; // Optional
    pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional
    // configurePipelineMultisampleStateCreateInfo(&pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pipelineDepthStencilStateCreateInfo.pNext = NULL;
    pipelineDepthStencilStateCreateInfo.flags = 0;
    pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    VkStencilOpState empty = {};
    pipelineDepthStencilStateCreateInfo.front = empty;
    pipelineDepthStencilStateCreateInfo.back = empty;
    pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
    pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    // configurePipelineDepthStencilStateCreateInfo(&pipelineDepthStencilStateCreateInfo);

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    Uint32 attachmentCount = 1;
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

    pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlendStateCreateInfo.pNext = NULL;
    pipelineColorBlendStateCreateInfo.flags = 0;
    pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
    pipelineColorBlendStateCreateInfo.pAttachments = colorBlendAttachmentState;
    pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
    pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
    pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
    pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f; // Optional
    // configurePipelineColorBlendStateCreateInfo(1, &pipelineColorBlendStateCreateInfo);
    
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    uint32_t dynamicCount = 2;
    VkDynamicState * dynamicStates = (VkDynamicState *)SDL_malloc(dynamicCount * sizeof(VkDynamicState));
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.pNext = NULL;
    pipelineDynamicStateCreateInfo.flags = 0;
    pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicCount;
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates;
    // configureDynamicsState(&pipelineDynamicStateCreateInfo);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = NULL;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = shaderCount;
    pipelineCreateInfo.pStages = pPipelineShaderStageCreateInfo;
    pipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    pipelineCreateInfo.pTessellationState = NULL;
    pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
    pipelineCreateInfo.layout = *pPipelineLayout;
    pipelineCreateInfo.renderPass = *pRenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = NULL;
    pipelineCreateInfo.basePipelineIndex = -1;

    resultVulkan(vkCreateGraphicsPipelines(*pDevice, NULL, 1, &pipelineCreateInfo, allInOne.pAllocationCallbacks, pGraphicsPipeline),
                                        code, 4,
                                        (void*)pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions,
                                        (void*)pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions,
                                        (void*)pipelineColorBlendStateCreateInfo.pAttachments,
                                        (void*)pipelineDynamicStateCreateInfo.pDynamicStates);

    //printf("graphicsPipelinecreated\n");
}
void createGraphicsPipeline(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t shaderCount, VkPipelineShaderStageCreateInfo * pPipelineShaderStageCreateInfo, VkPipelineLayout * pPipelineLayout, VkRenderPass * pRenderPass, VkPipeline * pGraphicsPipeline)
{
    FuncCode code = createGraphicsPipelineF;

    // VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    // configurePipelineVertexInputState(&pipelineVertexInputStateCreateInfo);
    VkVertexInputBindingDescription pBindingDescription[3];
    pBindingDescription[0].binding = 0;
    pBindingDescription[0].stride = sizeof(Vertex);
    pBindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription pAttributeDescriptions[3] = VERTEX_LAYOUT_IN;

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.pNext = NULL;
    pipelineVertexInputStateCreateInfo.flags = 0;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = pBindingDescription;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 3;
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = pAttributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.pNext = NULL;
    pipelineInputAssemblyStateCreateInfo.flags = 0;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    // configurePipelineInputAssemblyState(&pipelineInputAssemblyStateCreateInfo);

    //VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo = {};
    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    VkViewport pViewport[3];
    pViewport[0].x = 0.0f;
    pViewport[0].y = 0.0f;
    pViewport[0].width = (float)pExtent2D->width;
    pViewport[0].height = (float)pExtent2D->height;
    pViewport[0].minDepth = 0.0f;
    pViewport[0].maxDepth = 1.0f;

    VkRect2D pScissor[3];
    pScissor[0].offset = (VkOffset2D){0, 0};
    pScissor[0].extent = *pExtent2D;

    pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfo.pNext = NULL;
    pipelineViewportStateCreateInfo.flags = 0;
    pipelineViewportStateCreateInfo.viewportCount = 1;
    pipelineViewportStateCreateInfo.pViewports = pViewport;
    pipelineViewportStateCreateInfo.scissorCount = 1;
    pipelineViewportStateCreateInfo.pScissors = pScissor;
    // configurePipelineViewportsStateCreateInfo(&pipelineViewportStateCreateInfo, pExtent2D);

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizationStateCreateInfo.pNext = NULL;
    pipelineRasterizationStateCreateInfo.flags = 0;
    pipelineRasterizationStateCreateInfo.depthBiasClamp = VK_FALSE;
    pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE; 
    pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
    // configurePipelineRasterizationStateCreateInfo(&pipelineRasterizationStateCreateInfo);

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleStateCreateInfo.pNext = NULL;
    pipelineMultisampleStateCreateInfo.flags = 0;
    pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
    pipelineMultisampleStateCreateInfo.pSampleMask = NULL; // Optional
    pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional
    // configurePipelineMultisampleStateCreateInfo(&pipelineMultisampleStateCreateInfo);

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pipelineDepthStencilStateCreateInfo.pNext = NULL;
    pipelineDepthStencilStateCreateInfo.flags = 0;
    pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    VkStencilOpState empty = {};
    pipelineDepthStencilStateCreateInfo.front = empty;
    pipelineDepthStencilStateCreateInfo.back = empty;
    pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
    pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
    // configurePipelineDepthStencilStateCreateInfo(&pipelineDepthStencilStateCreateInfo);

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    Uint32 attachmentCount = 1;
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

    pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlendStateCreateInfo.pNext = NULL;
    pipelineColorBlendStateCreateInfo.flags = 0;
    pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
    pipelineColorBlendStateCreateInfo.pAttachments = colorBlendAttachmentState;
    pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
    pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
    pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
    pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f; // Optional
    // configurePipelineColorBlendStateCreateInfo(1, &pipelineColorBlendStateCreateInfo);
    
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
    uint32_t dynamicCount = 2;
    VkDynamicState * dynamicStates = (VkDynamicState *)SDL_malloc(dynamicCount * sizeof(VkDynamicState));
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.pNext = NULL;
    pipelineDynamicStateCreateInfo.flags = 0;
    pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicCount;
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates;
    // configureDynamicsState(&pipelineDynamicStateCreateInfo);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = NULL;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = shaderCount;
    pipelineCreateInfo.pStages = pPipelineShaderStageCreateInfo;
    pipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    pipelineCreateInfo.pTessellationState = NULL;
    pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
    pipelineCreateInfo.layout = *pPipelineLayout;
    pipelineCreateInfo.renderPass = *pRenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = NULL;
    pipelineCreateInfo.basePipelineIndex = -1;

    resultVulkan(vkCreateGraphicsPipelines(*pDevice, NULL, 1, &pipelineCreateInfo, allInOne.pAllocationCallbacks, pGraphicsPipeline),
                                        code, 4,
                                        (void*)pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions,
                                        (void*)pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions,
                                        (void*)pipelineColorBlendStateCreateInfo.pAttachments,
                                        (void*)pipelineDynamicStateCreateInfo.pDynamicStates);

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
    attributeDescription[0].location = 0;
    attributeDescription[0].binding = 0;
    attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription[0].offset = offsetof(Particle, position);

    attributeDescription[1].location = 1;
    attributeDescription[1].binding = 0;
    attributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescription[1].offset = offsetof(Particle, color);

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.pNext = NULL;
    pipelineVertexInputStateCreateInfo.flags = 0;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription;
    //configurePipelineVertexInputState(&pipelineVertexInputStateCreateInfo);

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.pNext = NULL;
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
    // pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
    // pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;

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
    pipelineCreateInfo.pNext = NULL;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = shaderCount;
    pipelineCreateInfo.pStages = pPipelineShaderStageCreateInfo;
    pipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    pipelineCreateInfo.pTessellationState = NULL;
    pipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
    pipelineCreateInfo.layout = *pPipelineLayout;
    pipelineCreateInfo.renderPass = *pRenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = NULL;
    pipelineCreateInfo.basePipelineIndex = -1;

    resultVulkan(vkCreateGraphicsPipelines(*pDevice, NULL, 1, &pipelineCreateInfo, allInOne.pAllocationCallbacks, pGraphicsPipeline),
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
    pipelineInfo.pNext = NULL;
    pipelineInfo.flags = 0;
    pipelineInfo.stage = *pShaderStageCreateInfo;
    pipelineInfo.layout = *pComputePipelineLayout;
    pipelineInfo.basePipelineHandle = NULL;
    pipelineInfo.basePipelineIndex = 0;

    vkCreateComputePipelines(*pDevice, NULL, 1, &pipelineInfo, allInOne.pAllocationCallbacks, pComputePipeline);
}