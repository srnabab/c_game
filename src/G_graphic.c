#include "G_constants.h"
#include "G_log.h"
#include "G_graphic.h"
#include "G_texture.h"
#include "G_TileMap/G_TileSet.h"
#include "G_staticModel.h"
#include "G_map.h"
#include "G_image.h"
#include "G_allocator.h"
#include "G_buffer.h"
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
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD)) 
    {
        //todo
        //add result juadge for sdl

        //show a messagebox for error informations
        int temp = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "", "Error initializing SDL.\n", NULL);
        if (!temp)
        {
            print("Error show messagebox\n");
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
// Uint32 width = 800;
// Uint32 height = 600;
Uint32 width = 1920;
Uint32 height = 1080;

float physicalCoffectX = 1.0f;
float physicalCoffectY = 1.0f;

bool initWindow_3D(void)
{
   //create sdl window_3D and sign window_3D as a vulkan window_3D
    if (!initSDL()) return false;
    window_3D = SDL_CreateWindow("Vulkan_3D", width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (window_3D == NULL) return false;

    CO_addWindow(window_3D);
    
    int count = 0;
    SDL_DisplayID * displays = SDL_GetDisplays(&count);
    for (int i = 0;i < count;i++)
    {
        print("display id:%u, name:%s", displays[i], SDL_GetDisplayName(displays[i]));
    }
    displayId = displays[0];
    SDL_DisplayMode ** modes = NULL;
    modes = SDL_GetFullscreenDisplayModes(displayId, &count);
    for (int i = 0;i < count;i++)
    {
        print("id %u format: %u: %u x %u, %lfHz", modes[i]->displayID, modes[i]->format, modes[i]->w, modes[i]->h, modes[i]->refresh_rate);
    }

    physicalCoffectX = (float)width / LOGICAL_WIDTH;
    physicalCoffectY = (float)height / LOGICAL_HEIGHT;

    print("window_3D initialized");

    Uint32 iconWidth, iconHeight;
    iconWidth = iconHeight = 0;         
    uint8_t iconChannel;
    void * iconPixels = readPNG(IconPng, &iconWidth, &iconHeight, &iconChannel);
    if (iconPixels == NULL)
    {
        return false;
    }
    SDL_Surface * iconSurface = SDL_CreateSurfaceFrom(iconWidth, iconHeight, SDL_PIXELFORMAT_RGBA32, iconPixels, iconWidth * iconChannel);
    if (iconSurface == NULL)
    {
        G_free(iconPixels);
        return false;
    }

    if (!SDL_SetWindowIcon(window_3D, iconSurface))
        return false;

    SDL_DestroySurface(iconSurface);
    G_free(iconPixels);

    return true;
}

static VkAllocationCallbacks SDL_allocationCallBacks = {};

static VkFormat swapchainFormat = 0;

static VkPipelineShaderStageCreateInfo * graphciShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * modelShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * shadowShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * particleShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * shapeShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * computeShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * SSGIShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * combineShaderStageCreateInfo = NULL;

static VkDescriptorSetLayout * graphicDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * modelDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * shadowDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * particleDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * shapeDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * computeDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * SSGIDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * combineDescriptorSetLayout = NULL;

static UniformBufferObject ubo = {};

static UniformBufferObject ubo3D = {};

static UniformBufferObject uboUI = {};

static ComputeUniformBufferObject computeUbo = {};

static SSGIUniformBufferObject SSGIubo = {};

static DirectionLight  Sunubo = {};

static LightSpace lightSpaceubo = {};


static VkDescriptorPool graphicDescriptorPool = NULL;

static VkDescriptorPool computeDescriptorPool = NULL;

static float camera_X = 0.0f;
static float camera_Y = 0.0f;

static float pictureX = 0;
static float pictureY = 0;

// static VkDeviceMemory shaderStorageBuffersMem[MAX_FRAMES_IN_FLIGHT];

static PushConstants picturePushConstants = {0.0f};
static ShapeConstants shapePushConstants = {(vec2){0.0f, 0.0f}, (vec2){0.053333333f, 0.053333333f}};

static G_StaticModelPool staticModelPool = {};

// static G_Thread_Pool threadPool = {};

//store all compoents for initialize vulkan in a struct
VK_ALL allInOne = {};

static void initializeAllInOne(void)
{
    allInOne.pAllocationCallbacks = &SDL_allocationCallBacks;

    allInOne.pStaticModelPool = &staticModelPool;
    allInOne.pGraphicUbo = &ubo;
    allInOne.pGraphic3DUbo = &ubo3D;
    allInOne.pUIUbo = &uboUI;
    allInOne.pSSGIubo = &SSGIubo;
    allInOne.pLightSpaceUbo = &lightSpaceubo;
    allInOne.pSunubo = &Sunubo;

    allInOne.pComputeUbo = &computeUbo;

    allInOne.pCamera_X = &camera_X;
    allInOne.pCamera_Y = &camera_Y;

    allInOne.pPictureX = &pictureX;
    allInOne.pPictureY = &pictureY;

    allInOne.pPushConstants = &picturePushConstants;
    allInOne.pShapeConstants = &shapePushConstants;

    // initStack(&allInOne.bottomImageDrawStack, sizeof(DrawHere), NULL, NULL);
    // initStack(&allInOne.bottomImageMoveStack, sizeof(FromTo), NULL, NULL);

    // createThreadPool(&threadPool, 1, false);
    // allInOne.pThreadPool = &threadPool;
    // allInOne.timelineSemaphoreSignalValue = 0;
}

void initVulkan(void)
{
    /*fixed compoent*/
    print("initializing...");

    initGlobalTexture();

    initializeAllInOne();

    initCollection();

    SDL_allocationCallBacks.pUserData = NULL;
    SDL_allocationCallBacks.pfnAllocation = VK_alloc;
    SDL_allocationCallBacks.pfnReallocation = VK_realloc;
    SDL_allocationCallBacks.pfnFree = VK_free;
    SDL_allocationCallBacks.pfnInternalAllocation = NULL;
    SDL_allocationCallBacks.pfnInternalFree = NULL;    

    vulkanVersion();

    createInstance();
    CO_addInstance(allInOne.instance);// CO

    createSurface(window_3D, &allInOne.surface3D);
    CO_addSurface(allInOne.surface3D);// Co

    pickPhysicalDevice();

    findQueueFamilies();
    createLogicalDevice();
    CO_addDevice(allInOne.device);// CO

    createQueue();

    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, &allInOne.graphicCommandPool);
    CO_addCommandPool(allInOne.graphicCommandPool);// CO
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, allInOne.queueFamilyIndices.presentFamily.familyIndice, &allInOne.presentCommandPool);
    CO_addCommandPool(allInOne.presentCommandPool);// CO
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, allInOne.queueFamilyIndices.computeFamily.familyIndice, &allInOne.computeCommandPool);
    CO_addCommandPool(allInOne.computeCommandPool);// CO
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, allInOne.queueFamilyIndices.transferFamily.familyIndice, &allInOne.transferCommandPool);
    CO_addCommandPool(allInOne.transferCommandPool);// CO

    createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.graphicCommandPool, allInOne.pGraphicCommandBuffer, 2);
    createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_SECONDARY, allInOne.graphicCommandPool, allInOne.pGraphicCopyCommandBuffer, 2);
    createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.presentCommandPool, allInOne.pPresentCommandBuffer, 2);
    createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.computeCommandPool, allInOne.pComputeCommandBuffer, 2);
    createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_SECONDARY, allInOne.computeCommandPool, allInOne.pComputeCopyCommandBuffer, 2);
    createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.transferCommandPool, allInOne.pTransferCommandBuffer, 2);
    createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_SECONDARY, allInOne.transferCommandPool, allInOne.pTransferCopyCommandBuffer, 2);

    getSurfaceFormats(allInOne.surface3D, &allInOne.surface3DFormat);
    getPresentModes(&allInOne.presentMode3D);
    getSurfaceCapabilities(allInOne.surface3D, &allInOne.surface3DCapabilities);
    
    allInOne.extent2D.width = width;
    allInOne.extent2D.height = height;

    createSwapchain(allInOne.surface3D, allInOne.surface3DCapabilities, allInOne.surface3DFormat, allInOne.presentMode3D, &allInOne.swapchain3D, NULL);
    CO_addSwapchain(allInOne.swapchain3D);// CO

    getSwapchainNumber(allInOne.swapchain3D, &allInOne.imageCount3D);
    createSwapchainImage(allInOne.swapchain3D, &allInOne.imageCount3D, &allInOne.pSwapchain3DImages);
    CO_addSwapchainImage(allInOne.pSwapchain3DImages);// CO

    swapchainFormat = allInOne.surface3DFormat.format;

    //swapchain3D image view
    createSwapchainImageView(allInOne.pSwapchain3DImages, allInOne.imageCount3D, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, &allInOne.pSwapchain3DImageViews);
    CO_addSwapchainImageView(allInOne.imageCount3D, allInOne.pSwapchain3DImageViews);// CO

    loadShadowResource(TEXTURE_SHADOW, SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT);
    G_Texture_P * modelShadowTexture = getTexture(TEXTURE_SHADOW);
    createShadowRenderPass(modelShadowTexture->format, &allInOne.shadowRenderPass);
    CO_addRenderPass(allInOne.shadowRenderPass);// CO
    createFrameBuffer(2, SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT, 1, &modelShadowTexture->imageView, NULL, allInOne.shadowRenderPass, &allInOne.pShadowFramebuffer);
    CO_addFrameBuffer(2, allInOne.pShadowFramebuffer);// CO

    loadDepthResource(TEXTURE_MODEL_DEPTH, true);
    loadDepthResource(TEXTURE_2D_DEPTH, true);
    loadImageResource(VK_FORMAT_R16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_SHADOW_MAP, NULL);
    loadNormalResource(TEXTURE_NORMAL);
    loadImageResource(swapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_MODEL_COLOR, NULL);
    loadImageResource(swapchainFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_2D_COLOR, NULL);

    G_Texture_P * modelDepthTexutre = getTexture(TEXTURE_MODEL_DEPTH);
    G_Texture_P * modelNormalTexture = getTexture(TEXTURE_NORMAL);
    G_Texture_P * modelColorTexture = getTexture(TEXTURE_MODEL_COLOR);
    G_Texture_P * seprateShadowTexture = getTexture(TEXTURE_SHADOW_MAP);
    G_Texture_P * color2dTexture = getTexture(TEXTURE_2D_COLOR);
    createModelRenderPass(modelColorTexture->format, modelNormalTexture->format, seprateShadowTexture->format, modelDepthTexutre->format, &allInOne.modelRenderPass);
    CO_addRenderPass(allInOne.modelRenderPass);// CO

    VkImageView modelImageViews[] = {modelColorTexture->imageView, modelNormalTexture->imageView, seprateShadowTexture->imageView, modelDepthTexutre->imageView};
    createFrameBuffer(2, allInOne.extent2D.width, allInOne.extent2D.height, 4, modelImageViews, NULL, allInOne.modelRenderPass, &allInOne.pDirectColorFramebuffer);
    CO_addFrameBuffer(2, allInOne.pDirectColorFramebuffer);// CO

    createOffscreenRenderPass(VK_FORMAT_R8G8B8A8_SRGB, &allInOne.offscreenRenderPass);
    CO_addRenderPass(allInOne.offscreenRenderPass);// CO

    createCombineRenderPass(swapchainFormat, &allInOne.combineRenderPass);
    CO_addRenderPass(allInOne.combineRenderPass);// CO
    createFrameBuffer(allInOne.imageCount3D, allInOne.extent2D.width, allInOne.extent2D.height, 1, NULL, allInOne.pSwapchain3DImageViews, allInOne.combineRenderPass, &allInOne.pCombineFramebuffer);
    CO_addFrameBuffer(allInOne.imageCount3D, allInOne.pCombineFramebuffer);// CO

    G_Texture_P * color2dDepthTexture = getTexture(TEXTURE_2D_DEPTH);
    createGraphicRenderPass(swapchainFormat, color2dDepthTexture->format, &allInOne.renderPass);
    CO_addRenderPass(allInOne.renderPass);// CO
    VkImageView color2dImageViews[] = {color2dTexture->imageView, color2dDepthTexture->imageView};
    createFrameBuffer(2, allInOne.extent2D.width, allInOne.extent2D.height, 1, color2dImageViews, NULL, allInOne.renderPass, &allInOne.pGraphic2dFramebuffer);
    CO_addFrameBuffer(2, allInOne.pGraphic2dFramebuffer);// CO
    
    VkImageView bottomImageViews[] = {modelColorTexture->imageView};
    createFrameBuffer(2, allInOne.extent2D.width, allInOne.extent2D.height, 1, bottomImageViews, NULL, allInOne.renderPass, &allInOne.pBottomFramebuffer);
    CO_addFrameBuffer(2, allInOne.pBottomFramebuffer);// CO

    createTextureSampler(&allInOne.textureSampler);
    CO_addSampler(allInOne.textureSampler);// CO
    createNormalSampler(&allInOne.normalSampler);
    CO_addSampler(allInOne.normalSampler);// CO
    createDepthSampler(&allInOne.depthSampler);
    CO_addSampler(allInOne.depthSampler);// CO
    createShadowSampler(&allInOne.shadowSampler);
    CO_addSampler(allInOne.shadowSampler);// CO

    allInOne.stagingBufferPool = createBufferPool(200 * 1024 * 1024, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
    allInOne.vertexStagingBufferPool = createBufferPool(64, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
    allInOne.vertexBufferPool = createBufferPool(5 * sizeof(Vertex23_) + (VERTEX_COUNT_IN_BUFFER_2D + MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D) * sizeof(Vertex332_) * 2 + 30000 * sizeof(Vertex3323) * 2 + 60 * sizeof(mat4) * 2\
    , VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);
    allInOne.indexStagingBufferPool = createBufferPool(64, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
    allInOne.indexBufferPool = createBufferPool(INDEX_COUNT_IN_BUFFER_2D * sizeof(Uint16) + 45000 * sizeof(Uint32) * 2, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);
    allInOne.uniformStagingBufferPool = createBufferPool(sizeof(UniformBufferObject) * 2 * 3 + sizeof(ComputeUniformBufferObject) * 2 + sizeof(SSGIUniformBufferObject) * 2 + sizeof(DirectionLight) * 2 + sizeof(LightSpace) * 2\
    , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
    allInOne.storageBufferPool = createBufferPool(PARTICLE_COUNT * sizeof(Particle) * 2, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);

    Vertex23_ shapeVertex[5] =
    {
        {{-0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}, // 0: 左下
        {{ 0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}, // 1: 右下
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}}, // 2: 右上
        {{-0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}}, // 3: 左上
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}  // 4: 回到左下 (闭合)
    };
    allInOne.tempBuffer = allocateBuffer(5 * sizeof(Vertex23_), &allInOne.vertexBufferPool);
    initBufferData(allInOne.tempBuffer, shapeVertex, 5 * sizeof(Vertex23_));

    allInOne.pVertices2D = (Vertex332_*)G_calloc(VERTEX_COUNT_IN_BUFFER_2D + MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D, sizeof(Vertex332_));
    allInOne.maxVertices2DCount = VERTEX_COUNT_IN_BUFFER_2D + MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D;
    allInOne.vertexBuffer2D[0] = allocateBuffer((VERTEX_COUNT_IN_BUFFER_2D + MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D) * sizeof(Vertex332_), &allInOne.vertexBufferPool);
    allInOne.vertexBuffer2D[1] = allocateBuffer((VERTEX_COUNT_IN_BUFFER_2D + MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D) * sizeof(Vertex332_), &allInOne.vertexBufferPool);

    Uint16 * indices2D = (Uint16 *)G_calloc(INDEX_COUNT_IN_BUFFER_2D, sizeof(Uint16));
    indexInitialize(indices2D, MAX_UNIT_COUNT_2D);

    allInOne.indexBuffer2D = allocateBuffer(INDEX_COUNT_IN_BUFFER_2D * sizeof(Uint16), &allInOne.indexBufferPool);
    initBufferData(allInOne.indexBuffer2D, indices2D, INDEX_COUNT_IN_BUFFER_2D * sizeof(Uint16));

    G_free(indices2D);

    allInOne.pVertices3D = (Vertex3323*)G_calloc(30000, sizeof(Vertex3323));

    allInOne.maxVertices3DCount = 30000;
    allInOne.vertexBuffer3D[0] = allocateBuffer(30000 * sizeof(Vertex3323), &allInOne.vertexBufferPool);
    allInOne.vertexBuffer3D[1] = allocateBuffer(30000 * sizeof(Vertex3323), &allInOne.vertexBufferPool);

    allInOne.pIndices3D = (Uint32*)G_calloc(45000, sizeof(Uint32));
    allInOne.indexBuffer3D[0] = allocateBuffer(45000 * sizeof(Uint32), &allInOne.indexBufferPool);
    allInOne.indexBuffer3D[1] = allocateBuffer(45000 * sizeof(Uint32), &allInOne.indexBufferPool);

    allInOne.pGraphicUniformBuffer[0] = allocateBuffer(sizeof(UniformBufferObject), &allInOne.uniformStagingBufferPool);
    allInOne.pGraphicUniformBuffer[1] = allocateBuffer(sizeof(UniformBufferObject), &allInOne.uniformStagingBufferPool);

    allInOne.pGraphic3DUniformBuffer[0] = allocateBuffer(sizeof(UniformBufferObject), &allInOne.uniformStagingBufferPool);
    allInOne.pGraphic3DUniformBuffer[1] = allocateBuffer(sizeof(UniformBufferObject), &allInOne.uniformStagingBufferPool);

    allInOne.pUIUniformBuffer[0] = allocateBuffer(sizeof(UniformBufferObject), &allInOne.uniformStagingBufferPool);
    allInOne.pUIUniformBuffer[1] = allocateBuffer(sizeof(UniformBufferObject), &allInOne.uniformStagingBufferPool);

    allInOne.pComputeUniformBuffer[0] = allocateBuffer(sizeof(ComputeUniformBufferObject), &allInOne.uniformStagingBufferPool);
    allInOne.pComputeUniformBuffer[1] = allocateBuffer(sizeof(ComputeUniformBufferObject), &allInOne.uniformStagingBufferPool);

    allInOne.pSSGIUniformBuffer[0] = allocateBuffer(sizeof(SSGIUniformBufferObject), &allInOne.uniformStagingBufferPool);
    allInOne.pSSGIUniformBuffer[1] = allocateBuffer(sizeof(SSGIUniformBufferObject), &allInOne.uniformStagingBufferPool);

    allInOne.pSunUniformBuffer[0] = allocateBuffer(sizeof(DirectionLight), &allInOne.uniformStagingBufferPool);
    allInOne.pSunUniformBuffer[1] = allocateBuffer(sizeof(DirectionLight), &allInOne.uniformStagingBufferPool);

    allInOne.pLightSpaceUniformBuffer[0] = allocateBuffer(sizeof(LightSpace), &allInOne.uniformStagingBufferPool);
    allInOne.pLightSpaceUniformBuffer[1] = allocateBuffer(sizeof(LightSpace), &allInOne.uniformStagingBufferPool);

    createShaderStorageBuffers(allInOne.pShaderStorageBuffer, &allInOne.storageBufferPool);

    /*unfixed code*/

    VkDescriptorPoolSize graphicDescriptorPoolSize[2];
    graphicDescriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    graphicDescriptorPoolSize[0].descriptorCount = 24;
    graphicDescriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    graphicDescriptorPoolSize[1].descriptorCount = 26;
    createDescriptorPool(&allInOne.device, 2, graphicDescriptorPoolSize, 26, &graphicDescriptorPool);
    CO_addDescriptorPool(graphicDescriptorPool);// CO

    //graphic shader
    PathType graphicTypes[] = {TriangleVertShader, TriangleFragShader};
    VkShaderModule * graphicTempModule = NULL;
    Uint32 graphicSetCount = CreateShaderModulesAndDescriptorSets(graphicTypes, 2, &graphicTempModule, &graphciShaderStageCreateInfo, &graphicDescriptorSetLayout, &allInOne.graphicPipelineLayout);
    G_createDescriptorSets(&graphicDescriptorPool, graphicDescriptorSetLayout, graphicSetCount, 5, image2dDescriptorSetUpdate, &allInOne.graphicDescriptorSets);
    createGraphicsPipeline(allInOne.extent2D, 2, graphciShaderStageCreateInfo, allInOne.graphicPipelineLayout, allInOne.renderPass, &allInOne.graphicPipeline);

    //3d model shader
    PathType modelTypes[] = {Model3dVertShader, Model3dFragShader};
    VkShaderModule * modelTempModule = NULL;
    Uint32 modelSetCount = CreateShaderModulesAndDescriptorSets(modelTypes, 2, &modelTempModule, &modelShaderStageCreateInfo, &modelDescriptorSetLayout, &allInOne.modelPipelineLayout);
    G_createDescriptorSets(&graphicDescriptorPool, modelDescriptorSetLayout, modelSetCount, 3, image3dDescriptorSetUpdate, &allInOne.modelDescriptorSets);
    createModelPipeline(allInOne.extent2D, 2, modelShaderStageCreateInfo, allInOne.modelPipelineLayout, allInOne.modelRenderPass, &allInOne.modelPipeline);

    // particle shader
    PathType particleTypes[] = {ParticleVertShader, ParticleFragShader};
    VkShaderModule * particleTempModule = NULL;
    Uint32 particleSetCount = CreateShaderModulesAndDescriptorSets(particleTypes, 2, &particleTempModule, &particleShaderStageCreateInfo, &particleDescriptorSetLayout, &allInOne.particlePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, particleDescriptorSetLayout, particleSetCount, 1, &allInOne.pParticleDescriptorSets);
    CO_addDescriptorSetsMem(allInOne.pParticleDescriptorSets); // CO
    createParticlePipeline(allInOne.extent2D, 2, particleShaderStageCreateInfo, allInOne.particlePipelineLayout, allInOne.renderPass, &allInOne.particlePipeline);

    // shape shader
    PathType shapeTypes[] = {ShapeVertShader, ShapeFragShader};
    VkShaderModule * shapeTempModule = NULL;
    Uint32 shapeSetCount = CreateShaderModulesAndDescriptorSets(shapeTypes, 2, &shapeTempModule, &shapeShaderStageCreateInfo, &shapeDescriptorSetLayout, &allInOne.shapePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, shapeDescriptorSetLayout, shapeSetCount, 1, &allInOne.pShapeDescriptorSets);
    CO_addDescriptorSetsMem(allInOne.pShapeDescriptorSets); // CO
    createShapePipeline(allInOne.extent2D, 2, shapeShaderStageCreateInfo, allInOne.shapePipelineLayout, allInOne.renderPass, &allInOne.shapePipeline);

    // combine Shader
    PathType combineTypes[] = {CombineVertShader, CombineFragShader};
    VkShaderModule * combineTempModule = NULL;
    Uint32 combineSetCount = CreateShaderModulesAndDescriptorSets(combineTypes, 2, &combineTempModule, &combineShaderStageCreateInfo, &combineDescriptorSetLayout, &allInOne.combinePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, combineDescriptorSetLayout, combineSetCount, 1, &allInOne.pCombineDescriptorSets);
    CO_addDescriptorSetsMem(allInOne.pCombineDescriptorSets); // CO
    createCombinePipeline(allInOne.extent2D, 2, combineShaderStageCreateInfo, allInOne.combinePipelineLayout, allInOne.combineRenderPass, &allInOne.combinePipeline);

    //shadow shader
    PathType shadowTypes[] = {ShadowVertShader, EmptyFragShader};
    VkShaderModule * shadowTempModule = NULL;
    Uint32 shadowSetCount = CreateShaderModulesAndDescriptorSets(shadowTypes, 2, &shadowTempModule, &shadowShaderStageCreateInfo, &shadowDescriptorSetLayout, &allInOne.shadowPipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, shadowDescriptorSetLayout, shadowSetCount, 1, &allInOne.pShadowDescriptorSets);
    CO_addDescriptorSetsMem(allInOne.pShadowDescriptorSets); // CO
    createShadowPipeline((VkExtent2D){SHADOW_MAPPING_WIDTH, SHADOW_MAPPING_HEIGHT}, 2, shadowShaderStageCreateInfo, allInOne.shadowPipelineLayout, allInOne.shadowRenderPass, &allInOne.shadowPipeline);

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
    createDescriptorPool(&allInOne.device, 4, computeDescriptorPoolSize, 8, &computeDescriptorPool);
    CO_addDescriptorPool(computeDescriptorPool);// CO

    //compute shader
    PathType computeTypes[] = {ParticleCompShader};
    VkShaderModule * computeTempModule = NULL;
    Uint32 computeSetCount = CreateShaderModulesAndDescriptorSets(computeTypes, 1, &computeTempModule, &computeShaderStageCreateInfo, &computeDescriptorSetLayout, &allInOne.computePipelineLayout);
    createDescriptorSets(&computeDescriptorPool, computeDescriptorSetLayout, computeSetCount, 1, &allInOne.pComputeDescriptorSets);
    CO_addDescriptorSetsMem(allInOne.pComputeDescriptorSets); // CO
    addComputePipeline(computeShaderStageCreateInfo, allInOne.computePipelineLayout, NULL, 0, &allInOne.computePipeline);

    // SSGI shader
    PathType SSGITypes[] = {SSGICompShader};
    VkShaderModule * SSGITempModule = NULL;
    Uint32 SSGISetCount = CreateShaderModulesAndDescriptorSets(SSGITypes, 1, &SSGITempModule, &SSGIShaderStageCreateInfo, &SSGIDescriptorSetLayout, &allInOne.SSGIPipelineLayout);
    createDescriptorSets(&computeDescriptorPool, SSGIDescriptorSetLayout, SSGISetCount, 1, &allInOne.pSSGIDescriptorSets);
    CO_addDescriptorSetsMem(allInOne.pSSGIDescriptorSets); // CO
    addComputePipeline(SSGIShaderStageCreateInfo, allInOne.SSGIPipelineLayout, NULL, 0, &allInOne.SSGIPipeline);

    executeCreateComputePipelines(NULL);

    createTimelineSemaphoreByBuffering(&allInOne.pTimelineSemaphore2d);
    CO_addSemaphore(allInOne.pTimelineSemaphore2d[0]);// CO
    CO_addSemaphore(allInOne.pTimelineSemaphore2d[1]);// CO

    createTimelineSemaphoreByBuffering(&allInOne.pTimelineSemaphore3d);
    CO_addSemaphore(allInOne.pTimelineSemaphore3d[0]);// CO
    CO_addSemaphore(allInOne.pTimelineSemaphore3d[1]);// CO

    createSemaphoreByBuffering(&allInOne.pImageAvailableSemaphore);
    CO_addSemaphore(allInOne.pImageAvailableSemaphore[0]);// CO
    CO_addSemaphore(allInOne.pImageAvailableSemaphore[1]);// CO

    createSemaphoreByBuffering(&allInOne.pRenderFinishedSemaphore);
    CO_addSemaphore(allInOne.pRenderFinishedSemaphore[0]);// CO
    CO_addSemaphore(allInOne.pRenderFinishedSemaphore[1]);// CO

    createSemaphoreByBuffering(&allInOne.pComputeSemaphore);
    CO_addSemaphore(allInOne.pComputeSemaphore[0]);// CO
    CO_addSemaphore(allInOne.pComputeSemaphore[1]);// CO

    createSemaphoreByBuffering(&allInOne.pGraphicSemaphore);
    CO_addSemaphore(allInOne.pGraphicSemaphore[0]);// CO
    CO_addSemaphore(allInOne.pGraphicSemaphore[1]);// CO

    createSemaphoreByBuffering(&allInOne.pTransferSemaphore);
    CO_addSemaphore(allInOne.pTransferSemaphore[0]);// CO
    CO_addSemaphore(allInOne.pTransferSemaphore[1]);// CO

    createFenceByBuffering(&allInOne.pGraphicInFlightFence);
    CO_addFence(allInOne.pGraphicInFlightFence[0]);// CO
    CO_addFence(allInOne.pGraphicInFlightFence[1]);// CO

    createFenceByBuffering(&allInOne.pComputeInFlightFence);
    CO_addFence(allInOne.pComputeInFlightFence[0]);// CO
    CO_addFence(allInOne.pComputeInFlightFence[1]);// CO

    createFenceByBuffering(&allInOne.pTransferInFlightFence);
    CO_addFence(allInOne.pTransferInFlightFence[0]);// CO
    CO_addFence(allInOne.pTransferInFlightFence[1]);// CO
   
    loadTileSet(TileSet1Png, TileSet1Tsd, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_TILE_SET, &allInOne.graphicDescriptorSets, allInOne.pGraphicUniformBuffer, true);
    loadTileMap(TileMap1TsdI, TEXTURE_TILE_SET, MAIN_TILE_MAP);

    createStaticModelPool(&staticModelPool, &allInOne.vertexBufferPool, 60);

    void * datas[] = {allInOne.pGraphic3DUniformBuffer, allInOne.pSunUniformBuffer, getTexture(TEXTURE_SHADOW)};
    loadStaticModel(&staticModelPool, 1, BottomObj, BottomPng, allInOne.pVertices3D, &allInOne.vertices3DCount, allInOne.pIndices3D, &allInOne.indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_BOTTOM, &allInOne.modelDescriptorSets, datas, true);
    loadStaticModel(&staticModelPool, 10, BoxObj, BoxPng, allInOne.pVertices3D, &allInOne.vertices3DCount, allInOne.pIndices3D, &allInOne.indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_MODEL, &allInOne.modelDescriptorSets, datas, true);
    loadStaticModel(&staticModelPool, 1, VoxelObj, VoxelPng, allInOne.pVertices3D, &allInOne.vertices3DCount, allInOne.pIndices3D, &allInOne.indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_VOXEL, &allInOne.modelDescriptorSets, datas, true);

    loadImageResource(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_GENERAL, TEXTURE_SSGI_STORAGE_IMAGE, allInOne.pSSGIDescriptorSets + 2);

    G_loadImage(Loading1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_LOADING, &allInOne.graphicDescriptorSets, 0, allInOne.pUIUniformBuffer, true);
    G_loadImage(CirclePng, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_CIRCLE, &allInOne.graphicDescriptorSets, 0, allInOne.pGraphicUniformBuffer, true);
    G_loadImage(MainFontPng, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_FONT, &allInOne.graphicDescriptorSets, 0, allInOne.pUIUniformBuffer, true);
    // loadTexture(Box1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_BOX, graphicDescriptorSets + 8);

    addDescriptorSetToTexture(TEXTURE_MODEL_DEPTH, allInOne.pSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_NORMAL, allInOne.pSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, allInOne.pSSGIDescriptorSets + 0);

    G_Texture_P * normalTexture = getTexture(TEXTURE_NORMAL);
    // G_Texture_P * directColorTexture = getTexture(TEXTURE_MODEL_COLOR);
    G_Texture_P * SSGIStorageTexture = getTexture(TEXTURE_SSGI_STORAGE_IMAGE);

    // model
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pShadowDescriptorSets, allInOne.pLightSpaceUniformBuffer);

    // shape
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pShapeDescriptorSets, allInOne.pGraphicUniformBuffer);

    // SSGI
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, normalTexture->pDescriptorSet, allInOne.pSSGIUniformBuffer);

    // SSGI
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, modelDepthTexutre, NULL, allInOne.depthSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, normalTexture, NULL, allInOne.normalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, modelColorTexture, NULL, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, SSGIStorageTexture, NULL, NULL, VK_IMAGE_LAYOUT_GENERAL);

    // graphic
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pParticleDescriptorSets, allInOne.pGraphicUniformBuffer);//16

    // compute
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pComputeDescriptorSets, allInOne.pComputeUniformBuffer);
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, allInOne.pComputeDescriptorSets, allInOne.pShaderStorageBuffer);
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, allInOne.pComputeDescriptorSets, allInOne.pShaderStorageBuffer);

    executeUpdateDescriptorSets();

    // combine
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, modelColorTexture, allInOne.pCombineDescriptorSets, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, SSGIStorageTexture, allInOne.pCombineDescriptorSets, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, color2dTexture, allInOne.pCombineDescriptorSets, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, seprateShadowTexture, allInOne.pCombineDescriptorSets, allInOne.depthSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, TEXTURE_2D_DEPTH, allInOne.depthSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    executeUpdateDescriptorSets();

    resultVulkan(VK_SUCCESS, 0);
}

void cleanVulkan(void)
{
    print("\nclean begin");

    if (allInOne.device) vkDeviceWaitIdle(allInOne.device);

    // destroyThreadPool(allInOne.pThreadPool);

    destroyStaticModelPool(&staticModelPool);
    destroyBufferPool(&allInOne.stagingBufferPool);
    destroyBufferPool(&allInOne.vertexStagingBufferPool);
    destroyBufferPool(&allInOne.vertexBufferPool);
    destroyBufferPool(&allInOne.indexStagingBufferPool);
    destroyBufferPool(&allInOne.indexBufferPool);
    destroyBufferPool(&allInOne.uniformStagingBufferPool);
    destroyBufferPool(&allInOne.storageBufferPool);
    G_destroyDescriptorSets(&allInOne.graphicDescriptorSets);
    G_destroyDescriptorSets(&allInOne.modelDescriptorSets);
    G_free(allInOne.pVertices2D);
    G_free(allInOne.pVertices3D);
    G_free(allInOne.pIndices3D);
    unloadAllTexture();
    CO_CleanAllVkResource();
    // destroyThreadPool(allInOne.pThreadPool);
}
    /*vertices2D[4] = (Vertex332_){{-0.0f, -0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices2D[5] = (Vertex332_){{1.0f, -0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices2D[6] = (Vertex332_){{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices2D[7] = (Vertex332_){{-0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};*/