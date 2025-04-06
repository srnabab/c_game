#include "G_constants.h"

#include "vk_code_h/vk_recreate.h"
#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_collection.h"
#include "vk_code_h/vk_descriptorPool.h"

#include "G_log.h"

extern VK_ALL allInOne;

void recreateSwapchain(void)
{
    VkDevice * pDevice = allInOne.pDevice;

    resultVulkan(vkDeviceWaitIdle(*pDevice), 0);
 
    VkImageView * oldSwapchainImageView3D = *allInOne.ppSwapchain3DImageViews;
    void * oldSwapchain3DImages = *allInOne.ppSwapchain3DImages;
    Uint32 oldImageCount3D = *allInOne.pImageCount3D;
    VkSwapchainKHR oldSwapchain3D = *allInOne.pSwapchain3D;

    getSurfaceCapabilities(*allInOne.pSurface3D, allInOne.pSurface3DCapabilities);
    getSurfaceFormats(*allInOne.pSurface3D, allInOne.pSurface3DFormat);

    createSwapchain(*allInOne.pSurface3D, *allInOne.pSurface3DCapabilities, *allInOne.pSurface3DFormat, *allInOne.pPresentMode3D, allInOne.pSwapchain3D, oldSwapchain3D);
    CO_addSwapchain(*allInOne.pSwapchain3D);
    
    getSwapchainNumber(*allInOne.pSwapchain3D, allInOne.pImageCount3D);

    createSwapchainImage(*allInOne.pSwapchain3D, allInOne.pImageCount3D, allInOne.ppSwapchain3DImages);
    CO_addSwapchainImage(*allInOne.ppSwapchain3DImages);

    createImageViews(*allInOne.ppSwapchain3DImages, *allInOne.pImageCount3D, allInOne.pSurface3DFormat->format, VK_IMAGE_ASPECT_COLOR_BIT, allInOne.ppSwapchain3DImageViews);
    CO_addSwapchainImageView(*allInOne.pImageCount3D, *allInOne.ppSwapchain3DImageViews);

    unloadTexture(TEXTURE_MODEL_DEPTH);
    unloadTexture(TEXTURE_NORMAL);
    unloadTexture(TEXTURE_MODEL_COLOR);
    unloadTexture(TEXTURE_SHADOW_MAP);
    unloadTexture(TEXTURE_SSGI_STORAGE_IMAGE);

    loadDepthResource(TEXTURE_MODEL_DEPTH, true);
    loadNormalResource(TEXTURE_NORMAL);
    loadImageResource(allInOne.pSurface3DFormat->format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_MODEL_COLOR, NULL);
    loadImageResource(VK_FORMAT_R16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_SHADOW_MAP, NULL);
    loadImageResource(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_GENERAL, TEXTURE_SSGI_STORAGE_IMAGE\
        , *allInOne.ppSSGIDescriptorSets + 2);

    G_Texture_P * modelDepthTexutre = getTexture(TEXTURE_MODEL_DEPTH);
    G_Texture_P * modelNormalTexture = getTexture(TEXTURE_NORMAL);
    G_Texture_P * modelColorTexture = getTexture(TEXTURE_MODEL_COLOR);
    G_Texture_P * seprateShadowTexture = getTexture(TEXTURE_SHADOW_MAP);

    addDescriptorSetToTexture(TEXTURE_MODEL_DEPTH, *allInOne.ppSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_NORMAL, *allInOne.ppSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, *allInOne.ppSSGIDescriptorSets + 0);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_DEPTH, *allInOne.pDepthSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_NORMAL, *allInOne.pNormalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_MODEL_COLOR, *allInOne.pTextureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, TEXTURE_SSGI_STORAGE_IMAGE, NULL, VK_IMAGE_LAYOUT_GENERAL);

    executeUpdateDescriptorSets();
 
    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, *allInOne.ppCombineDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_SSGI_STORAGE_IMAGE, *allInOne.ppCombineDescriptorSets + 0);
    // combine
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_COLOR, *allInOne.pTextureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_SSGI_STORAGE_IMAGE, *allInOne.pTextureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16

    executeUpdateDescriptorSets();


    VkImageView modelImageViews[] = {modelColorTexture->imageView, modelNormalTexture->imageView, seprateShadowTexture->imageView, modelDepthTexutre->imageView};
    CO_cleanFramebuffer(2, *allInOne.ppDirectColorFramebuffer);
    createFrameBuffer(2, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 4, modelImageViews, NULL, allInOne.pModelRenderPass, allInOne.ppDirectColorFramebuffer);
    CO_addFrameBuffer(2, *allInOne.ppDirectColorFramebuffer);// CO

    CO_cleanFramebuffer(oldImageCount3D, *allInOne.ppCombineFramebuffer);
    createFrameBuffer(*allInOne.pImageCount3D, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 1, NULL, *allInOne.ppSwapchain3DImageViews, allInOne.pCombineRenderPass, allInOne.ppCombineFramebuffer);
    CO_addFrameBuffer(*allInOne.pImageCount3D, *allInOne.ppCombineFramebuffer);// CO

    CO_cleanFramebuffer(oldImageCount3D, *allInOne.ppGraphic2dFramebuffer);
    createFrameBuffer(*allInOne.pImageCount3D, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 1, NULL, *allInOne.ppSwapchain3DImageViews, allInOne.pRenderPass, allInOne.ppGraphic2dFramebuffer);
    CO_addFrameBuffer(*allInOne.pImageCount3D, *allInOne.ppGraphic2dFramebuffer);// CO
 
    CO_cleanSwapchainImageView(oldImageCount3D, oldSwapchainImageView3D);
    CO_cleanSwapchainImage(oldSwapchain3DImages);
    CO_cleanSwapchain(oldSwapchain3D);

    print("recreate swapchain");
}