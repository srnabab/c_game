#include "vk_framebuffer.h"

void createFrameBuffer(VkDevice * pDevice, VkExtent2D * pExtent2D, uint32_t imageCount, VkImageView * pSwapchainImageView, VkImageView * pDepthImageView, VkRenderPass * pRenderPass, VkFramebuffer ** pSwapchainFramebuffer)
{
    FuncCode code = createFrameBufferF;

    *pSwapchainFramebuffer = (VkFramebuffer *)calloc(imageCount, sizeof(VkFramebuffer));
    printf("framebuffer width: %u, height: %u\n", pExtent2D->width, pExtent2D->height);

    for (int i = 0;i < imageCount;i++)
    {
        VkImageView attachments[2] = {pSwapchainImageView[i], *pDepthImageView};

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = VK_NULL_HANDLE;
        framebufferInfo.flags = 0;
        framebufferInfo.renderPass = *pRenderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = pExtent2D->width;
        framebufferInfo.height = pExtent2D->height;
        framebufferInfo.layers = 1;

        resultVulkan(vkCreateFramebuffer(*pDevice, &framebufferInfo, VK_NULL_HANDLE, &((*pSwapchainFramebuffer)[i])), code, 0);
    }

    //printf("swapchain framebuffer created\n");
}
void destroyedFrameBuffer(VkDevice * pDevice, uint32_t imageCount, VkFramebuffer * pSwapchainFramebuffer)
{
    for (int i = 0;i < imageCount;i++)
    {
        vkDestroyFramebuffer(*pDevice, pSwapchainFramebuffer[i], VK_NULL_HANDLE);
    }
} 