#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_struct.h"

#include "G_log.h"

extern VK_ALL allInOne;

void createFrameBuffer(uint32_t imageCount, VkImageView * pSwapchainImageView, VkImageView const * pDepthImageView, VkRenderPass * pRenderPass, VkFramebuffer ** pSwapchainFramebuffer)
{
    FuncCode code = createFrameBufferF;

    *pSwapchainFramebuffer = (VkFramebuffer *)SDL_calloc(imageCount, sizeof(VkFramebuffer));
    logMessage("framebuffer width: %u, height: %u\n", allInOne.pExtent2D->width, allInOne.pExtent2D->height);

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
        framebufferInfo.width = allInOne.pExtent2D->width;
        framebufferInfo.height = allInOne.pExtent2D->height;
        framebufferInfo.layers = 1;

        resultVulkan(vkCreateFramebuffer(*allInOne.pDevice, &framebufferInfo, allInOne.pAllocationCallbacks, &((*pSwapchainFramebuffer)[i])), code, 0);
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