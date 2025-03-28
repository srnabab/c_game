#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_format.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_log.h"

extern VK_ALL allInOne;

void findDepthFormat(VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pDepthFormat)
{
    VkFormat candiates[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    findSupportFormat(candiates, allInOne.pPhysicalDevice, tiling, features, pDepthFormat);
}
VkFormat createDepthResoures(VkImage * pDepthImage, VkDeviceMemory * pDepthImageMem, VkImageView * pDepthImageView)
{
    FuncCode code = createDepthResouresF;

    VkFormat depthFormat = 0;;
    findDepthFormat(VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, &depthFormat);
    
    resultVulkan(createImage(allInOne.pExtent2D->width, allInOne.pExtent2D->height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pDepthImage, pDepthImageMem), code, 0);

    resultVulkan(createImageView(pDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, pDepthImageView), code, 0);

    resultVulkan(transitionImageLayout(pDepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL), code, 0);

    return depthFormat;
}