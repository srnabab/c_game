#include "vk_framebuffer.h"

void createFrameBuffer(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t imageCount, VkImageView * pSwapchainImageView, VkImageView * pDepthImageView, VkRenderPass * pRenderPass, VkFramebuffer ** pSwapchainFramebuffer)
{
    FuncCode code = createFrameBufferF;

    *pSwapchainFramebuffer = (VkFramebuffer *)SDL_calloc(imageCount, sizeof(VkFramebuffer));
    SDL_Log("framebuffer width: %u, height: %u\n", pExtent2D->width, pExtent2D->height);

    for (uint32_t i = 0;i < imageCount;i++)
    {
        VkImageView attachments[2] = {pSwapchainImageView[i], *pDepthImageView};

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = NULL;
        framebufferInfo.flags = 0;
        framebufferInfo.renderPass = *pRenderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = pExtent2D->width;
        framebufferInfo.height = pExtent2D->height;
        framebufferInfo.layers = 1;

        resultVulkan(vkCreateFramebuffer(*pDevice, &framebufferInfo, NULL, &((*pSwapchainFramebuffer)[i])), code, 0);
    }

    //printf("swapchain framebuffer created\n");
}
void destroyedFrameBuffer(VkDevice * pDevice, uint32_t imageCount, VkFramebuffer * pSwapchainFramebuffer)
{
    for (uint32_t i = 0;i < imageCount;i++)
    {
        vkDestroyFramebuffer(*pDevice, pSwapchainFramebuffer[i], NULL);
    }
} 