#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_log.h"

extern VK_ALL allInOne;

void createFrameBuffer(uint32_t imageCount, Uint32 width, Uint32 height, Uint32 attachmentCount, VkImageView * pImageViews, VkImageView * pSwapchainImageView, VkRenderPass * pRenderPass, VkFramebuffer ** pSwapchainFramebuffer)
{
    Uint32 i;
    VkImageView * pImageViewAttachments = (VkImageView *)SDL_malloc(attachmentCount * sizeof(VkImageView));

    *pSwapchainFramebuffer = (VkFramebuffer *)SDL_calloc(imageCount, sizeof(VkFramebuffer));
    logMessage("framebuffer width: %u, height: %u\n", allInOne.pExtent2D->width, allInOne.pExtent2D->height);

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
            framebufferInfo.renderPass = *pRenderPass;
            framebufferInfo.attachmentCount = attachmentCount;
            framebufferInfo.pAttachments = pImageViewAttachments;
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            resultVulkan(vkCreateFramebuffer(*allInOne.pDevice, &framebufferInfo, allInOne.pAllocationCallbacks, &((*pSwapchainFramebuffer)[i])), 2, pImageViewAttachments, *pSwapchainFramebuffer);
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
            framebufferInfo.renderPass = *pRenderPass;
            framebufferInfo.attachmentCount = attachmentCount;
            framebufferInfo.pAttachments = pImageViewAttachments;
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            resultVulkan(vkCreateFramebuffer(*allInOne.pDevice, &framebufferInfo, allInOne.pAllocationCallbacks, &((*pSwapchainFramebuffer)[i])), 2, pImageViewAttachments, *pSwapchainFramebuffer);
        }
    }
    //printf("swapchain framebuffer created\n");
}
void destroyedFrameBuffer(uint32_t imageCount, VkFramebuffer * pSwapchainFramebuffer)
{
    for (uint32_t i = 0;i < imageCount;i++)
    {
        vkDestroyFramebuffer(*allInOne.pDevice, pSwapchainFramebuffer[i], allInOne.pAllocationCallbacks);
    }
} 