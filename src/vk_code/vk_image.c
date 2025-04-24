#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_drawTool.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_collection.h"

#include "G_log.h"

extern VK_ALL allInOne;

#define IDENTITY_COMPONENT ((VkComponentMapping){VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY})

VkResult _createImage(void * pNext, VkImageCreateFlags flags, VkImageType imageType, VkFormat format, VkExtent3D extent, Uint32 mipLevels, Uint32 arrayLayers, VkSampleCountFlagBits samples\
, VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharingMode, Uint32 queueFamilyIndexCount, const Uint32 * pQueueFamilyIndices, VkImageLayout initialLayout, VkImage * pImage\
, VkMemoryPropertyFlags properties, VkDeviceMemory * pImageMem)
{
    VkResult result = VK_SUCCESS;

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = pNext;
    imageInfo.flags = flags;
    imageInfo.imageType = imageType;
    imageInfo.format = format;
    imageInfo.extent = extent;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.samples = samples;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = sharingMode;
    imageInfo.queueFamilyIndexCount = queueFamilyIndexCount;
    imageInfo.pQueueFamilyIndices = pQueueFamilyIndices;
    imageInfo.initialLayout = initialLayout;

    result |= vkCreateImage(allInOne.device, &imageInfo, allInOne.pAllocationCallbacks, pImage);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(allInOne.device, *pImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result |= vkAllocateMemory(allInOne.device, &allocInfo, allInOne.pAllocationCallbacks, pImageMem);

    result |= vkBindImageMemory(allInOne.device, *pImage, *pImageMem, 0);

    return result;
}
VkResult createImage(Uint32 width, Uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage * pImage, VkDeviceMemory * pImageMem)
{
    VkResult res = _createImage(NULL, 0, VK_IMAGE_TYPE_2D, format, (VkExtent3D){width, height, 1}, 1, 1, VK_SAMPLE_COUNT_1_BIT, tiling, usage, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, VK_IMAGE_LAYOUT_UNDEFINED, pImage, properties, pImageMem);
    return res;
}
VkResult createImageArray(Uint32 width, Uint32 height, Uint32 arrayLayers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage * pImage, VkDeviceMemory * pImageMem)
{
    VkResult res = _createImage(NULL, 0, VK_IMAGE_TYPE_2D, format, (VkExtent3D){width, height, 1}, 1, arrayLayers, VK_SAMPLE_COUNT_1_BIT, tiling, usage, VK_SHARING_MODE_EXCLUSIVE, 0, NULL, VK_IMAGE_LAYOUT_UNDEFINED, pImage, properties, pImageMem);
    transitionImageLayout(NULL, *pImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, arrayLayers);
    return res;
}
VkResult _createImageView(void * pNext, VkImageViewCreateFlags flags, VkImage image, VkImageViewType viewType, VkFormat format, VkComponentMapping components, VkImageAspectFlags aspectFlags\
    , Uint32 baseMipLevel, Uint32 levelCount, Uint32 baseArrayLayer, Uint32 layerCount, VkImageView * pImageView)
{
    VkResult result = VK_SUCCESS;

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = pNext;
    viewInfo.flags = flags;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.components = components;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
    viewInfo.subresourceRange.levelCount = levelCount;
    viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
    viewInfo.subresourceRange.layerCount = layerCount;


    result |= vkCreateImageView(allInOne.device, &viewInfo, allInOne.pAllocationCallbacks, pImageView);

    return result;
}
VkResult createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pImageView)
{
    VkResult result = _createImageView(NULL, 0, image, VK_IMAGE_VIEW_TYPE_2D, format, IDENTITY_COMPONENT, aspectFlags, 0, 1, 0, 1, pImageView);

    return result;
}
VkResult createImageViews(VkImage * pImages, Uint32 imageCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView ** ppImageView)
{
    VkResult result = VK_SUCCESS;

    *ppImageView = (VkImageView *)SDL_calloc(imageCount, sizeof(VkImageView));

    for (Uint32 i = 0;i < imageCount;i++)
    {
        result |= createImageView(pImages[i], format, aspectFlags, (*ppImageView) + i);
    }

    return result;
}
VkResult createImageViewsForImageArray(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, Uint32 imageViewCount, VkImageView ** ppImageView)
{
    VkResult result = VK_SUCCESS;

    *ppImageView = (VkImageView *)SDL_calloc(imageViewCount, sizeof(VkImageView));

    for (Uint32 i = 0;i < imageViewCount;i++)
    {
        result |= _createImageView(NULL, 0, image, VK_IMAGE_VIEW_TYPE_2D, format, IDENTITY_COMPONENT, aspectFlags, 0, 1, i, 1, (*ppImageView) + i);
        CO_addImageView((*ppImageView)[i]);
    }

    return result;
}
VkResult createImageViewArray(VkImage image, Uint32 layerCount, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView * pImageView)
{
    VkResult result = VK_SUCCESS;

    result |= _createImageView(NULL, 0, image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, format, IDENTITY_COMPONENT, aspectFlags, 0, 1, 0, layerCount, pImageView);

    return result;
}
void destroyImageViews(VkImageView * pImageView, Uint32 imageCount)
{
    for (Uint32 i = 0;i < imageCount;i++)
    {
        vkDestroyImageView(allInOne.device, pImageView[i], allInOne.pAllocationCallbacks);
    }
}
VkResult transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Uint32 baseArrayLayer, Uint32 layerCount)
{
    VkResult result  = VK_SUCCESS;

    VkCommandBuffer singleCommandBuffer = NULL;
    if (commandBuffer == NULL)
    {
        result |= beginSingleTimeCommands(allInOne.graphicCommandPool, &singleCommandBuffer);
    }

    VkAccessFlags srcAccessMask = 0;
    VkAccessFlags dstAccessMask = 0;
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL || oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) 
    {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) 
        {
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) 
    {
        srcAccessMask = 0;
        
        if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_GENERAL)
        {
            dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
        else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
    {
        srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else 
    {
        print("unsupported layout transition!");
    }

    VkImageMemoryBarrier barrier = {};
    _setImageMemoryBarrier(NULL, srcAccessMask, dstAccessMask, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, aspectMask, 0, 1, baseArrayLayer, layerCount, &barrier);

    if (commandBuffer == NULL)
    {
        vkCmdPipelineBarrier(singleCommandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);

        result |= endSingleTimeCommands(allInOne.graphicCommandPool, getGraphic2dQueue(), &singleCommandBuffer);
    }
    else
    {
        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
    }

    return result;
}
bool hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}