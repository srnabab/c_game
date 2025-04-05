#include "G_constants.h"
#include "G_log.h"
#include "G_graphic.h"
#include "G_resource.h"
#include "G_TileMap/G_TileSet.h"
#include "G_staticModel.h"
#include "G_custom_math.h"

#include "SDL3/SDL_video.h"
#include "SDL3/SDL_vulkan.h"

#include "vk_code_h/vk_device.h"
#include "vk_code_h/vk_instance.h"
#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_commandPool.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_descriptorPool.h"
#include "vk_code_h/vk_synchronize.h"
#include "vk_code_h/vk_uniform.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_load_model.h"
#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_sampler.h"
#include "vk_code_h/vk_computeShader.h"
#include "vk_code_h/vk_shader.h"
#include "vk_code_h/vk_renderPass.h"
#include "vk_code_h/vk_pipeline.h"
#include "vk_code_h/vk_vertex.h"
#include "vk_code_h/vk_index.h"
#include "vk_code_h/vk_alloc_func.h"
#include "vk_code_h/vk_version.h"
#include "vk_code_h/vk_surface.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_collection.h"
#include "vk_code_h/vk_judge.h"

#include "spirv_reflect/shader_resolve.h"

static bool initSDL(void)
{
    /*initialize sdl
    timer, audio, video, event, joysitck, haptic, gamecontroller, sensor*/
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) 
    {
        //todo
        //add result juadge for sdl

        //show a messagebox for error informations
        int temp = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "", "Error initializing SDL.\n", NULL);
        if (!temp)
        {
            logMessage("Error show messagebox\n");
            return false;
        }
        return false;
    }

    return true;
}

// declare a sdl window_3D
SDL_Window * window_3D = NULL;
SDL_DisplayID displayId = 0;

// windows' width and height
Uint32 width = 800;
Uint32 height = 600;

float physicalCoffectX = 1.0f;
float physicalCoffectY = 1.0f;

bool initWindow_3D(void)
{
   //create sdl window_3D and sign window_3D as a vulkan window_3D
    if (!initSDL()) return false;
    window_3D = SDL_CreateWindow("Vulkan_3D", width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (window_3D == NULL)
        return false;

    CO_addWindow(window_3D);
    
    int count = 0;
    SDL_DisplayID * displays = SDL_GetDisplays(&count);
    for (int i = 0;i < count;i++)
    {
        logMessage("display id:%u, name:%s", displays[i], SDL_GetDisplayName(displays[i]));
    }
    displayId = displays[0];
    SDL_DisplayMode ** modes = NULL;
    modes = SDL_GetFullscreenDisplayModes(displayId, &count);
    for (int i = 0;i < count;i++)
    {
        logMessage("id %u format: %u: %u x %u, %uHz", modes[i]->displayID, modes[i]->format, modes[i]->w, modes[i]->h, modes[i]->refresh_rate);
    }

    physicalCoffectX = (float)width / LOGICAL_WIDTH;
    physicalCoffectY = (float)height / LOGICAL_HEIGHT;

    logMessage("window_3D initialized");

    Uint32 iconWidth, iconHeight;
    iconWidth = iconHeight = 0;         
    uint8_t iconChannel;
    void * iconPixels = readPNG(IconPng, &iconWidth, &iconHeight, &iconChannel);
    SDL_Surface * iconSurface = SDL_CreateSurfaceFrom(iconWidth, iconHeight, SDL_PIXELFORMAT_RGBA32, iconPixels, iconWidth * iconChannel);
    if (iconSurface == NULL)
    {
        SDL_free(iconPixels);
        return false;
    }

    if (!SDL_SetWindowIcon(window_3D, iconSurface))
        return false;

    SDL_DestroySurface(iconSurface);
    SDL_free(iconPixels);

    return true;
}

static VkInstance instance = NULL;

static VkPhysicalDevice physicalDevice = NULL;

static QueueFamilyIndices queueIndices = {};
static VkDevice device = NULL;
static VkQueue graphicQueue = NULL;
static VkQueue presentQueue = NULL;
static VkQueue computeQueue = NULL;
static VkQueue transferQueue = NULL;

static VkAllocationCallbacks SDL_allocationCallBacks = {};

static VkExtent2D extent2D = {};
static VkExtent2D oldExtent2D = {};

static VkFormat swapchainFormat = 0;

static VkFramebuffer * graphic2DFramebuffer = NULL;

static VkFramebuffer * shadowFrameBuffer = NULL;
static VkFramebuffer * directColorFramebuffer = NULL;
static VkFramebuffer * combineFrameBuffer = NULL;

static VkPipelineShaderStageCreateInfo * graphciShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * modelShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * shadowShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * particleShaderStageCreateInfo = NULL;

// static VkShaderModule compShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * computeShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * SSGIShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * combineShaderStageCreateInfo = NULL;

static VkDescriptorSetLayout * graphicDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * modelDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * shadowDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * particleDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * computeDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * SSGIDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * combineDescriptorSetLayout = NULL;

static VkPipelineLayout graphicPipelineLayout = NULL;

static VkPipelineLayout particlePipelineLayout = NULL;

static VkPipelineLayout computePipelineLayout = NULL;

static VkRenderPass renderPass = NULL;

static VkPipelineLayout modelPipelineLayout = NULL;
static VkRenderPass modelRenderPass = NULL;

static VkPipelineLayout shadwoPipelineLayout = NULL;
static VkRenderPass shadowRenderPass = NULL;

static VkPipelineLayout SSGIPipelineLayout = NULL;

static VkPipelineLayout combinePipelineLayout = NULL;
static VkRenderPass combineRenderPass = NULL;

static VkPipeline graphicPipeline = NULL;
static VkPipeline particlePipeline = NULL;
static VkPipeline computePipeline = NULL;

static VkPipeline modelPipeline = NULL;
static VkPipeline shadowPipeline = NULL;
static VkPipeline SSGIPipeline = NULL;
static VkPipeline combinePipeline = NULL;

static VkCommandPool graphicCommandPool = NULL;
static VkCommandPool presentCommandPool = NULL;
static VkCommandPool transferCommandPool = NULL;
static VkCommandPool computeCommandPool = NULL;

static VkSampler textureSampler = NULL;
static VkSampler normalSampler = NULL;
static VkSampler depthSampler = NULL;
static VkSampler shadowSampler = NULL;

static VkBuffer vertexBuffer2D[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory vertexBuffer2DMem[MAX_FRAMES_IN_FLIGHT];
static void * vertexBuffer2DMemMapped[MAX_FRAMES_IN_FLIGHT];
static Uint32 vertices2DCount = 0;
static Vertex * vertices2D = NULL;

static VkBuffer indexBuffer2D[1];
static VkDeviceMemory indexBuffer2DMem[1];
static void * indexBuffer2DMemMapped[1];
static Uint16 * indices2D = NULL;

static VkBuffer vertexBuffer3D[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory vertexBuffer3DMem[MAX_FRAMES_IN_FLIGHT];
static void * vertexBuffer3DMemMapped[MAX_FRAMES_IN_FLIGHT];
static Uint32 vertices3DCount = 0;
static Vertex4 * vertices3D = NULL;

static VkBuffer indexBuffer3D[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory indexBuffer3DMem[MAX_FRAMES_IN_FLIGHT];
static void * indexBuffer3DMemMapped[MAX_FRAMES_IN_FLIGHT];
static Uint32 indices3DCount = 0;
static Uint32 * indices3D = NULL;

static VkBuffer graphicUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory graphicUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static void* graphicUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject ubo = {};

static VkBuffer graphic3DUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory graphic3DUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static void* graphic3DUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject ubo3D = {};

static VkBuffer UIUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory UIUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static void* UIUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject uboUI = {};

static VkBuffer computeUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory computeUniformBuffersmemory[MAX_FRAMES_IN_FLIGHT];
static void* computeUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static ComputeUniformBufferObject computeUbo = {};

static VkBuffer SSGIUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory SSGIUniformBufferMem[MAX_FRAMES_IN_FLIGHT];
static void* SSGIUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static SSGIUniformBufferObject SSGIubo = {};

static VkBuffer SunUniformBuffer[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory SunUniformBufferMem[MAX_FRAMES_IN_FLIGHT];
static void* SunUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static DirectionLight  Sunubo = {};

static VkBuffer lightSpaceUniformBuffer[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory lightSpaceUniformBufferMem[MAX_FRAMES_IN_FLIGHT];
static void* lightSpaceUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static LightSpace lightSpaceubo = {};


static VkDescriptorPool graphicDescriptorPool = NULL;
static VkDescriptorSet * graphicDescriptorSets = NULL;
static VkDescriptorSet * modelDescriptorSets = NULL;

static VkDescriptorSet * particleDescriptorSets = NULL;

static VkDescriptorPool computeDescriptorPool = NULL;
static VkDescriptorSet * computeDescriptorSets = NULL;

static VkDescriptorSet * shadowDescriptorSets = NULL;
static VkDescriptorSet * SSGIDescriptorSets = NULL;
static VkDescriptorSet * combineDescriptorSets = NULL;

static VkCommandBuffer graphicCommandBuffer[MAX_FRAMES_IN_FLIGHT];
static VkCommandBuffer presentCommandBuffer[MAX_FRAMES_IN_FLIGHT];
static VkCommandBuffer computeCommandBuffer[MAX_FRAMES_IN_FLIGHT];
static VkCommandBuffer transferCommandBuffer[MAX_FRAMES_IN_FLIGHT];

static VkSemaphore timelineSemaphore1[MAX_FRAMES_IN_FLIGHT];

static VkSemaphore imageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
static VkSemaphore renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];

static VkFence graphicInFlightFence[2];
static VkFence computeInFlightFences[2];

static Uint32 currentFrame = 0;

static float camera_X = 0.0f;
static float camera_Y = 0.0f;

static float pictureX = 0;
static float pictureY = 0;

// static bool moveEnabled = false;

static VkBuffer shaderStorageBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory shaderStorageBuffersMem[MAX_FRAMES_IN_FLIGHT];

static PushConstants picturePushConstants = {0.0f};

static VkSurfaceKHR surface3D = NULL;

static VkSurfaceFormatKHR surface3DFormat = {};
static VkPresentModeKHR presentMode3D = 0;
static VkSurfaceCapabilitiesKHR surface3DCapabilities = {};

static VkSwapchainKHR swapchain3D = NULL;
static Uint32 imageCount3D = 0;

static VkImage * swapchain3DImages = NULL;
static VkImageView * swapchain3DImageViews = NULL;
static VkFramebuffer * swapchain3DFramebuffer = NULL;

static G_StaticModelPool staticModelPool = {};

//store all compoents for initialize vulkan in a struct
VK_ALL allInOne = {};

static void initializeAllInOne(void)
{
    allInOne.pAllocationCallbacks = &SDL_allocationCallBacks;

    allInOne.pInstance = &instance;

    allInOne.pPhysicalDevice = &physicalDevice;

    allInOne.pDevice = &device;

    allInOne.pQueueFamilyIndices = &queueIndices;
    allInOne.pGraphicQueue = &graphicQueue;
    allInOne.pPresentQueue = &presentQueue;
    allInOne.pComputeQueue = &computeQueue;
    allInOne.pTransferQueue = &transferQueue;
    
    allInOne.pGraphicCommandPool = &graphicCommandPool;
    allInOne.pPresentCommandPool = &presentCommandPool;
    allInOne.pComputeCommandPool = &computeCommandPool;
    allInOne.pTransferCommandPool = &transferCommandPool;

    allInOne.pExtent2D = &extent2D;
    allInOne.pOldExtent2D = &oldExtent2D;

    allInOne.pRenderPass = &renderPass;
    allInOne.pModelRenderPass = &modelRenderPass;
    allInOne.pShadowRenderPass = &shadowRenderPass;
    allInOne.pCombineRenderPass = &combineRenderPass;

    allInOne.pGraphicPipelineLayout = &graphicPipelineLayout;
    allInOne.pGraphicPipeline = &graphicPipeline;

    allInOne.pParticlePipelineLayout = &particlePipelineLayout;
    allInOne.pParticlePipeline = &particlePipeline;

    allInOne.pComputePipelineLayout = &computePipelineLayout;
    allInOne.pComputePipeline = &computePipeline;

    allInOne.pModelPipelineLayout = &modelPipelineLayout;
    allInOne.pModelPipeline = &modelPipeline;

    allInOne.pShadowPipelineLayout = &shadwoPipelineLayout;
    allInOne.pShadowPipeline = &shadowPipeline;

    allInOne.pSSGIPipelineLayout = &SSGIPipelineLayout;
    allInOne.pSSGIPipeline = &SSGIPipeline;

    allInOne.pCombinePipelineLayout = &combinePipelineLayout;
    allInOne.pCombinePipeline = &combinePipeline;

    allInOne.pSurface3D = &surface3D;
    allInOne.pSurface3DFormat = &surface3DFormat;
    allInOne.pSurface3DCapabilities = &surface3DCapabilities;
    allInOne.pPresentMode3D = &presentMode3D;
    allInOne.pSwapchain3D = &swapchain3D;
    allInOne.pImageCount3D = &imageCount3D;
    allInOne.ppSwapchain3DImages = &swapchain3DImages;
    allInOne.ppSwapchain3DImageViews = &swapchain3DImageViews;


    allInOne.ppGraphic2dFramebuffer = &graphic2DFramebuffer;
    allInOne.ppSwapchain3DFramebuffer = &swapchain3DFramebuffer;
    allInOne.ppShadowFramebuffer = &shadowFrameBuffer;
    allInOne.ppDirectColorFramebuffer = &directColorFramebuffer;
    allInOne.ppCombineFramebuffer = &combineFrameBuffer;

    allInOne.pVertexBuffer2D = &vertexBuffer2D;
    allInOne.maxVertices2DCount = (BALLCOUNT + MAX_CHARACTERS) * 4 * 2;
    allInOne.ppVertices2D = &vertices2D;
    allInOne.pVertices2DCount = &vertices2DCount;
    allInOne.pVertexBuffer2DMem = &vertexBuffer2DMem;
    allInOne.ppVertexBuffer2DMemMapped = &vertexBuffer2DMemMapped;

    allInOne.pIndexBuffer2D = &indexBuffer2D;
    allInOne.ppIndices2D = &indices2D;
    allInOne.pIndexBuffer2DMem = &indexBuffer2DMem;
    allInOne.ppIndexBuffer2DMemMapped = &indexBuffer2DMemMapped;

    allInOne.pVertexBuffer3D = &vertexBuffer3D;
    allInOne.maxVertices3DCount = (BALLCOUNT + MAX_CHARACTERS) * 4 * 2;
    allInOne.ppVertices3D = &vertices3D;
    allInOne.pVertices3DCount = &vertices3DCount;
    allInOne.pVertexBuffer3DMem = &vertexBuffer3DMem;
    allInOne.ppVertexBuffer3DMemMapped = &vertexBuffer3DMemMapped;

    allInOne.pIndexBuffer3D = &indexBuffer3D;
    allInOne.ppIndices3D = &indices3D;
    allInOne.pIndices3DCount = &indices3DCount;
    allInOne.pIndexBuffer3DMem = &indexBuffer3DMem;
    allInOne.ppIndexBuffer3DMemMapped = &indexBuffer3DMemMapped;

    allInOne.pStaticModelPool = &staticModelPool;

    allInOne.pTextureSampler = &textureSampler;

    allInOne.ppGraphicUniformBuffer = &graphicUniformBuffers;
    allInOne.pppGraphicUniformBufferMapped = &graphicUniformBufferMapped;
    allInOne.pGraphicUbo = &ubo;

    allInOne.ppGraphic3DUniformBuffer = &graphic3DUniformBuffers;
    allInOne.pppGraphic3DUniformBufferMapped = &graphic3DUniformBufferMapped;
    allInOne.pGraphic3DUbo = &ubo3D;

    allInOne.ppUIUniformBuffer = &UIUniformBuffers;
    allInOne.pppUIUniformBufferMapped = &UIUniformBufferMapped;
    allInOne.pUIUbo = &uboUI;

    allInOne.ppSSGIUniformBuffer = &SSGIUniformBuffers;
    allInOne.pppSSGIUniformBufferMapped = &SSGIUniformBufferMapped;
    allInOne.pSSGIubo = &SSGIubo;

    allInOne.ppLightSpaceUniformBuffer = &lightSpaceUniformBuffer;
    allInOne.pppLightSpaceUniformBufferMapped = &lightSpaceUniformBufferMapped;
    allInOne.pLightSpaceUbo = &lightSpaceubo;

    allInOne.ppSunUniformBuffer = &SunUniformBuffer;
    allInOne.pppSunUniformBufferMapped = &SunUniformBufferMapped;
    allInOne.pSunubo = &Sunubo;

    allInOne.ppGraphicDescriptorSets = &graphicDescriptorSets;

    allInOne.ppParticleDescriptorSets = &particleDescriptorSets;

    allInOne.pComputeUbo = &computeUbo;

    allInOne.pppComputeUniformBufferMapped = &computeUniformBufferMapped;

    allInOne.ppComputeDescriptorSets = &computeDescriptorSets;

    allInOne.ppShadowDescriptorSets = &shadowDescriptorSets;
    allInOne.ppSSGIDescriptorSets = &SSGIDescriptorSets;
    allInOne.ppCombineDescriptorSets = &combineDescriptorSets;

    allInOne.ppShaderStorageBuffers = &shaderStorageBuffers;

    allInOne.ppGraphicCommandBuffer = &graphicCommandBuffer;
    allInOne.ppPresentCommandBuffer = &presentCommandBuffer;
    allInOne.ppComputeCommandBuffer = &computeCommandBuffer;
    allInOne.ppTransferCommandBuffer = &transferCommandBuffer;

    allInOne.ppTimelineSemaphore1 = &timelineSemaphore1;
    allInOne.ppImageAvailableSemaphore = &imageAvailableSemaphore;
    allInOne.ppRenderFinishedSemaphore = &renderFinishedSemaphore;

    allInOne.ppGraphicInFlightFence = &graphicInFlightFence;

    allInOne.ppComputeInFlightFence = &computeInFlightFences;

    allInOne.pCurrentFrame = &currentFrame;

    allInOne.pCamera_X = &camera_X;
    allInOne.pCamera_Y = &camera_Y;

    allInOne.pPictureX = &pictureX;
    allInOne.pPictureY = &pictureY;

    allInOne.pPushConstants = &picturePushConstants;
}

void initVulkan(void)
{
    /*fixed compoent*/
    logMessage("initializing...");

    initGlobalTexture();

    initializeAllInOne();

    initCollection();

    SDL_allocationCallBacks.pUserData = NULL;
    SDL_allocationCallBacks.pfnAllocation = SDL_VK_alloc;
    SDL_allocationCallBacks.pfnReallocation = SDL_VK_realloc;
    SDL_allocationCallBacks.pfnFree = SDL_VK_free;
    SDL_allocationCallBacks.pfnInternalAllocation = NULL;
    SDL_allocationCallBacks.pfnInternalFree = NULL;    

    vulkanVersion();

    createInstance();
    CO_addInstance(instance);// CO

    createSurface(window_3D, &surface3D);
    CO_addSurface(surface3D);// Co

    pickPhysicalDevice();

    findQueueFamilies();
    createLogicalDevice();
    CO_addDevice(device);// CO

    createQueue(queueIndices.graphicsFamily.familyIndice, &graphicQueue);
    createQueue(queueIndices.presentFamily.familyIndice, &presentQueue);
    createQueue(queueIndices.computeFamily.familyIndice, &computeQueue);
    createQueue(queueIndices.transferFamily.familyIndice, &transferQueue);

    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.graphicsFamily.familyIndice, &graphicCommandPool);
    CO_addCommandPool(graphicCommandPool);// CO
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.presentFamily.familyIndice, &presentCommandPool);
    CO_addCommandPool(presentCommandPool);// CO
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.computeFamily.familyIndice, &computeCommandPool);
    CO_addCommandPool(computeCommandPool);// CO
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.transferFamily.familyIndice, &transferCommandPool);
    CO_addCommandPool(transferCommandPool);// CO

    getSurfaceFormats(surface3D, &surface3DFormat);
    getPresentModes(&presentMode3D);
    getSurfaceCapabilities(surface3D, &surface3DCapabilities);
    
    extent2D.width = width;
    extent2D.height = height;

    createSwapchain(surface3D, surface3DCapabilities, surface3DFormat, presentMode3D, &swapchain3D, NULL);
    CO_addSwapchain(swapchain3D);// CO

    getSwapchainNumber(swapchain3D, &imageCount3D);
    createSwapchainImage(swapchain3D, &imageCount3D, &swapchain3DImages);
    CO_addSwapchainImage(swapchain3DImages);// CO

    swapchainFormat = surface3DFormat.format;

    //swapchain3D image view
    createSwapchainImageView(swapchain3DImages, imageCount3D, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, &swapchain3DImageViews);
    CO_addSwapchainImageView(imageCount3D, swapchain3DImageViews);// CO

    loadShadowResource(TEXTURE_SHADOW, SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT);
    G_Texture_P * modelShadowTexture = getTexture(TEXTURE_SHADOW);
    createShadowRenderPass(modelShadowTexture->format, &shadowRenderPass);
    CO_addRenderPass(shadowRenderPass);// CO
    createFrameBuffer(2, SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT, 1, &modelShadowTexture->imageView, NULL, &shadowRenderPass, &shadowFrameBuffer);
    CO_addFrameBuffer(2, shadowFrameBuffer);// CO

    loadDepthResource(TEXTURE_MODEL_DEPTH, true);
    loadImageResource(VK_FORMAT_R16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_SHADOW_MAP, NULL);
    loadNormalResource(TEXTURE_NORMAL);
    loadImageResource(swapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_MODEL_COLOR, NULL);

    G_Texture_P * modelDepthTexutre = getTexture(TEXTURE_MODEL_DEPTH);
    G_Texture_P * modelNormalTexture = getTexture(TEXTURE_NORMAL);
    G_Texture_P * modelColorTexture = getTexture(TEXTURE_MODEL_COLOR);
    G_Texture_P * seprateShadowTexture = getTexture(TEXTURE_SHADOW_MAP);
    createModelRenderPass(modelColorTexture->format, modelNormalTexture->format, seprateShadowTexture->format, modelDepthTexutre->format, &modelRenderPass);
    CO_addRenderPass(modelRenderPass);// CO

    VkImageView modelImageViews[] = {modelColorTexture->imageView, modelNormalTexture->imageView, seprateShadowTexture->imageView, modelDepthTexutre->imageView};
    createFrameBuffer(2, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 4, modelImageViews, NULL, &modelRenderPass, &directColorFramebuffer);
    CO_addFrameBuffer(2, directColorFramebuffer);// CO

    loadImageResource(swapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_COMBINE_COLOR, NULL);

    createCombineRenderPass(swapchainFormat, &combineRenderPass);
    CO_addRenderPass(combineRenderPass);// CO
    createFrameBuffer(imageCount3D, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 1, NULL, swapchain3DImageViews, &combineRenderPass, &combineFrameBuffer);
    CO_addFrameBuffer(imageCount3D, combineFrameBuffer);// CO

    createGraphicRenderPass(swapchainFormat, &renderPass);
    CO_addRenderPass(renderPass);// CO
    createFrameBuffer(imageCount3D, allInOne.pExtent2D->width, allInOne.pExtent2D->height, 1, NULL, swapchain3DImageViews, &renderPass, &graphic2DFramebuffer);
    CO_addFrameBuffer(imageCount3D, graphic2DFramebuffer);// CO
 
    createTextureSampler(&textureSampler);
    CO_addSampler(textureSampler);// CO
    createNormalSampler(&normalSampler);
    CO_addSampler(normalSampler);// CO
    createDepthSampler(&depthSampler);
    CO_addSampler(depthSampler);// CO
    createShadowSampler(&shadowSampler);
    CO_addSampler(shadowSampler);// CO

    vertices2D = (Vertex*)SDL_calloc(VERTEX_COUNT_IN_BUFFER_2D, sizeof(Vertex));
    allInOne.maxVertices2DCount = VERTEX_COUNT_IN_BUFFER_2D;

    createVertexBuffer(&physicalDevice, &device, vertexBuffer2D, vertexBuffer2DMem, vertexBuffer2DMemMapped, VERTEX_COUNT_IN_BUFFER_2D * sizeof(Vertex));
    CO_addBuffer(true, vertexBuffer2D[0], vertexBuffer2DMem[0], NULL);// CO
    createVertexBuffer(&physicalDevice, &device, vertexBuffer2D + 1, vertexBuffer2DMem + 1, vertexBuffer2DMemMapped + 1, VERTEX_COUNT_IN_BUFFER_2D * sizeof(Vertex));
    CO_addBuffer(true, vertexBuffer2D[1], vertexBuffer2DMem[1], vertices2D);// CO

    indices2D = (Uint16 *)SDL_calloc(INDEX_COUNT_IN_BUFFER_2D, sizeof(Uint16 ));
    indexInitialize(indices2D, MAX_UNIT_COUNT_2D);

    createIndexBuffer(&physicalDevice, &device, indexBuffer2D, indexBuffer2DMem, indexBuffer2DMemMapped, indices2D, INDEX_COUNT_IN_BUFFER_2D, sizeof(Uint16), false);
    CO_addBuffer(false, indexBuffer2D[0], indexBuffer2DMem[0], NULL);// CO

    SDL_free(indices2D);

    vertices3D = (Vertex4*)SDL_calloc(30000, sizeof(Vertex4));
    allInOne.maxVertices3DCount = 30000;
    indices3D = (Uint32*)SDL_calloc(45000, sizeof(Uint32));
    createVertexBuffer(&physicalDevice, &device, vertexBuffer3D, vertexBuffer3DMem, vertexBuffer3DMemMapped, 30000 * sizeof(Vertex4));
    CO_addBuffer(true, vertexBuffer3D[0], vertexBuffer3DMem[0], NULL);// CO
    createVertexBuffer(&physicalDevice, &device, vertexBuffer3D + 1, vertexBuffer3DMem + 1, vertexBuffer3DMemMapped + 1, 30000 * sizeof(Vertex4));
    CO_addBuffer(true, vertexBuffer3D[1], vertexBuffer3DMem[1], vertices3D);// CO
    createIndexBuffer(&physicalDevice, &device, indexBuffer3D, indexBuffer3DMem, indexBuffer3DMemMapped, indices3D, 45000, sizeof(Uint32), true);
    CO_addBuffer(true, indexBuffer3D[0], indexBuffer3DMem[0], NULL);// CO
    createIndexBuffer(&physicalDevice, &device, indexBuffer3D + 1, indexBuffer3DMem + 1, indexBuffer3DMemMapped + 1, indices3D, 45000, sizeof(Uint32), true);
    CO_addBuffer(true, indexBuffer3D[1], indexBuffer3DMem[1], indices3D);// CO

    createUniformBufferByBuffering(&graphicUniformBuffers, &graphicUniformBuffersMemory, &graphicUniformBufferMapped, sizeof(UniformBufferObject));
    CO_addBuffer(true, graphicUniformBuffers[0], graphicUniformBuffersMemory[0], NULL);// CO
    CO_addBuffer(true, graphicUniformBuffers[1], graphicUniformBuffersMemory[1], NULL);// CO
    createUniformBufferByBuffering(&graphic3DUniformBuffers, &graphic3DUniformBuffersMemory, &graphic3DUniformBufferMapped, sizeof(UniformBufferObject));
    CO_addBuffer(true, graphic3DUniformBuffers[0], graphic3DUniformBuffersMemory[0], NULL);// CO
    CO_addBuffer(true, graphic3DUniformBuffers[1], graphic3DUniformBuffersMemory[1], NULL);// CO
    createUniformBufferByBuffering(&UIUniformBuffers, &UIUniformBuffersMemory, &UIUniformBufferMapped, sizeof(UniformBufferObject));
    CO_addBuffer(true, UIUniformBuffers[0], UIUniformBuffersMemory[0], NULL);// CO
    CO_addBuffer(true, UIUniformBuffers[1], UIUniformBuffersMemory[1], NULL);// CO

    createUniformBufferByBuffering(&computeUniformBuffers, &computeUniformBuffersmemory, &computeUniformBufferMapped, sizeof(ComputeUniformBufferObject));
    CO_addBuffer(true, computeUniformBuffers[0], computeUniformBuffersmemory[0], NULL);// CO
    CO_addBuffer(true, computeUniformBuffers[1], computeUniformBuffersmemory[1], NULL);// CO

    createUniformBufferByBuffering(&SSGIUniformBuffers, &SSGIUniformBufferMem, &SSGIUniformBufferMapped, sizeof(SSGIUniformBufferObject));
    CO_addBuffer(true, SSGIUniformBuffers[0], SSGIUniformBufferMem[0], NULL);// CO
    CO_addBuffer(true, SSGIUniformBuffers[1], SSGIUniformBufferMem[1], NULL);// CO

    createUniformBufferByBuffering(&SunUniformBuffer, &SunUniformBufferMem, &SunUniformBufferMapped, sizeof(DirectionLight));
    CO_addBuffer(true, SunUniformBuffer[0], SunUniformBufferMem[0], NULL);// CO
    CO_addBuffer(true, SunUniformBuffer[1], SunUniformBufferMem[1], NULL);// CO
    createUniformBufferByBuffering(&lightSpaceUniformBuffer, &lightSpaceUniformBufferMem, &lightSpaceUniformBufferMapped, sizeof(LightSpace));
    CO_addBuffer(true, lightSpaceUniformBuffer[0], lightSpaceUniformBufferMem[0], NULL);// CO
    CO_addBuffer(true, lightSpaceUniformBuffer[1], lightSpaceUniformBufferMem[1], NULL);// CO

    createShaderStorageBuffers(&physicalDevice, &device, &shaderStorageBuffers, &shaderStorageBuffersMem);
    CO_addBuffer(false, shaderStorageBuffers[0], shaderStorageBuffersMem[0], NULL);// CO
    CO_addBuffer(false, shaderStorageBuffers[1], shaderStorageBuffersMem[1], NULL);// CO

    /*unfixed code*/

    VkDescriptorPoolSize graphicDescriptorPoolSize[2];
    graphicDescriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    graphicDescriptorPoolSize[0].descriptorCount = 16;
    graphicDescriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    graphicDescriptorPoolSize[1].descriptorCount = 26;
    createDescriptorPool(&device, 2, graphicDescriptorPoolSize, 22, &graphicDescriptorPool);
    CO_addDescriptorPool(graphicDescriptorPool);// CO

    //graphic shader
    PathType graphicTypes[] = {TriangleVertShader, TriangleFragShader};
    VkShaderModule * graphicTempModule = NULL;
    Uint32 graphicSetCount = CreateShaderModulesAndDescriptorSets(graphicTypes, 2, &graphicTempModule, &graphciShaderStageCreateInfo, &graphicDescriptorSetLayout, &graphicPipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, graphicDescriptorSetLayout, graphicSetCount, 5, &graphicDescriptorSets);
    createGraphicsPipeline(extent2D, 2, graphciShaderStageCreateInfo, graphicPipelineLayout, renderPass, &graphicPipeline);

    //3d model shader
    PathType modelTypes[] = {Model3dVertShader, Model3dFragShader};
    VkShaderModule * modelTempModule = NULL;
    Uint32 modelSetCount = CreateShaderModulesAndDescriptorSets(modelTypes, 2, &modelTempModule, &modelShaderStageCreateInfo, &modelDescriptorSetLayout, &modelPipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, modelDescriptorSetLayout, modelSetCount, 2, &modelDescriptorSets);
    createModelPipeline(extent2D, 2, modelShaderStageCreateInfo, modelPipelineLayout, modelRenderPass, &modelPipeline);

    PathType particleTypes[] = {ParticleVertShader, ParticleFragShader};
    VkShaderModule * particleTempModule = NULL;
    Uint32 particleSetCount = CreateShaderModulesAndDescriptorSets(particleTypes, 2, &particleTempModule, &particleShaderStageCreateInfo, &particleDescriptorSetLayout, &particlePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, particleDescriptorSetLayout, particleSetCount, 1, &particleDescriptorSets);
    createParticlePipeline(extent2D, 2, particleShaderStageCreateInfo, particlePipelineLayout, renderPass, &particlePipeline);

    // combine Shader
    PathType combineTypes[] = {CombineVertShader, CombineFragShader};
    VkShaderModule * combineTempModule = NULL;
    Uint32 combineSetCount = CreateShaderModulesAndDescriptorSets(combineTypes, 2, &combineTempModule, &combineShaderStageCreateInfo, &combineDescriptorSetLayout, &combinePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, combineDescriptorSetLayout, combineSetCount, 1, &combineDescriptorSets);
    createCombinePipeline(extent2D, 2, combineShaderStageCreateInfo, combinePipelineLayout, combineRenderPass, &combinePipeline);

    //shadow shader
    PathType shadowTypes[] = {ShadowVertShader, EmptyFragShader};
    VkShaderModule * shadowTempModule = NULL;
    Uint32 shadowSetCount = CreateShaderModulesAndDescriptorSets(shadowTypes, 2, &shadowTempModule, &shadowShaderStageCreateInfo, &shadowDescriptorSetLayout, &shadwoPipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, shadowDescriptorSetLayout, shadowSetCount, 1, &shadowDescriptorSets);
    createShadowPipeline((VkExtent2D){SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT}, 2, shadowShaderStageCreateInfo, shadwoPipelineLayout, shadowRenderPass, &shadowPipeline);

    executeCreateGraphicsPipelines(NULL);

    //compute descriptor pool 
    VkDescriptorPoolSize computeDescriptorPoolSize[4];
    computeDescriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    computeDescriptorPoolSize[0].descriptorCount = 4;
    computeDescriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeDescriptorPoolSize[1].descriptorCount = 4;
    computeDescriptorPoolSize[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    computeDescriptorPoolSize[2].descriptorCount = 10;
    computeDescriptorPoolSize[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    computeDescriptorPoolSize[3].descriptorCount = 2;
    createDescriptorPool(&device, 4, computeDescriptorPoolSize, 8, &computeDescriptorPool);
    CO_addDescriptorPool(computeDescriptorPool);// CO

    //compute shader
    PathType computeTypes[] = {ParticleCompShader};
    VkShaderModule * computeTempModule = NULL;
    Uint32 computeSetCount = CreateShaderModulesAndDescriptorSets(computeTypes, 1, &computeTempModule, &computeShaderStageCreateInfo, &computeDescriptorSetLayout, &computePipelineLayout);
    createDescriptorSets(&computeDescriptorPool, computeDescriptorSetLayout, computeSetCount, 1, &computeDescriptorSets);
    addComputePipeline(computeShaderStageCreateInfo, &computePipelineLayout, NULL, 0, &computePipeline);

    // SSGI shader
    PathType SSGITypes[] = {SSGICompShader};
    VkShaderModule * SSGITempModule = NULL;
    Uint32 SSGISetCount = CreateShaderModulesAndDescriptorSets(SSGITypes, 1, &SSGITempModule, &SSGIShaderStageCreateInfo, &SSGIDescriptorSetLayout, &SSGIPipelineLayout);
    createDescriptorSets(&computeDescriptorPool, SSGIDescriptorSetLayout, SSGISetCount, 1, &SSGIDescriptorSets);
    addComputePipeline(SSGIShaderStageCreateInfo, &SSGIPipelineLayout, NULL, 0, &SSGIPipeline);

    executeCreateComputePipelines(NULL);

    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &graphicCommandPool, &graphicCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &presentCommandPool, &presentCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &computeCommandPool, &computeCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &transferCommandPool, &transferCommandBuffer);

    createTimelineSemaphoreByBuffering(&timelineSemaphore1);
    CO_addSemaphore(timelineSemaphore1[0]);// CO
    CO_addSemaphore(timelineSemaphore1[1]);// CO

    createSemaphoreByBuffering(&imageAvailableSemaphore);
    CO_addSemaphore(imageAvailableSemaphore[0]);// CO
    CO_addSemaphore(imageAvailableSemaphore[1]);// CO
    createSemaphoreByBuffering(&renderFinishedSemaphore);
    CO_addSemaphore(renderFinishedSemaphore[0]);// CO
    CO_addSemaphore(renderFinishedSemaphore[1]);// CO
 
    createFenceByBuffering(&graphicInFlightFence);
    CO_addFence(graphicInFlightFence[0]);// CO
    CO_addFence(graphicInFlightFence[1]);// CO

    createFenceByBuffering(&computeInFlightFences);
    CO_addFence(computeInFlightFences[0]);// CO
    CO_addFence(computeInFlightFences[1]);// CO
    
    loadTileSet(TileSet1Png, TileSet1Tsd, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_TILE_SET, graphicDescriptorSets + 6);
    loadTileMap(TileMap1TsdI, -400, -500, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -1200, -1100, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -1200, -300, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -1200, 500, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -400, 500, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, 400, 500, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, 400, -300, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, 400, -1100, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -400, -1100, TEXTURE_TILE_SET);
    createStaticModelPool(&staticModelPool, 20);
    loadStaticModel(&staticModelPool, 10, BoxObj, BoxPng, vertices3D, &vertices3DCount, indices3D, &indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_MODEL, modelDescriptorSets, false);
    loadStaticModel(&staticModelPool, 10, BottomObj, BottomPng, vertices3D, &vertices3DCount, indices3D, &indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_BOTTOM, modelDescriptorSets + 2, true);

    loadImageResource(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_GENERAL, TEXTURE_SSGI_STORAGE_IMAGE, SSGIDescriptorSets + 2);

    loadTexture(Loading1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_LOADING, graphicDescriptorSets);
    loadTexture(CirclePng, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_CIRCLE, graphicDescriptorSets + 2);
    loadTexture(MainFontPng, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_FONT, graphicDescriptorSets + 4);
    // loadTexture(Box1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_BOX, graphicDescriptorSets + 8);

    addDescriptorSetToTexture(TEXTURE_MODEL_DEPTH, SSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_NORMAL, SSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, SSGIDescriptorSets + 0);

    G_Texture_P * loadingTexture = getTexture(TEXTURE_LOADING);
    G_Texture_P * circleTexture = getTexture(TEXTURE_CIRCLE);
    G_Texture_P * fontTexture = getTexture(TEXTURE_FONT);
    G_Texture_P * tileSetTexture = getTexture(TEXTURE_TILE_SET);
    G_Texture_P * modelTexture = getTexture(TEXTURE_MODEL);
    G_Texture_P * bottomTexture = getTexture(TEXTURE_BOTTOM);
    G_Texture_P * normalTexture = getTexture(TEXTURE_NORMAL);
    G_Texture_P * shadowTexture = getTexture(TEXTURE_SHADOW);
    // G_Texture_P * directColorTexture = getTexture(TEXTURE_MODEL_COLOR);
    // G_Texture_P * SSGIStorageTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);

    // UI
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, loadingTexture->pDescriptorSet, UIUniformBuffers, 0, sizeof(UniformBufferObject));//0
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, fontTexture->pDescriptorSet, UIUniformBuffers, 0, sizeof(UniformBufferObject));

    // graphic
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, circleTexture->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, tileSetTexture->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    // addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, getTexture(TEXTURE_BOX)->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));

    // model
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, modelTexture->pDescriptorSet, graphic3DUniformBuffers, 0, sizeof(UniformBufferObject));//4
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, bottomTexture->pDescriptorSet, graphic3DUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, modelTexture->pDescriptorSet, SunUniformBuffer, 0, sizeof(DirectionLight));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, bottomTexture->pDescriptorSet, SunUniformBuffer, 0, sizeof(DirectionLight));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, shadowDescriptorSets, lightSpaceUniformBuffer, 0, sizeof(LightSpace));

    // SSGI
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, normalTexture->pDescriptorSet, SSGIUniformBuffers, 0, sizeof(SSGIUniformBufferObject));

    // graphics
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_LOADING, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_CIRCLE, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//8
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_FONT, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_TILE_SET, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // addDescriptorSetToTexture(TEXTURE_2D_COLOR, combine2DDescriptorSets);
    // addDescriptorSetToTexture(TEXTURE_SHADOW_MAP, combine2DDescriptorSets);
    // addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_2D_COLOR, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_SHADOW_MAP, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // model
    addShadowDescriptorSetToTexture(TEXTURE_MODEL, shadowTexture->pDescriptorSet);
    addShadowDescriptorSetToTexture(TEXTURE_BOTTOM, shadowTexture->pDescriptorSet);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_MODEL, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_BOTTOM, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//12
    addDescriptorSetToTexture(TEXTURE_SHADOW, modelTexture->pDescriptorSet);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_SHADOW, shadowSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorSetToTexture(TEXTURE_SHADOW, bottomTexture->pDescriptorSet);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_SHADOW, shadowSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // SSGI
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_DEPTH, depthSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_NORMAL, normalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_MODEL_COLOR, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, TEXTURE_SSGI_STORAGE_IMAGE, NULL, VK_IMAGE_LAYOUT_GENERAL);

    // graphic
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, particleDescriptorSets, graphicUniformBuffers, 0, sizeof(UniformBufferObject));//16

    // compute
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, computeDescriptorSets, computeUniformBuffers, 0, sizeof(ComputeUniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, computeDescriptorSets, shaderStorageBuffers, 0, sizeof(Particle) * PARTICLE_COUNT);
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, computeDescriptorSets, shaderStorageBuffers, 0, sizeof(Particle) * PARTICLE_COUNT);

    executeUpdateDescriptorSets();

    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, combineDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_SSGI_STORAGE_IMAGE, combineDescriptorSets + 0);
    // combine
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_COLOR, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_SSGI_STORAGE_IMAGE, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16

    executeUpdateDescriptorSets();

    // initializeRecreate();

    resultVulkan(VK_SUCCESS, 0);
}

void cleanVulkan(void)
{
    print("\nclean begin");

    vkDeviceWaitIdle(device);

    destroyStaticModelPool(&staticModelPool);
    unloadAllTexture();
    CO_CleanAllVkResource();
}
    /*vertices2D[4] = (Vertex){{-0.0f, -0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices2D[5] = (Vertex){{1.0f, -0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices2D[6] = (Vertex){{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices2D[7] = (Vertex){{-0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};*/