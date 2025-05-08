#include "G_constants.h"

#include "vk_code_h/vk_buffer.h"
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
#include "G_struct.h"

extern VK_ALL allInOne;
extern G_SYNC allSync;

void recreateSwapchain(Uint32 currentFrame)
{
    resultVulkan(vkDeviceWaitIdle(allInOne.device), 0);
 
    VkImageView * oldSwapchainImageView3D = allInOne.pSwapchain3DImageViews;
    void * oldSwapchain3DImages = allInOne.pSwapchain3DImages;
    Uint32 oldImageCount3D = allInOne.imageCount3D;
    VkSwapchainKHR oldSwapchain3D = allInOne.swapchain3D;

    getSurfaceCapabilities(allInOne.surface3D, &allInOne.surface3DCapabilities);
    getSurfaceFormats(allInOne.surface3D, &allInOne.surface3DFormat);

    createSwapchain(allInOne.surface3D, allInOne.surface3DCapabilities, allInOne.surface3DFormat, allInOne.presentMode3D, &allInOne.swapchain3D, oldSwapchain3D);
    CO_addSwapchain(allInOne.swapchain3D);
    
    getSwapchainNumber(allInOne.swapchain3D, &allInOne.imageCount3D);

    createSwapchainImage(allInOne.swapchain3D, &allInOne.imageCount3D, &allInOne.pSwapchain3DImages);
    CO_addSwapchainImage(allInOne.pSwapchain3DImages);

    createImageViews(allInOne.pSwapchain3DImages, allInOne.imageCount3D, allInOne.surface3DFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &allInOne.pSwapchain3DImageViews);
    CO_addSwapchainImageView(allInOne.imageCount3D, allInOne.pSwapchain3DImageViews);

    unloadTexture(TEXTURE_MODEL_DEPTH);
    unloadTexture(TEXTURE_NORMAL);
    unloadTexture(TEXTURE_MODEL_COLOR);
    unloadTexture(TEXTURE_SHADOW_MAP);
    unloadTexture(TEXTURE_SSGI_STORAGE_IMAGE);
    unloadTexture(TEXTURE_2D_COLOR);

    loadDepthResource(TEXTURE_MODEL_DEPTH, true);
    loadNormalResource(TEXTURE_NORMAL);
    loadImageResource(allInOne.surface3DFormat.format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_MODEL_COLOR, NULL);
    loadImageResource(VK_FORMAT_R16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_SHADOW_MAP, NULL);
    loadImageResource(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_GENERAL, TEXTURE_SSGI_STORAGE_IMAGE\
        , allInOne.pSSGIDescriptorSets + 2);
    loadImageResource(allInOne.surface3DFormat.format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_2D_COLOR, NULL);

    G_Texture_P * modelDepthTexutre = getTexture(TEXTURE_MODEL_DEPTH);
    G_Texture_P * modelNormalTexture = getTexture(TEXTURE_NORMAL);
    G_Texture_P * modelColorTexture = getTexture(TEXTURE_MODEL_COLOR);
    G_Texture_P * seprateShadowTexture = getTexture(TEXTURE_SHADOW_MAP);
    G_Texture_P * color2dTexture = getTexture(TEXTURE_2D_COLOR);

    addDescriptorSetToTexture(TEXTURE_MODEL_DEPTH, allInOne.pSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_NORMAL, allInOne.pSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, allInOne.pSSGIDescriptorSets + 0);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_DEPTH, allInOne.depthSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_NORMAL, allInOne.normalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_MODEL_COLOR, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, TEXTURE_SSGI_STORAGE_IMAGE, NULL, VK_IMAGE_LAYOUT_GENERAL);

    executeUpdateDescriptorSets();
 
    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, allInOne.pCombineDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_SSGI_STORAGE_IMAGE, allInOne.pCombineDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_2D_COLOR, allInOne.pCombineDescriptorSets + 0);
    // combine
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_COLOR, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_SSGI_STORAGE_IMAGE, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_2D_COLOR, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16

    executeUpdateDescriptorSets();


    VkImageView modelImageViews[] = {modelColorTexture->imageView, modelNormalTexture->imageView, seprateShadowTexture->imageView, modelDepthTexutre->imageView};

    SDL_LockMutex(allSync.renderMutex);
    CO_cleanFramebuffer(2, allInOne.pDirectColorFramebuffer);
    CO_cleanFramebuffer(oldImageCount3D, allInOne.pCombineFramebuffer);
    CO_cleanFramebuffer(2, allInOne.pGraphic2dFramebuffer);

    createFrameBuffer(2, allInOne.extent2D.width, allInOne.extent2D.height, 4, modelImageViews, NULL, allInOne.modelRenderPass, &allInOne.pDirectColorFramebuffer);
    CO_addFrameBuffer(2, allInOne.pDirectColorFramebuffer);// CO

    createFrameBuffer(allInOne.imageCount3D, allInOne.extent2D.width, allInOne.extent2D.height, 1, NULL, allInOne.pSwapchain3DImageViews, allInOne.combineRenderPass, &allInOne.pCombineFramebuffer);
    CO_addFrameBuffer(allInOne.imageCount3D, allInOne.pCombineFramebuffer);// CO

    createFrameBuffer(2, allInOne.extent2D.width, allInOne.extent2D.height, 1, &color2dTexture->imageView, NULL, allInOne.renderPass, &allInOne.pGraphic2dFramebuffer);
    CO_addFrameBuffer(2, allInOne.pGraphic2dFramebuffer);// CO
    SDL_UnlockMutex(allSync.renderMutex);

    releaseBufferFromQueue(allInOne.graphicCommandPool, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.queueFamilyIndices.computeFamily.familyIndice\
        , allInOne.pShaderStorageBuffer[(currentFrame + 1) % MAX_FRAMES_IN_FLIGHT], sizeof(Particle) * PARTICLE_COUNT);
 
    CO_cleanSwapchainImageView(oldImageCount3D, oldSwapchainImageView3D);
    CO_cleanSwapchainImage(oldSwapchain3DImages);
    CO_cleanSwapchain(oldSwapchain3D);

    print("recreate swapchain");
}