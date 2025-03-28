#include "vk_code_h/vk_normal.h"
#include "vk_code_h/vk_format.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "G_log.h"

extern VK_ALL allInOne;

void findNormalFormat(VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat * pNormalFormat)
{
    VkFormat candiates[3] = {VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT, VK_FORMAT_R16G16B16A16_SNORM};
    findSupportFormat(candiates, allInOne.pPhysicalDevice, tiling, features, pNormalFormat);
}
VkFormat createNormalResoures(VkImage * pNormalImage, VkDeviceMemory * pNormalImageMem, VkImageView * pNormalImageView)
{
    FuncCode code = createDepthResouresF;

    VkFormat normalFormat = 0;
    findNormalFormat(VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, &normalFormat);
    
    resultVulkan(createImage(allInOne.pExtent2D->width, allInOne.pExtent2D->height, normalFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pNormalImage, pNormalImageMem), code, 0);

    resultVulkan(createImageView(pNormalImage, normalFormat, VK_IMAGE_ASPECT_COLOR_BIT, pNormalImageView), code, 0);

    resultVulkan(transitionImageLayout(pNormalImage, normalFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL), code, 0);

    return normalFormat;
}