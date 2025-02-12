#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_struct.h"
#include "vk_code_h/vk_judge.h"

#include "G_log.h"

extern VK_ALL allInOne;

void findSupportFormat(VkFormat * candiates, VkPhysicalDevice * pPhysicalDevice, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pFormat)
{
    for (int i = 0;i < 3;i++)
    {
        VkFormatProperties props = {};
        vkGetPhysicalDeviceFormatProperties(*pPhysicalDevice, candiates[i], &props);
        //printf("1: %d, 2: %d\n", props.linearTilingFeatures & features, props.optimalTilingFeatures & features);

        if ((tiling == VK_IMAGE_TILING_LINEAR) && (props.linearTilingFeatures & features)) 
        {
            *pFormat = candiates[i];
            break;
        } 
        else if ((tiling == VK_IMAGE_TILING_OPTIMAL) && (props.optimalTilingFeatures & features)) 
        {
            *pFormat = candiates[i];
            break;
        }
    }

    if (*pFormat == 0)
        logMessage("falied to find supported format!\n");
}
void findDepthFormat(VkImageTiling tiling, VkFormatFeatureFlags features)
{
    VkFormat candiates[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    findSupportFormat(candiates, allInOne.pPhysicalDevice, tiling, features, allInOne.pDepthFormat);
}
void createDepthResoures(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkExtent2D * pExtent2D, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkImage * pDepthImage, VkDeviceMemory * pDepthImageMem, VkImageView * pDepthImageView)
{
    FuncCode code = createDepthResouresF;

    findDepthFormat(VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    
    resultVulkan(createImage(pExtent2D->width, pExtent2D->height, *allInOne.pDepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pDepthImage, pDepthImageMem), code, 0);

    resultVulkan(createImageView(pDepthImage, *allInOne.pDepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, pDepthImageView), code, 0);

    resultVulkan(transitionImageLayout(pCommandPool, pGraphicQueue, pDepthImage, *allInOne.pDepthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL), code, 0);
}