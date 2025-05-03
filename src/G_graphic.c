#include "G_constants.h"
#include "G_log.h"
#include "G_graphic.h"
#include "G_resource.h"
#include "G_TileMap/G_TileSet.h"
#include "G_staticModel.h"
#include "G_map.h"
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
Uint32 width = 600;
Uint32 height = 600;

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
        SDL_free(iconPixels);
        return false;
    }

    if (!SDL_SetWindowIcon(window_3D, iconSurface))
        return false;

    SDL_DestroySurface(iconSurface);
    SDL_free(iconPixels);

    return true;
}

static VkAllocationCallbacks SDL_allocationCallBacks = {};

static VkFormat swapchainFormat = 0;

static VkPipelineShaderStageCreateInfo * graphciShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * modelShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * bottomShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * shadowShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * particleShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * shapeShaderStageCreateInfo = NULL;

// static VkShaderModule compShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * computeShaderStageCreateInfo = NULL;

static VkPipelineShaderStageCreateInfo * SSGIShaderStageCreateInfo = NULL;
static VkPipelineShaderStageCreateInfo * combineShaderStageCreateInfo = NULL;

static VkDescriptorSetLayout * graphicDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * modelDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * bottomDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * shadowDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * particleDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * shapeDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * computeDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * SSGIDescriptorSetLayout = NULL;
static VkDescriptorSetLayout * combineDescriptorSetLayout = NULL;

static VkDeviceMemory graphicUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject ubo = {};

static VkDeviceMemory graphic3DUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject ubo3D = {};

static VkDeviceMemory UIUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject uboUI = {};

static VkDeviceMemory tilemapUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject uboTilemap = {};

static VkDeviceMemory computeUniformBuffersmemory[MAX_FRAMES_IN_FLIGHT];
static ComputeUniformBufferObject computeUbo = {};

static VkDeviceMemory SSGIUniformBufferMem[MAX_FRAMES_IN_FLIGHT];
static SSGIUniformBufferObject SSGIubo = {};

static VkDeviceMemory SunUniformBufferMem[MAX_FRAMES_IN_FLIGHT];
static DirectionLight  Sunubo = {};

static VkDeviceMemory lightSpaceUniformBufferMem[MAX_FRAMES_IN_FLIGHT];
static LightSpace lightSpaceubo = {};


static VkDescriptorPool graphicDescriptorPool = NULL;

static VkDescriptorPool computeDescriptorPool = NULL;

static float camera_X = 0.0f;
static float camera_Y = 0.0f;

static float pictureX = 0;
static float pictureY = 0;

// static bool moveEnabled = false;

static VkDeviceMemory shaderStorageBuffersMem[MAX_FRAMES_IN_FLIGHT];

static PushConstants picturePushConstants = {0.0f};
static ShapeConstants shapePushConstants = {(vec2){0.0f, 0.0f}, (vec2){0.053333333f, 0.053333333f}};

static G_StaticModelPool staticModelPool = {};

static G_Thread_Pool threadPool = {};

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
    allInOne.pTilemapUbo = &uboTilemap;

    allInOne.pComputeUbo = &computeUbo;

    allInOne.pCamera_X = &camera_X;
    allInOne.pCamera_Y = &camera_Y;

    allInOne.pPictureX = &pictureX;
    allInOne.pPictureY = &pictureY;

    allInOne.pPushConstants = &picturePushConstants;
    allInOne.pShapeConstants = &shapePushConstants;

    initStack(&allInOne.bottomImageDrawStack, sizeof(DrawHere), NULL, NULL);
    initStack(&allInOne.bottomImageMoveStack, sizeof(FromTo), NULL, NULL);

    createThreadPool(&threadPool, 1, false);
    allInOne.pThreadPool = &threadPool;
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
    SDL_allocationCallBacks.pfnAllocation = SDL_VK_alloc;
    SDL_allocationCallBacks.pfnReallocation = SDL_VK_realloc;
    SDL_allocationCallBacks.pfnFree = SDL_VK_free;
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

    createGraphicRenderPass(swapchainFormat, &allInOne.renderPass);
    CO_addRenderPass(allInOne.renderPass);// CO
    VkImageView color2dImageViews[] = {color2dTexture->imageView};
    createFrameBuffer(2, allInOne.extent2D.width, allInOne.extent2D.height, 1, color2dImageViews, NULL, allInOne.renderPass, &allInOne.pGraphic2dFramebuffer);
    CO_addFrameBuffer(2, allInOne.pGraphic2dFramebuffer);// CO
 
    createTextureSampler(&allInOne.textureSampler);
    CO_addSampler(allInOne.textureSampler);// CO
    createNormalSampler(&allInOne.normalSampler);
    CO_addSampler(allInOne.normalSampler);// CO
    createDepthSampler(&allInOne.depthSampler);
    CO_addSampler(allInOne.depthSampler);// CO
    createShadowSampler(&allInOne.shadowSampler);
    CO_addSampler(allInOne.shadowSampler);// CO

    Vertex23 shapeVertex[5] =
    {
        {{-0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}, // 0: 左下
        {{ 0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}, // 1: 右下
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}}, // 2: 右上
        {{-0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}}, // 3: 左上
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}  // 4: 回到左下 (闭合)
    };
    createVertexBuffer(&allInOne.tempBuffer, &allInOne.tempBufferMemory, NULL, shapeVertex, 5 * sizeof(Vertex23), false);
    CO_addBuffer(false, allInOne.tempBuffer, allInOne.tempBufferMemory, NULL);

    Vertex33 tileMapVertex[VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP] = {};
    initVertices33(BOTTOM_WIDTH, BOTTOM_HEIGHT, 50, 50, 0.1f, tileMapVertex);
    createVertexBuffer(&allInOne.tileMapVertexBuffer, &allInOne.tileMapVertexBufferMem, NULL, tileMapVertex, VERTEX_COUNT_IN_UNIT_2D * MAX_TILES_IN_GROUP * sizeof(Vertex33), false);
    CO_addBuffer(false, allInOne.tileMapVertexBuffer, allInOne.tileMapVertexBufferMem, NULL);// CO

    allInOne.pTileMapUVs = (vec2*)SDL_calloc(MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D, sizeof(vec2));
    createVertexBuffer(allInOne.tileMapTexCoordBuffer + 0, allInOne.pTimeMapTexCoordBufferMem + 0, allInOne.pTimeMapTexCoordBufferMapped + 0, allInOne.pTileMapUVs, MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D * sizeof(vec2), true);
    CO_addBuffer(true, allInOne.tileMapTexCoordBuffer[0], allInOne.pTimeMapTexCoordBufferMem[0], NULL);// CO
    createVertexBuffer(allInOne.tileMapTexCoordBuffer + 1, allInOne.pTimeMapTexCoordBufferMem + 1, allInOne.pTimeMapTexCoordBufferMapped + 1, allInOne.pTileMapUVs, MAX_MAP_GROUP * MAX_TILES_IN_GROUP * VERTEX_COUNT_IN_UNIT_2D * sizeof(vec2), true);
    CO_addBuffer(true, allInOne.tileMapTexCoordBuffer[1], allInOne.pTimeMapTexCoordBufferMem[1], allInOne.pTileMapUVs);// CO

    allInOne.pVertices2D = (Vertex332*)SDL_calloc(VERTEX_COUNT_IN_BUFFER_2D, sizeof(Vertex332));
    allInOne.maxVertices2DCount = VERTEX_COUNT_IN_BUFFER_2D;
    createVertexBuffer(allInOne.vertexBuffer2D + 0, allInOne.pVertexBuffer2DMem + 0, allInOne.pVertexBuffer2DMemMapped + 0, NULL, VERTEX_COUNT_IN_BUFFER_2D * sizeof(Vertex332), true);
    CO_addBuffer(true, allInOne.vertexBuffer2D[0], allInOne.pVertexBuffer2DMem[0], NULL);// CO
    createVertexBuffer(allInOne.vertexBuffer2D + 1, allInOne.pVertexBuffer2DMem + 1, allInOne.pVertexBuffer2DMemMapped + 1, NULL, VERTEX_COUNT_IN_BUFFER_2D * sizeof(Vertex332), true);
    CO_addBuffer(true, allInOne.vertexBuffer2D[1], allInOne.pVertexBuffer2DMem[1], allInOne.pVertices2D);// CO

    allInOne.pIndices2D = (Uint16 *)SDL_calloc(INDEX_COUNT_IN_BUFFER_2D, sizeof(Uint16 ));
    indexInitialize(allInOne.pIndices2D, MAX_UNIT_COUNT_2D);

    createIndexBuffer(&allInOne.indexBuffer2D, &allInOne.indexBuffer2DMem, NULL, allInOne.pIndices2D, INDEX_COUNT_IN_BUFFER_2D, sizeof(Uint16), false);
    CO_addBuffer(false, allInOne.indexBuffer2D, allInOne.indexBuffer2DMem, NULL);// CO

    SDL_free(allInOne.pIndices2D);

    allInOne.pVertices3D = (Vertex3323*)SDL_calloc(30000, sizeof(Vertex3323));
    allInOne.maxVertices3DCount = 30000;
    allInOne.pIndices3D = (Uint32*)SDL_calloc(45000, sizeof(Uint32));
    createVertexBuffer(allInOne.vertexBuffer3D + 0, allInOne.vertexBuffer3DMem + 0, allInOne.pVertexBuffer3DMemMapped + 0, NULL, 30000 * sizeof(Vertex3323), true);
    CO_addBuffer(true, allInOne.vertexBuffer3D[0], allInOne.vertexBuffer3DMem[0], NULL);// CO
    createVertexBuffer(allInOne.vertexBuffer3D + 1, allInOne.vertexBuffer3DMem + 1, allInOne.pVertexBuffer3DMemMapped + 1, NULL, 30000 * sizeof(Vertex3323), true);
    CO_addBuffer(true, allInOne.vertexBuffer3D[1], allInOne.vertexBuffer3DMem[1], NULL);// CO

    createIndexBuffer(allInOne.indexBuffer3D + 0, allInOne.indexBuffer3DMem + 0, allInOne.pIndexBuffer3DMemMapped + 0, allInOne.pIndices3D, 45000, sizeof(Uint32), true);
    CO_addBuffer(true, allInOne.indexBuffer3D[0], allInOne.indexBuffer3DMem[0], NULL);// CO
    createIndexBuffer(allInOne.indexBuffer3D + 1, allInOne.indexBuffer3DMem + 1, allInOne.pIndexBuffer3DMemMapped + 1, allInOne.pIndices3D, 45000, sizeof(Uint32), true);
    CO_addBuffer(true, allInOne.indexBuffer3D[1], allInOne.indexBuffer3DMem[1], allInOne.pIndices3D);// CO

    createUniformBufferByBuffering(&allInOne.pGraphicUniformBuffer, &graphicUniformBuffersMemory, &allInOne.ppGraphicUniformBufferMapped, sizeof(UniformBufferObject));
    CO_addBuffer(true, allInOne.pGraphicUniformBuffer[0], graphicUniformBuffersMemory[0], NULL);// CO
    CO_addBuffer(true, allInOne.pGraphicUniformBuffer[1], graphicUniformBuffersMemory[1], NULL);// CO
    createUniformBufferByBuffering(&allInOne.pGraphic3DUniformBuffer, &graphic3DUniformBuffersMemory, &allInOne.ppGraphic3DUniformBufferMapped, sizeof(UniformBufferObject));
    CO_addBuffer(true, allInOne.pGraphic3DUniformBuffer[0], graphic3DUniformBuffersMemory[0], NULL);// CO
    CO_addBuffer(true, allInOne.pGraphic3DUniformBuffer[1], graphic3DUniformBuffersMemory[1], NULL);// CO
    createUniformBufferByBuffering(&allInOne.pUIUniformBuffer, &UIUniformBuffersMemory, &allInOne.ppUIUniformBufferMapped, sizeof(UniformBufferObject));
    CO_addBuffer(true, allInOne.pUIUniformBuffer[0], UIUniformBuffersMemory[0], NULL);// CO
    CO_addBuffer(true, allInOne.pUIUniformBuffer[1], UIUniformBuffersMemory[1], NULL);// CO
    createUniformBufferByBuffering(&allInOne.pTilemapUniformBuffer, &tilemapUniformBuffersMemory, &allInOne.ppTilemapUniformBufferMapped, sizeof(UniformBufferObject));
    CO_addBuffer(true, allInOne.pTilemapUniformBuffer[0], tilemapUniformBuffersMemory[0], NULL);// CO
    CO_addBuffer(true, allInOne.pTilemapUniformBuffer[1], tilemapUniformBuffersMemory[1], NULL);// CO

    createUniformBufferByBuffering(&allInOne.pComputeUniformBuffer, &computeUniformBuffersmemory, &allInOne.ppComputeUniformBufferMapped, sizeof(ComputeUniformBufferObject));
    CO_addBuffer(true, allInOne.pComputeUniformBuffer[0], computeUniformBuffersmemory[0], NULL);// CO
    CO_addBuffer(true, allInOne.pComputeUniformBuffer[1], computeUniformBuffersmemory[1], NULL);// CO

    createUniformBufferByBuffering(&allInOne.pSSGIUniformBuffer, &SSGIUniformBufferMem, &allInOne.ppSSGIUniformBufferMapped, sizeof(SSGIUniformBufferObject));
    CO_addBuffer(true, allInOne.pSSGIUniformBuffer[0], SSGIUniformBufferMem[0], NULL);// CO
    CO_addBuffer(true, allInOne.pSSGIUniformBuffer[1], SSGIUniformBufferMem[1], NULL);// CO

    createUniformBufferByBuffering(&allInOne.pSunUniformBuffer, &SunUniformBufferMem, &allInOne.ppSunUniformBufferMapped, sizeof(DirectionLight));
    CO_addBuffer(true, allInOne.pSunUniformBuffer[0], SunUniformBufferMem[0], NULL);// CO
    CO_addBuffer(true, allInOne.pSunUniformBuffer[1], SunUniformBufferMem[1], NULL);// CO

    createUniformBufferByBuffering(&allInOne.pLightSpaceUniformBuffer, &lightSpaceUniformBufferMem, &allInOne.ppLightSpaceUniformBufferMapped, sizeof(LightSpace));
    CO_addBuffer(true, allInOne.pLightSpaceUniformBuffer[0], lightSpaceUniformBufferMem[0], NULL);// CO
    CO_addBuffer(true, allInOne.pLightSpaceUniformBuffer[1], lightSpaceUniformBufferMem[1], NULL);// CO

    createShaderStorageBuffers(&allInOne.pShaderStorageBuffer, &shaderStorageBuffersMem);
    CO_addBuffer(false, allInOne.pShaderStorageBuffer[0], shaderStorageBuffersMem[0], NULL);// CO
    CO_addBuffer(false, allInOne.pShaderStorageBuffer[1], shaderStorageBuffersMem[1], NULL);// CO

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
    createDescriptorSets(&graphicDescriptorPool, graphicDescriptorSetLayout, graphicSetCount, 5, &allInOne.pGraphicDescriptorSets);
    createGraphicsPipeline(allInOne.extent2D, 2, graphciShaderStageCreateInfo, allInOne.graphicPipelineLayout, allInOne.renderPass, &allInOne.graphicPipeline);
    createTileMapPipeline(2, graphciShaderStageCreateInfo, allInOne.graphicPipelineLayout, allInOne.renderPass, &allInOne.tilemapPipeline);

    //3d model shader
    PathType modelTypes[] = {Model3dVertShader, Model3dFragShader};
    VkShaderModule * modelTempModule = NULL;
    Uint32 modelSetCount = CreateShaderModulesAndDescriptorSets(modelTypes, 2, &modelTempModule, &modelShaderStageCreateInfo, &modelDescriptorSetLayout, &allInOne.modelPipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, modelDescriptorSetLayout, modelSetCount, 1, &allInOne.pModelDescriptorSets);
    createModelPipeline(allInOne.extent2D, 2, modelShaderStageCreateInfo, allInOne.modelPipelineLayout, allInOne.modelRenderPass, &allInOne.modelPipeline);

    // 3d bottom shader
    PathType bottomTypes[] = {Model3dVertShader, ModelBottomFragShader};
    VkShaderModule * bottomTempModule = NULL;
    Uint32 bottomSetCount = CreateShaderModulesAndDescriptorSets(bottomTypes, 2, &bottomTempModule, &bottomShaderStageCreateInfo, &bottomDescriptorSetLayout, &allInOne.bottomPipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, bottomDescriptorSetLayout, bottomSetCount, 1, &allInOne.pBottomDescriptorSets);
    createModelPipeline(allInOne.extent2D, 2, bottomShaderStageCreateInfo, allInOne.bottomPipelineLayout, allInOne.modelRenderPass, &allInOne.bottomPipeline);

    // particle shader
    PathType particleTypes[] = {ParticleVertShader, ParticleFragShader};
    VkShaderModule * particleTempModule = NULL;
    Uint32 particleSetCount = CreateShaderModulesAndDescriptorSets(particleTypes, 2, &particleTempModule, &particleShaderStageCreateInfo, &particleDescriptorSetLayout, &allInOne.particlePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, particleDescriptorSetLayout, particleSetCount, 1, &allInOne.pParticleDescriptorSets);
    createParticlePipeline(allInOne.extent2D, 2, particleShaderStageCreateInfo, allInOne.particlePipelineLayout, allInOne.renderPass, &allInOne.particlePipeline);

    // shape shader
    PathType shapeTypes[] = {ShapeVertShader, ShapeFragShader};
    VkShaderModule * shapeTempModule = NULL;
    Uint32 shapeSetCount =CreateShaderModulesAndDescriptorSets(shapeTypes, 2, &shapeTempModule, &shapeShaderStageCreateInfo, &shapeDescriptorSetLayout, &allInOne.shapePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, shapeDescriptorSetLayout, shapeSetCount, 1, &allInOne.pShapeDescriptorSets);
    createShapePipeline(allInOne.extent2D, 2, shapeShaderStageCreateInfo, allInOne.shapePipelineLayout, allInOne.renderPass, &allInOne.shapePipeline);

    // combine Shader
    PathType combineTypes[] = {CombineVertShader, CombineFragShader};
    VkShaderModule * combineTempModule = NULL;
    Uint32 combineSetCount = CreateShaderModulesAndDescriptorSets(combineTypes, 2, &combineTempModule, &combineShaderStageCreateInfo, &combineDescriptorSetLayout, &allInOne.combinePipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, combineDescriptorSetLayout, combineSetCount, 1, &allInOne.pCombineDescriptorSets);
    createCombinePipeline(allInOne.extent2D, 2, combineShaderStageCreateInfo, allInOne.combinePipelineLayout, allInOne.combineRenderPass, &allInOne.combinePipeline);

    //shadow shader
    PathType shadowTypes[] = {ShadowVertShader, EmptyFragShader};
    VkShaderModule * shadowTempModule = NULL;
    Uint32 shadowSetCount = CreateShaderModulesAndDescriptorSets(shadowTypes, 2, &shadowTempModule, &shadowShaderStageCreateInfo, &shadowDescriptorSetLayout, &allInOne.shadowPipelineLayout);
    createDescriptorSets(&graphicDescriptorPool, shadowDescriptorSetLayout, shadowSetCount, 1, &allInOne.pShadowDescriptorSets);
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
    addComputePipeline(computeShaderStageCreateInfo, allInOne.computePipelineLayout, NULL, 0, &allInOne.computePipeline);

    // SSGI shader
    PathType SSGITypes[] = {SSGICompShader};
    VkShaderModule * SSGITempModule = NULL;
    Uint32 SSGISetCount = CreateShaderModulesAndDescriptorSets(SSGITypes, 1, &SSGITempModule, &SSGIShaderStageCreateInfo, &SSGIDescriptorSetLayout, &allInOne.SSGIPipelineLayout);
    createDescriptorSets(&computeDescriptorPool, SSGIDescriptorSetLayout, SSGISetCount, 1, &allInOne.pSSGIDescriptorSets);
    addComputePipeline(SSGIShaderStageCreateInfo, allInOne.SSGIPipelineLayout, NULL, 0, &allInOne.SSGIPipeline);

    executeCreateComputePipelines(NULL);

    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.graphicCommandPool, &allInOne.pGraphicCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.presentCommandPool, &allInOne.pPresentCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.computeCommandPool, &allInOne.pComputeCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, allInOne.transferCommandPool, &allInOne.pTransferCommandBuffer);

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
   
    loadTileSet(TileSet1Png, TileSet1Tsd, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_TILE_SET, allInOne.pGraphicDescriptorSets + 6);
    loadTileMap(TileMap1TsdI, TEXTURE_TILE_SET, MAIN_TILE_MAP);
    // loadTileMap(TileMap1TsdI, -1200, -1100, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -1200, -300, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -1200, 500, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -400, 500, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, 400, 500, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, 400, -300, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, 400, -1100, TEXTURE_TILE_SET);
    // loadTileMap(TileMap1TsdI, -400, -1100, TEXTURE_TILE_SET);
    createStaticModelPool(&staticModelPool, 58);
    loadStaticModel(&staticModelPool, 48, BottomObj, BottomPng, allInOne.pVertices3D, &allInOne.vertices3DCount, allInOne.pIndices3D, &allInOne.indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_BOTTOM, allInOne.pBottomDescriptorSets, true);
    loadStaticModel(&staticModelPool, 10, BoxObj, BoxPng, allInOne.pVertices3D, &allInOne.vertices3DCount, allInOne.pIndices3D, &allInOne.indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_MODEL, allInOne.pModelDescriptorSets, false);

    VkImage imageArray = NULL;
    VkDeviceMemory imageArrayMemory = NULL;
    VkImageView imageArrayView = NULL;
    VkImageView * imageArrayViews = NULL;
    createImageArray(800, 800, 48, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT\
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &imageArray, &imageArrayMemory);
    createImageViewArray(imageArray, 48, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, &imageArrayView);
    addTexture(800, 800, VK_FORMAT_R8G8B8A8_SRGB, imageArray, imageArrayMemory, imageArrayView, allInOne.pBottomDescriptorSets + 2, 48, VK_IMAGE_LAYOUT_UNDEFINED, TEXTURE_MAP_ARRAY);
    transitionImageLayout(NULL, getTexture(TEXTURE_MAP_ARRAY), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 48);

    createImageViewsForImageArray(imageArray, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 48, &imageArrayViews);
    createFrameBufferByImageArray(48, 800, 800, imageArrayViews, allInOne.offscreenRenderPass, &allInOne.pBottomImageArrayFramebuffers);
    CO_addFrameBuffer(48, allInOne.pBottomImageArrayFramebuffers);// CO

    loadImageResource(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_GENERAL, TEXTURE_SSGI_STORAGE_IMAGE, allInOne.pSSGIDescriptorSets + 2);

    loadTexture(Loading1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_LOADING, allInOne.pGraphicDescriptorSets);
    loadTexture(CirclePng, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_CIRCLE, allInOne.pGraphicDescriptorSets + 2);
    loadTexture(MainFontPng, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_FONT, allInOne.pGraphicDescriptorSets + 4);
    // loadTexture(Box1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_BOX, graphicDescriptorSets + 8);

    addDescriptorSetToTexture(TEXTURE_MODEL_DEPTH, allInOne.pSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_NORMAL, allInOne.pSSGIDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, allInOne.pSSGIDescriptorSets + 0);

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
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, loadingTexture->pDescriptorSet, allInOne.pUIUniformBuffer, 0, sizeof(UniformBufferObject));//0
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, fontTexture->pDescriptorSet, allInOne.pUIUniformBuffer, 0, sizeof(UniformBufferObject));

    // graphic
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, circleTexture->pDescriptorSet, allInOne.pGraphicUniformBuffer, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, tileSetTexture->pDescriptorSet, allInOne.pTilemapUniformBuffer, 0, sizeof(UniformBufferObject));
    // addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, getTexture(TEXTURE_BOX)->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));

    // model
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, modelTexture->pDescriptorSet, allInOne.pGraphic3DUniformBuffer, 0, sizeof(UniformBufferObject));//4
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, bottomTexture->pDescriptorSet, allInOne.pGraphic3DUniformBuffer, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, modelTexture->pDescriptorSet, allInOne.pSunUniformBuffer, 0, sizeof(DirectionLight));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, bottomTexture->pDescriptorSet, allInOne.pSunUniformBuffer, 0, sizeof(DirectionLight));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pShadowDescriptorSets, allInOne.pLightSpaceUniformBuffer, 0, sizeof(LightSpace));

    // shape
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pShapeDescriptorSets, allInOne.pGraphic3DUniformBuffer, 0, sizeof(UniformBufferObject));

    // SSGI
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, normalTexture->pDescriptorSet, allInOne.pSSGIUniformBuffer, 0, sizeof(SSGIUniformBufferObject));

    // graphics
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_LOADING, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_CIRCLE, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//8
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_FONT, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_TILE_SET, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // addDescriptorSetToTexture(TEXTURE_2D_COLOR, combine2DDescriptorSets);
    // addDescriptorSetToTexture(TEXTURE_SHADOW_MAP, combine2DDescriptorSets);
    // addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_2D_COLOR, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_SHADOW_MAP, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // model
    addShadowDescriptorSetToTexture(TEXTURE_MODEL, shadowTexture->pDescriptorSet);
    addShadowDescriptorSetToTexture(TEXTURE_BOTTOM, shadowTexture->pDescriptorSet);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_MODEL, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_BOTTOM, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//12
    addDescriptorSetToTexture(TEXTURE_SHADOW, modelTexture->pDescriptorSet);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_SHADOW, allInOne.shadowSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorSetToTexture(TEXTURE_SHADOW, bottomTexture->pDescriptorSet);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_SHADOW, allInOne.shadowSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, TEXTURE_MAP_ARRAY, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // SSGI
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_DEPTH, allInOne.depthSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_NORMAL, allInOne.normalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_MODEL_COLOR, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, TEXTURE_SSGI_STORAGE_IMAGE, NULL, VK_IMAGE_LAYOUT_GENERAL);

    // graphic
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pParticleDescriptorSets, allInOne.pGraphicUniformBuffer, 0, sizeof(UniformBufferObject));//16

    // compute
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, allInOne.pComputeDescriptorSets, allInOne.pComputeUniformBuffer, 0, sizeof(ComputeUniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, allInOne.pComputeDescriptorSets, allInOne.pShaderStorageBuffer, 0, sizeof(Particle) * PARTICLE_COUNT);
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, allInOne.pComputeDescriptorSets, allInOne.pShaderStorageBuffer, 0, sizeof(Particle) * PARTICLE_COUNT);

    executeUpdateDescriptorSets();

    addDescriptorSetToTexture(TEXTURE_MODEL_COLOR, allInOne.pCombineDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_SSGI_STORAGE_IMAGE, allInOne.pCombineDescriptorSets + 0);
    addDescriptorSetToTexture(TEXTURE_2D_COLOR, allInOne.pCombineDescriptorSets + 0);
    // combine
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, TEXTURE_MODEL_COLOR, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_SSGI_STORAGE_IMAGE, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, TEXTURE_2D_COLOR, allInOne.textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);//16

    executeUpdateDescriptorSets();

    // initializeRecreate();

    resultVulkan(VK_SUCCESS, 0);
}

void cleanVulkan(void)
{
    print("\nclean begin");

    vkDeviceWaitIdle(allInOne.device);

    destroyThreadPool(allInOne.pThreadPool);

    destroyStaticModelPool(&staticModelPool);
    unloadAllTexture();
    CO_CleanAllVkResource();
}
    /*vertices2D[4] = (Vertex332){{-0.0f, -0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices2D[5] = (Vertex332){{1.0f, -0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices2D[6] = (Vertex332){{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices2D[7] = (Vertex332){{-0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};*/