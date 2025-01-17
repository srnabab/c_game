#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_buffer.h"

VkResult createImage(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage * pImage, VkDeviceMemory * pImageMem)
{
    VkResult result = VK_SUCCESS;

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = NULL;
    imageInfo.flags = 0;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices = NULL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    result |= vkCreateImage(*pDevice, &imageInfo, NULL, pImage);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(*pDevice, *pImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(pPhysicalDevice, memRequirements.memoryTypeBits, properties);

    result |= vkAllocateMemory(*pDevice, &allocInfo, NULL, pImageMem);

    result |= vkBindImageMemory(*pDevice, *pImage, *pImageMem, 0);

    return result;
}
VkResult createImageView(VkDevice * pDevice, VkImage * pImage, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pImageView)
{
    VkResult result = VK_SUCCESS;

    VkImageViewCreateInfo viewInfo = {};

    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = NULL;
    viewInfo.flags = 0;
    viewInfo.image = *pImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    result |= vkCreateImageView(*pDevice, &viewInfo, NULL, pImageView);

    return result;
}
VkResult createImageViews(VkDevice * pDevice, VkImage ** ppImages, uint32_t imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView)
{
    VkResult result = VK_SUCCESS;

    VkImageViewCreateInfo * imageViewCreatInfo = (VkImageViewCreateInfo *)SDL_malloc(imageCount * sizeof(VkImageViewCreateInfo));

    //printf("pImageview: %p\n", pSwapchainImageView);
    //printf("imageview: %p\n", *pSwapchainImageView);
    *ppImageView = (VkImageView *)SDL_calloc(imageCount, sizeof(VkImageView));

    for (uint32_t i = 0;i < imageCount;i++)
    {
        result |= createImageView(pDevice, &(*ppImages)[i], format, aspectFlags, &(*ppImageView)[i]);
    }

    SDL_free(imageViewCreatInfo);
    //printf("swapchainImageView created\n");

    return result;
}
void destroyImageViews(VkDevice * pDevice, VkImageView * pImageView, uint32_t imageCount)
{
    for (uint32_t i = 0;i < imageCount;i++)
    {
        vkDestroyImageView(*pDevice, pImageView[i], NULL);
    }
}
VkResult transitionImageLayout(VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphcisQueue, VkImage * pImage, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkResult result  = VK_SUCCESS;

    VkCommandBuffer commandBuffer = NULL;
    result |= beginSingleTimeCommands(pDevice, pCommandPool, &commandBuffer);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = NULL;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = *pImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) 
        {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);

    result |= endSingleTimeCommands(pDevice, pCommandPool, pGraphcisQueue, &commandBuffer);

    return result;
}
bool hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}