#include "vk_code_h/vk_renderPass.h"

#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

static void setAttachmentDescription(VkAttachmentDescriptionFlags flags, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp\
    , VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout, VkImageLayout finalLayout, VkAttachmentDescription * pAttachment)
{
    pAttachment->flags = flags;
    pAttachment->format = format;
    pAttachment->samples = samples;
    pAttachment->loadOp = loadOp;
    pAttachment->storeOp = storeOp;
    pAttachment->stencilLoadOp = stencilLoadOp;
    pAttachment->stencilStoreOp = stencilStoreOp;
    pAttachment->initialLayout = initialLayout;
    pAttachment->finalLayout = finalLayout;
}
static void setSubpassDescription(VkSubpassDescriptionFlags flags, VkPipelineBindPoint pipelineBindPoint, Uint32 inputAttachmentCount, const VkAttachmentReference * pInputAttachments\
    , Uint32 colorAttachmentCount, const VkAttachmentReference * pColorAttachments, const VkAttachmentReference * pResolveAttachments, const VkAttachmentReference * pDepthStencilAttachment\
    , Uint32 preserveAttachmentCount, const Uint32 * pPreserveAttachments, VkSubpassDescription * pSubpass)
{
    pSubpass->flags = flags;
    pSubpass->pipelineBindPoint = pipelineBindPoint;
    pSubpass->inputAttachmentCount = inputAttachmentCount;
    pSubpass->pInputAttachments = pInputAttachments;
    pSubpass->colorAttachmentCount = colorAttachmentCount;
    pSubpass->pColorAttachments = pColorAttachments;
    pSubpass->pResolveAttachments = pResolveAttachments;
    pSubpass->pDepthStencilAttachment = pDepthStencilAttachment;
    pSubpass->preserveAttachmentCount = preserveAttachmentCount;
    pSubpass->pPreserveAttachments = pPreserveAttachments;
}
static void setSubpassDependency(Uint32 srcSubpass, Uint32 dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask\
    , VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags dependencyFlags, VkSubpassDependency * pDependency)
{
    pDependency->srcSubpass = srcSubpass;
    pDependency->dstSubpass = dstSubpass;
    pDependency->srcStageMask = srcStageMask;
    pDependency->dstStageMask = dstStageMask;
    pDependency->srcAccessMask = srcAccessMask;
    pDependency->dstAccessMask = dstAccessMask;
    pDependency->dependencyFlags = dependencyFlags;
}
void createGraphicRenderPass(VkFormat surfaceFormat, VkRenderPass * pRenderPass)
{
    VkAttachmentDescription colorAttachment = {};

    setAttachmentDescription(0, surfaceFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE\
        , VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &colorAttachment);

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass1 = {};
    setSubpassDescription(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1, &colorAttachmentRef, NULL, NULL, 0, NULL, &subpass1);

    VkSubpassDependency dependency[2];
    setSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT\
        , VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, 0, &dependency[0]);

    // setSubpassDependency(0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0, 0, &dependency[1]);

    VkAttachmentDescription attachments[] = {colorAttachment};

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass1;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = dependency;

    resultVulkan(vkCreateRenderPass(allInOne.device, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), 0);

    //printf("renderPass created\n");
}
void createModelRenderPass(VkFormat colorFormat, VkFormat normalFormat, VkFormat shadowFormat, VkFormat depthFormat, VkRenderPass * pRenderPass)
{
    VkAttachmentDescription attachment[3];

    // color attachment
    setAttachmentDescription(0, colorFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE\
        , VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &attachment[0]);

    // normal attachment
    setAttachmentDescription(0, normalFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE\
        , VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &attachment[1]);

    // shadow attachment
    setAttachmentDescription(0, shadowFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE\
        , VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &attachment[2]);


    VkAttachmentReference attachmentRef[3];
    attachmentRef[0].attachment = 0;
    attachmentRef[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentRef[1].attachment = 1;
    attachmentRef[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentRef[2].attachment = 2;
    attachmentRef[2].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkAttachmentDescription depthAttachment = {};
    setAttachmentDescription(0, depthFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE\
        , VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, &depthAttachment);

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 3;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass0 = {};
    setSubpassDescription(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 3, attachmentRef, NULL, &depthAttachmentRef, 0, NULL, &subpass0);

    VkSubpassDependency dependency = {};
    setSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT\
        , 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, 0, &dependency);

    VkAttachmentDescription attachments[] = {attachment[0], attachment[1], attachment[2], depthAttachment};
    VkSubpassDescription subpasses[] = {subpass0};

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 4;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = subpasses;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    resultVulkan(vkCreateRenderPass(allInOne.device, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), 0);

    //printf("renderPass created\n");
}
void createShadowRenderPass(VkFormat depthFormat, VkRenderPass * pRenderPass)
{
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.flags = 0;
    depthAttachment.format = depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 0;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass0 = {};
    subpass0.flags = 0;
    subpass0.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass0.inputAttachmentCount = 0;
    subpass0.pInputAttachments = NULL;
    subpass0.colorAttachmentCount = 0;
    subpass0.pColorAttachments = NULL;
    subpass0.pResolveAttachments = NULL;
    subpass0.pDepthStencilAttachment = &depthAttachmentRef;
    subpass0.preserveAttachmentCount = 0;
    subpass0.pPreserveAttachments = NULL;

    VkSubpassDependency dependencies[2];

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkAttachmentDescription attachments[] = {depthAttachment};
    VkSubpassDescription subpasses[] = {subpass0};

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = subpasses;
    renderPassCreateInfo.dependencyCount = 2;
    renderPassCreateInfo.pDependencies = dependencies;

    resultVulkan(vkCreateRenderPass(allInOne.device, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), 0);

    //printf("renderPass created\n");
}
void createCombineRenderPass(VkFormat colorFormat, VkRenderPass * pRenderPass)
{
    VkAttachmentDescription attachment[2];
    // color attachment
    attachment[0].flags = 0;
    attachment[0].format = colorFormat;
    attachment[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentRef[2];
    attachmentRef[0].attachment = 0;
    attachmentRef[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass0 = {};
    subpass0.flags = 0;
    subpass0.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass0.inputAttachmentCount = 0;
    subpass0.pInputAttachments = NULL;
    subpass0.colorAttachmentCount = 1;
    subpass0.pColorAttachments = attachmentRef;
    subpass0.pResolveAttachments = NULL;
    subpass0.pDepthStencilAttachment = NULL;
    subpass0.preserveAttachmentCount = 0;
    subpass0.pPreserveAttachments = NULL;

    VkAttachmentDescription attachments[] = {attachment[0]};
    VkSubpassDescription subpasses[] = {subpass0};
    
    VkSubpassDependency dependencies[2];

    // Dependency from external (previous passes) to this subpass
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; // Adjust if inputs come from Compute
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT; // Adjust if inputs come from Compute
    dependencies[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; // Usually safe for post-processing

    // Dependency from this subpass to external (next pass)
    // dependencies[1].srcSubpass = 0;
    // dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    // dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; // Stage where next pass reads
    // dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    // dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;        // Access type for next pass reading
    // dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = subpasses;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = dependencies;

    resultVulkan(vkCreateRenderPass(allInOne.device, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), 0);

    //printf("renderPass created\n");
}
void createOffscreenRenderPass(VkFormat format, VkRenderPass * pRenderPass)
{
    VkAttachmentDescription colorAttachment = {};
    setAttachmentDescription(0, format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED\
        , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &colorAttachment);

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    setSubpassDescription(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, NULL, 1, &colorAttachmentRef, NULL, NULL, 0, NULL, &subpass);

    VkSubpassDependency dependency = {};
    setSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, 0, &dependency);

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    resultVulkan(vkCreateRenderPass(allInOne.device, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), 0);
}