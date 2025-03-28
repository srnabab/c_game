#include "vk_code_h/vk_format.h"
#include "G_log.h"

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
            return;
        } 
        else if ((tiling == VK_IMAGE_TILING_OPTIMAL) && (props.optimalTilingFeatures & features)) 
        {
            *pFormat = candiates[i];
            return;
        }
    }

    if (*pFormat == 0) print("falied to find supported format!\n");
}