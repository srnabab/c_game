#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_collection.h"

#include "G_log.h"

extern VK_ALL allInOne;

void createFrameBuffer(uint32_t imageCount, Uint32 width, Uint32 height, Uint32 attachmentCount, VkImageView * pImageViews, VkImageView * pSwapchainImageView, VkRenderPass renderPass, VkFramebuffer ** pSwapchainFramebuffer)
{
    Uint32 i;
    VkImageView * pImageViewAttachments = (VkImageView *)SDL_malloc(attachmentCount * sizeof(VkImageView));

    *pSwapchainFramebuffer = (VkFramebuffer *)SDL_calloc(imageCount, sizeof(VkFramebuffer));
    print("framebuffer width: %u, height: %u\n", allInOne.extent2D.width, allInOne.extent2D.height);

    if (pSwapchainImageView == NULL)
    {
        for (i = 0;i < attachmentCount;i++)
        {
            pImageViewAttachments[i] = pImageViews[i];
        }

        for (i = 0;i < imageCount;i++)
        {

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.pNext = NULL;
            framebufferInfo.flags = 0;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = attachmentCount;
            framebufferInfo.pAttachments = pImageViewAttachments;
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            resultVulkan(vkCreateFramebuffer(allInOne.device, &framebufferInfo, allInOne.pAllocationCallbacks, &((*pSwapchainFramebuffer)[i])), 2, pImageViewAttachments, *pSwapchainFramebuffer);
        }
    }
    else
    {
        for (i = 1;i < attachmentCount;i++)
        {
            pImageViewAttachments[i] = pImageViews[i - 1];
        }

        for (i = 0;i < imageCount;i++)
        {
            pImageViewAttachments[0] = pSwapchainImageView[i];

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.pNext = NULL;
            framebufferInfo.flags = 0;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = attachmentCount;
            framebufferInfo.pAttachments = pImageViewAttachments;
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            resultVulkan(vkCreateFramebuffer(allInOne.device, &framebufferInfo, allInOne.pAllocationCallbacks, &((*pSwapchainFramebuffer)[i])), 2, pImageViewAttachments, *pSwapchainFramebuffer);
        }
    }
    //printf("swapchain framebuffer created\n");
}
void createFrameBufferByImageArray(Uint32 imageCount, Uint32 width, Uint32 height, VkImageView * pImageViews, VkRenderPass renderPass, VkFramebuffer ** pFrameBuffer)
{
    Uint32 i;
    *pFrameBuffer = (VkFramebuffer *)SDL_calloc(imageCount, sizeof(VkFramebuffer));

    for (i = 0;i < imageCount;i++)
    {
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = NULL;
        framebufferInfo.flags = 0;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = pImageViews + i;
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        resultVulkan(vkCreateFramebuffer(allInOne.device, &framebufferInfo, allInOne.pAllocationCallbacks, (*pFrameBuffer) + i), 1, *pFrameBuffer);
    }
}
void destroyedFrameBuffer(uint32_t imageCount, VkFramebuffer * pSwapchainFramebuffer)
{
    for (uint32_t i = 0;i < imageCount;i++)
    {
        vkDestroyFramebuffer(allInOne.device, pSwapchainFramebuffer[i], allInOne.pAllocationCallbacks);
    }
} 