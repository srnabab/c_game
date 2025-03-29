#include "vk_code_h/vk_renderPass.h"

#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

void createGraphicRenderPass(VkFormat surfaceFormat, VkFormat depthFormat, VkRenderPass * pRenderPass)
{
    FuncCode code = createGraphicRenderPassF;
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

    resultVulkan(vkCreateRenderPass(*allInOne.pDevice, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), code, 0);

    //printf("renderPass created\n");
}
void createModelRenderPass(VkFormat colorFormat, VkFormat normalFormat, VkFormat depthFormat, VkRenderPass * pRenderPass)
{
    FuncCode code = createGraphicRenderPassF;
    VkAttachmentDescription attachment[2];

    // color attachment
    attachment[0].flags = 0;
    attachment[0].format = colorFormat;
    attachment[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // normal attachment
    attachment[1].flags = 0;
    attachment[1].format = normalFormat;
    attachment[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachment[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment[1].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference attachmentRef[2];
    attachmentRef[0].attachment = 0;
    attachmentRef[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentRef[1].attachment = 1;
    attachmentRef[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.flags = 0;
    depthAttachment.format = depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 2;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass0 = {};
    subpass0.flags = 0;
    subpass0.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass0.inputAttachmentCount = 0;
    subpass0.pInputAttachments = NULL;
    subpass0.colorAttachmentCount = 2;
    subpass0.pColorAttachments = attachmentRef;
    subpass0.pResolveAttachments = NULL;
    subpass0.pDepthStencilAttachment = &depthAttachmentRef;
    subpass0.preserveAttachmentCount = 0;
    subpass0.pPreserveAttachments = NULL;

    // VkSubpassDependency dependency = {};
    // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependency.dstSubpass = 0;
    // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // dependency.srcAccessMask = 0;
    // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    // dependency.dependencyFlags = 0;

    VkAttachmentDescription attachments[] = {attachment[0], attachment[1], depthAttachment};
    VkSubpassDescription subpasses[] = {subpass0};

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 3;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = subpasses;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = NULL;

    resultVulkan(vkCreateRenderPass(*allInOne.pDevice, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), code, 0);

    //printf("renderPass created\n");
}
void createCombineRenderPass(VkFormat colorFormat, VkRenderPass * pRenderPass)
{
    FuncCode code = createGraphicRenderPassF;
    VkAttachmentDescription attachment[2];

    // color attachment
    attachment[0].flags = 0;
    attachment[0].format = colorFormat;
    attachment[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
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

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = NULL;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = subpasses;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = NULL;

    resultVulkan(vkCreateRenderPass(*allInOne.pDevice, &renderPassCreateInfo, allInOne.pAllocationCallbacks, pRenderPass), code, 0);

    //printf("renderPass created\n");
}