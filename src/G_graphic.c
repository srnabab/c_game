#include "G_constants.h"
#include "G_log.h"
#include "G_graphic.h"
#include "G_resource.h"
#include "G_TileMap/G_TileSet.h"
#include "G_staticModel.h"
#include "G_custom_math.h"

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include "SDL3/SDL_vulkan.h"

// #include "vma/vk_mem_alloc.h"

#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_framebuffer.h"
#include "vk_code_h/vk_descriptorPool.h"
#include "vk_code_h/vk_commandPool.h"
#include "vk_code_h/vk_synchronize.h"
#include "vk_code_h/vk_computeShader.h"
#include "vk_code_h/vk_uniform.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_index.h"
#include "vk_code_h/vk_vertex.h"
#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_renderPass.h"
#include "vk_code_h/vk_pipeline.h"
#include "vk_code_h/vk_shader.h"
#include "vk_code_h/vk_swapchain.h"
#include "vk_code_h/vk_alloc_func.h"
#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_device.h"
#include "vk_code_h/vk_version.h"
#include "vk_code_h/vk_surface.h"
#include "vk_code_h/vk_instance.h"
#include "vk_code_h/vk_move.h"
#include "vk_code_h/vk_load_model.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "spirv_reflect/shader_resolve.h"

// declare a sdl window_2D
SDL_Window * window_2D = NULL;
SDL_DisplayID displayId = 0;

// windows' width and height
Uint32 width = 800;
Uint32 height = 600;

float physicalCoffectX = 1.0f;
float physicalCoffectY = 1.0f;

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

#if WINDOW_3D_DEBUG
SDL_Window * window_3D = NULL;
static bool initWindow_3D(void)
{
   //create sdl window_2D and sign window_2D as a vulkan window_2D
    window_3D = SDL_CreateWindow("Vulkan_3D", width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    int x, y;
    SDL_GetWindowPosition(window_3D, &x, &y);
    SDL_SetWindowPosition(window_3D, x - 800, y);
    if (window_3D == NULL)
        return false;
    
    logMessage("window_3D initialized");

    return true;
}
#endif

bool initWindow_2D(void)
{
   //create sdl window_2D and sign window_2D as a vulkan window_2D
    if (!initSDL()) return false;
    window_2D = SDL_CreateWindow("Vulkan_2D", width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (window_2D == NULL)
        return false;
    
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

    logMessage("window_2D initialized");

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

    if (!SDL_SetWindowIcon(window_2D, iconSurface))
        return false;

    SDL_DestroySurface(iconSurface);
    SDL_free(iconPixels);

#if WINDOW_3D_DEBUG
    bool res_3d = initWindow_3D();
    if (!res_3d) return false;
#endif

    return true;
}

static VkInstance instance = NULL;

static VkSurfaceKHR surface2D = NULL;

static VkPhysicalDevice physicalDevice = NULL;

static QueueFamilyIndices queueIndices = {};
static VkDevice device = NULL;
static VkQueue graphicQueue = NULL;
static VkQueue presentQueue = NULL;
static VkQueue computeQueue = NULL;
static VkQueue transferQueue = NULL;

static VkAllocationCallbacks SDL_allocationCallBacks = {};

static VkSurfaceFormatKHR surface2DFormat = {};
static VkPresentModeKHR presentMode2D = 0;
static VkSurfaceCapabilitiesKHR surface2DCapabilities = {};
static VkExtent2D extent2D = {};
static VkExtent2D oldExtent2D = {};
static VkSwapchainKHR swapchain2D = NULL;

static Uint32 imageCount2D = 0;
static VkFormat swapchainFormat = 0;

static VkImage * swapchain2DImages = NULL;
static VkImageView * swapchain2DImageViews = NULL;
static VkFramebuffer * swapchain2DFramebuffer = NULL;

// static VkShaderModule vertShaderCode = NULL;
// static VkShaderModule fragShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * graphciShaderStageCreateInfo = NULL;

// static VkShaderModule modelVertShaderCode = NULL;
// static VkShaderModule modelFragShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * modelShaderStageCreateInfo = NULL;

// static VkShaderModule particleVertexShaderCode = NULL;
// static VkShaderModule particleFragmentShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * particleShaderStageCreateInfo = NULL;

// static VkShaderModule compShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * computeShaderStageCreateInfo = NULL;

static VkDescriptorSetLayout * graphicDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * modelDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * particleDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * computeDescriptorSetLayout = NULL;

// static Uint32 graphicBinding = 0;
static VkDescriptorSetLayoutBinding * graphicBindings = NULL;
static VkPipelineLayout graphicPipelineLayout = NULL;

static VkPipelineLayout modelPipelineLayout = NULL;

static VkDescriptorSetLayoutBinding * particleBindings = NULL;
static VkPipelineLayout particlePipelineLayout = NULL;

// static Uint32 computeBinding = 0;
static VkDescriptorSetLayoutBinding * computeBindings = NULL;
static VkPipelineLayout computePipelineLayout = NULL;

static VkRenderPass renderPass = NULL;

static VkPipeline graphicPipeline = NULL;
static VkPipeline modelPipeline = NULL;

static VkPipeline particlePipeline = NULL;

static VkPipeline computePipeline = NULL;

static VkCommandPool graphicCommandPool = NULL;

static VkCommandPool presentCommandPool = NULL;

static VkCommandPool transferCommandPool = NULL;

static VkCommandPool computeCommandPool = NULL;

static VkSampler textureSampler = NULL;

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

static VkDescriptorPoolSize * graphicDescriptorPoolSize = NULL;
static VkDescriptorPool graphicDescriptorPool = NULL;
static VkDescriptorSet * graphicDescriptorSets = NULL;
static VkDescriptorSet * modelDescriptorSets = NULL;

static VkDescriptorPoolSize * particleDescriptorPoolSize = NULL;
static VkDescriptorPool particleDescriptorPool = NULL;
static VkDescriptorSet * particleDescriptorSets = NULL;

static VkDescriptorPoolSize * computeDescriptorPoolSize = NULL;
static VkDescriptorPool computeDescriptorPool = NULL;
static VkDescriptorSet * computeDescriptorSets = NULL;

static VkCommandBuffer graphicCommandBuffer[MAX_FRAMES_IN_FLIGHT];
static VkCommandBuffer presentCommandBuffer[MAX_FRAMES_IN_FLIGHT];
static VkCommandBuffer computeCommandBuffer[MAX_FRAMES_IN_FLIGHT];
static VkCommandBuffer transferCommandBuffer[MAX_FRAMES_IN_FLIGHT];

static VkSemaphore imageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
static VkSemaphore renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];

static VkFence graphicInFlightFence[2];

static VkSemaphore computeFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
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

#if WINDOW_3D_DEBUG
static VkSurfaceKHR surface3D = NULL;

static VkSurfaceFormatKHR surface3DFormat = {};
static VkPresentModeKHR presentMode3D = 0;
static VkSurfaceCapabilitiesKHR surface3DCapabilities = {};

static VkSwapchainKHR swapchain3D = NULL;
static Uint32 imageCount3D = 0;

static VkImage * swapchain3DImages = NULL;
static VkImageView * swapchain3DImageViews = NULL;
static VkFramebuffer * swapchain3DFramebuffer = NULL;
#endif

static G_StaticModelPool staticModelPool = {};

//store all compoents for initialize vulkan in a struct
VK_ALL allInOne = {};

static void initializeAllInOne(void)
{
    allInOne.pAllocationCallbacks = &SDL_allocationCallBacks;

    allInOne.pInstance = &instance;

    allInOne.pPhysicalDevice = &physicalDevice;

    allInOne.pDevice = &device;

    allInOne.pSurface2DCapabilities = &surface2DCapabilities;
    allInOne.pSurface2DFormat = &surface2DFormat;
    allInOne.pPresentMode2D = &presentMode2D;

    allInOne.pSurface2D = &surface2D;

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
    allInOne.pImageCount2D = &imageCount2D;

    allInOne.pSwapchain2D = &swapchain2D;
    allInOne.ppSwapchain2DImages = &swapchain2DImages;
    allInOne.ppSwapchain2DImageViews = &swapchain2DImageViews;
    allInOne.ppSwapchain2DFramebuffer = &swapchain2DFramebuffer;
 
    allInOne.pRenderPass = &renderPass;

    allInOne.pGraphicPipelineLayout = &graphicPipelineLayout;
    allInOne.pGraphicPipeline = &graphicPipeline;

    allInOne.pParticlePipelineLayout = &particlePipelineLayout;
    allInOne.pParticlePipeline = &particlePipeline;

    allInOne.pComputePipelineLayout = &computePipelineLayout;
    allInOne.pComputePipeline = &computePipeline;

    allInOne.pModelPipelineLayout = &modelPipelineLayout;
    allInOne.pModelPipeline = &modelPipeline;

   
#if WINDOW_3D_DEBUG

    allInOne.pSurface3D = &surface3D;
    allInOne.pSurface3DFormat = &surface3DFormat;
    allInOne.pSurface3DCapabilities = &surface3DCapabilities;
    allInOne.pPresentMode3D = &presentMode3D;
    allInOne.pSwapchain3D = &swapchain3D;
    allInOne.pImageCount3D = &imageCount3D;
    allInOne.ppSwapchain3DImages = &swapchain3DImages;
    allInOne.ppSwapchain3DImageViews = &swapchain3DImageViews;
    allInOne.ppSwapchain3DFramebuffer = &swapchain3DFramebuffer;

#endif

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

    allInOne.ppGraphicDescriptorSets = &graphicDescriptorSets;

    allInOne.ppParticleDescriptorSets = &particleDescriptorSets;

    allInOne.pComputeUbo = &computeUbo;

    allInOne.pppComputeUniformBufferMapped = &computeUniformBufferMapped;

    allInOne.ppComputeDescriptorSets = &computeDescriptorSets;

    allInOne.ppShaderStorageBuffers = &shaderStorageBuffers;

    allInOne.ppGraphicCommandBuffer = &graphicCommandBuffer;
    allInOne.ppPresentCommandBuffer = &presentCommandBuffer;
    allInOne.ppComputeCommandBuffer = &computeCommandBuffer;
    allInOne.ppTransferCommandBuffer = &transferCommandBuffer;

    allInOne.ppImageAvailableSemaphore = &imageAvailableSemaphore;
    allInOne.ppRenderFinishedSemaphore = &renderFinishedSemaphore;

    allInOne.ppGraphicInFlightFence = &graphicInFlightFence;

    allInOne.ppComputeFinishedSemaphore = &computeFinishedSemaphore;

    allInOne.ppComputeInFlightFence = &computeInFlightFences;

    allInOne.pCurrentFrame = &currentFrame;

    allInOne.pCamera_X = &camera_X;
    allInOne.pCamera_Y = &camera_Y;

    allInOne.pPictureX = &pictureX;
    allInOne.pPictureY = &pictureY;

    allInOne.pPushConstants = &picturePushConstants;

    initStack(&allInOne.shaderModuleStack, sizeof(VkShaderModule), NULL, NULL);
}

void initVulkan(void)
{
    /*fixed compoent*/
    logMessage("initializing...");

    initGlobalTexture();


    initializeAllInOne();

    SDL_allocationCallBacks.pUserData = NULL;
    SDL_allocationCallBacks.pfnAllocation = SDL_VK_alloc;
    SDL_allocationCallBacks.pfnReallocation = SDL_VK_realloc;
    SDL_allocationCallBacks.pfnFree = SDL_VK_free;
    SDL_allocationCallBacks.pfnInternalAllocation = NULL;
    SDL_allocationCallBacks.pfnInternalFree = NULL;    

    vulkanVersion();

    createInstance();

    createSurface(window_2D, &surface2D);

    pickPhysicalDevice();

    findQueueFamilies();
    createLogicalDevice();

    createQueue(queueIndices.graphicsFamily.familyIndice, &graphicQueue);
    createQueue(queueIndices.presentFamily.familyIndice, &presentQueue);
    createQueue(queueIndices.computeFamily.familyIndice, &computeQueue);
    createQueue(queueIndices.transferFamily.familyIndice, &transferQueue);

    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.graphicsFamily.familyIndice, &graphicCommandPool);
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.presentFamily.familyIndice, &presentCommandPool);
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.computeFamily.familyIndice, &computeCommandPool);
    createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueIndices.transferFamily.familyIndice, &transferCommandPool);

    getSurfaceFormats(surface2D, &surface2DFormat);
    getPresentModes(&presentMode2D);
    getSurfaceCapabilities(surface2D, &surface2DCapabilities);
    
    extent2D.width = width;
    extent2D.height = height;

    createSwapchain(surface2D, surface2DCapabilities, surface2DFormat, presentMode2D, &swapchain2D, NULL);

    getSwapchainNumber(swapchain2D, &imageCount2D);
    createSwapchainImage(swapchain2D, &imageCount2D, &swapchain2DImages);

    swapchainFormat = surface2DFormat.format;

    //swapchain2D image view
    createSwapchainImageView(swapchain2DImages, imageCount2D, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, &swapchain2DImageViews);

    loadDepthResource(TEXTURE_DEPTH);
    G_Texture_P const * depthTexutre = getTexture(TEXTURE_DEPTH);
    createGraphicRenderPass(swapchainFormat, depthTexutre->format, &renderPass);

    // createDepthResoures(&depthImage, &depthImageMemory, &depthImageView);

    createFrameBuffer(imageCount2D, swapchain2DImageViews, &depthTexutre->imageView, &renderPass, &swapchain2DFramebuffer);
    
#if WINDOW_3D_DEBUG

    createSurface(window_3D, &surface3D);
    getSurfaceFormats(surface3D, &surface3DFormat);
    getPresentModes(&presentMode3D);
    getSurfaceCapabilities(surface3D, &surface3DCapabilities);
    createSwapchain(surface3D, surface3DCapabilities, surface3DFormat, presentMode3D, &swapchain3D, NULL);
    getSwapchainNumber(swapchain3D, &imageCount3D);
    createSwapchainImage(swapchain3D, &imageCount3D, &swapchain3DImages);
    createSwapchainImageView(swapchain3DImages, imageCount3D, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, &swapchain3DImageViews);
    createFrameBuffer(imageCount3D, swapchain3DImageViews, &depthTexutre->imageView, &renderPass, &swapchain3DFramebuffer);
#endif
    createTextureSampler(&physicalDevice, &device, &textureSampler);

    vertices2D = (Vertex*)SDL_calloc(VERTEX_COUNT_IN_BUFFER_2D, sizeof(Vertex));
    allInOne.maxVertices2DCount = VERTEX_COUNT_IN_BUFFER_2D;

    createVertexBuffer(&physicalDevice, &device, vertexBuffer2D, vertexBuffer2DMem, vertexBuffer2DMemMapped, VERTEX_COUNT_IN_BUFFER_2D * sizeof(Vertex));
    createVertexBuffer(&physicalDevice, &device, vertexBuffer2D + 1, vertexBuffer2DMem + 1, vertexBuffer2DMemMapped + 1, VERTEX_COUNT_IN_BUFFER_2D * sizeof(Vertex));

    indices2D = (Uint16 *)SDL_calloc(INDEX_COUNT_IN_BUFFER_2D, sizeof(Uint16 ));
    indexInitialize(indices2D, MAX_UNIT_COUNT_2D);

    createIndexBuffer(&physicalDevice, &device, indexBuffer2D, indexBuffer2DMem, indexBuffer2DMemMapped, indices2D, INDEX_COUNT_IN_BUFFER_2D, sizeof(Uint16), false);

    SDL_free(indices2D);

    vertices3D = (Vertex4*)SDL_calloc(30000, sizeof(Vertex4));
    allInOne.maxVertices3DCount = 30000;
    indices3D = (Uint32*)SDL_calloc(45000, sizeof(Uint32));
    // UniformBufferObject tempUbo = {};
    // glm_mat4_identity(tempUbo.model);
    // // glm_scale(tempUbo.model, (vec3){0.0266666f, 0.0266666f, 0.0266666f});
    // glm_scale(tempUbo.model, (vec3){0.5f ,0.5f, 0.5f});
    // glm_lookat((vec3){0.0f, 4.0f, 4.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, tempUbo.view);
    // glm_ortho_vulkan(-((float)4 / 3), ((float)4 / 3), -1.0f, 1.0f, 0.1f, 100.0f, tempUbo.proj);
    // vec4 tempVec4;
    // vec2 * XYs = SDL_malloc(vertices3DCount * sizeof(vec2));
    // float max_x, max_y, min_x, min_y;
    // max_x = max_y = -1000000.0f;
    // min_x = min_y = 1000000.0f;
    // for (int lk = 0;lk < vertices3DCount;lk++)
    // {
    //     glm_mul(tempUbo.proj, tempUbo.view, tempUbo.proj);
    //     glm_mul(tempUbo.proj, tempUbo.model, tempUbo.proj);
    //     tempVec4[0] = vertices3D[lk].pos[0];
    //     tempVec4[1] = vertices3D[lk].pos[1];
    //     tempVec4[2] = vertices3D[lk].pos[2];
    //     tempVec4[3] = 1.0f;
    //     for (int kl = 0;kl < 2;kl++)
    //     {
    //         XYs[lk][kl] = glm_vec4_dot(tempUbo.proj[kl], tempVec4);
    //     }
    //     XYs[lk][0] = (XYs[lk][0] * width) - width / 2;
    //     XYs[lk][1] = (XYs[lk][1] * height) - height / 2;
    //     // XYs[lk][0] = (XYs[lk][0] + 1.0f) * 800.0f;
    //     // XYs[lk][1] = (XYs[lk][1] + 1.0f) * 600.0f;
    //     max_x = (XYs[lk][0] > max_x) ? XYs[lk][0] : max_x;
    //     max_y = (XYs[lk][1] > max_y) ? XYs[lk][1] : max_y;
    //     min_x = (XYs[lk][0] < min_x) ? XYs[lk][0] : min_x;
    //     min_y = (XYs[lk][1] < min_y) ? XYs[lk][1] : min_y;
    //     // print("point:x: %f, y: %f", XYs[lk][0], XYs[lk][1]);
    // }
    // SDL_free(XYs);
    // // print("max point:x: %f, y: %f", max_x, max_y);
    // // print("min point:x: %f, y: %f", min_x, min_y);
    // print("width: %f, height: %f", max_x - min_x, (max_y - min_y) * SDL_cosf(M_PI / 4));
    createVertexBuffer(&physicalDevice, &device, vertexBuffer3D, vertexBuffer3DMem, vertexBuffer3DMemMapped, 30000 * sizeof(Vertex4));
    createVertexBuffer(&physicalDevice, &device, vertexBuffer3D + 1, vertexBuffer3DMem + 1, vertexBuffer3DMemMapped + 1, 30000 * sizeof(Vertex4));
    createIndexBuffer(&physicalDevice, &device, indexBuffer3D, indexBuffer3DMem, indexBuffer3DMemMapped, indices3D, 45000, sizeof(Uint32), true);
    createIndexBuffer(&physicalDevice, &device, indexBuffer3D + 1, indexBuffer3DMem + 1, indexBuffer3DMemMapped + 1, indices3D, 45000, sizeof(Uint32), true);

    createUniformBufferByBuffering(&physicalDevice, &device, &graphicUniformBuffers, &graphicUniformBuffersMemory, &graphicUniformBufferMapped, sizeof(UniformBufferObject));
    createUniformBufferByBuffering(&physicalDevice, &device, &graphic3DUniformBuffers, &graphic3DUniformBuffersMemory, &graphic3DUniformBufferMapped, sizeof(UniformBufferObject));
    createUniformBufferByBuffering(&physicalDevice, &device, &UIUniformBuffers, &UIUniformBuffersMemory, &UIUniformBufferMapped, sizeof(UniformBufferObject));

    createUniformBufferByBuffering(&physicalDevice, &device, &computeUniformBuffers, &computeUniformBuffersmemory, &computeUniformBufferMapped, sizeof(ComputeUniformBufferObject));

    createShaderStorageBuffers(&physicalDevice, &device, &shaderStorageBuffers, &shaderStorageBuffersMem);

    /*unfixed code*/

    //graphic shader
    graphicDescriptorPoolSize = (VkDescriptorPoolSize*)SDL_malloc(2 * sizeof(VkDescriptorPoolSize));
    graphicDescriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    graphicDescriptorPoolSize[0].descriptorCount = 12;
    graphicDescriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    graphicDescriptorPoolSize[1].descriptorCount = 12;
    createDescriptorPool(&device, 2, graphicDescriptorPoolSize, 12, &graphicDescriptorPool);

    PathType graphicTypes[] = {TriangleVertShader, TriangleFragShader};
    VkShaderModule * graphicTempModule = NULL;
    char ** entryName = NULL;
    Uint32 graphicSetCount = CreateShaderModulesAndDescriptorSets(graphicTypes, 2, &graphicTempModule, &graphciShaderStageCreateInfo, &graphicDescriptorSetLayout, &graphicPipelineLayout, &entryName);
    // vertShaderCode = graphicTempModule[0];
    // fragShaderCode = graphicTempModule[1];

    createDescriptorSets(&graphicDescriptorPool, graphicDescriptorSetLayout, graphicSetCount, 4, &graphicDescriptorSets);

    createGraphicsPipeline(&device, &extent2D, 2, graphciShaderStageCreateInfo, &graphicPipelineLayout, &renderPass, &graphicPipeline);

    freeEntryName(2, entryName);

    //3d model shader
    PathType modelTypes[] = {Model3dVertShader, Model3dFragShader};
    VkShaderModule * modelTempModule = NULL;
    entryName = NULL;
    Uint32 modelSetCount = CreateShaderModulesAndDescriptorSets(modelTypes, 2, &modelTempModule, &modelShaderStageCreateInfo, &modelDescriptorSetLayout, &modelPipelineLayout, &entryName);
    // modelVertShaderCode = modelTempModule[0];
    // modelFragShaderCode = modelTempModule[1];

    createDescriptorSets(&graphicDescriptorPool, modelDescriptorSetLayout, modelSetCount, 2, &modelDescriptorSets);

    createModelPipeline(&device, &extent2D, 2, modelShaderStageCreateInfo, &modelPipelineLayout, &renderPass, &modelPipeline);

    freeEntryName(2, entryName);


    //particle shader
    particleDescriptorPoolSize = (VkDescriptorPoolSize*)SDL_malloc(1 * sizeof(VkDescriptorPoolSize));
    particleDescriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    particleDescriptorPoolSize[0].descriptorCount = 2;
    createDescriptorPool(&device, 1, particleDescriptorPoolSize, 2, &particleDescriptorPool);

    PathType particleTypes[] = {ParticleVertShader, ParticleFragShader};
    VkShaderModule * particleTempModule = NULL;
    entryName = NULL;
    Uint32 particleSetCount = CreateShaderModulesAndDescriptorSets(particleTypes, 2, &particleTempModule, &particleShaderStageCreateInfo, &particleDescriptorSetLayout, &particlePipelineLayout, &entryName);
    // particleVertexShaderCode = particleTempModule[0];
    // particleFragmentShaderCode = particleTempModule[1];

    createDescriptorSets(&particleDescriptorPool, particleDescriptorSetLayout, particleSetCount, 1, &particleDescriptorSets);

    createParticlePipeline(&device, &extent2D, 2, particleShaderStageCreateInfo, &particlePipelineLayout, &renderPass, &particlePipeline);
    freeEntryName(2, entryName);

    //compute shader
    computeDescriptorPoolSize = (VkDescriptorPoolSize*)SDL_malloc(2 * sizeof(VkDescriptorPoolSize));
    computeDescriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    computeDescriptorPoolSize[0].descriptorCount = 2;
    computeDescriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeDescriptorPoolSize[1].descriptorCount = 4;
    createDescriptorPool(&device, 2, computeDescriptorPoolSize, 2, &computeDescriptorPool);

    PathType computeTypes[] = {ParticleCompShader};
    VkShaderModule * computeTempModule = NULL;
    entryName = NULL;
    Uint32 computeSetCount = CreateShaderModulesAndDescriptorSets(computeTypes, 1, &computeTempModule, &computeShaderStageCreateInfo, &computeDescriptorSetLayout, &computePipelineLayout, &entryName);
    // compShaderCode = computeTempModule[0];
    
    createDescriptorSets(&computeDescriptorPool, computeDescriptorSetLayout, computeSetCount, 1, &computeDescriptorSets);

    createComputePipeline(&device, &computePipelineLayout, computeShaderStageCreateInfo, &computePipeline);
    freeEntryName(1, entryName);

    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &graphicCommandPool, &graphicCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &presentCommandPool, &presentCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &computeCommandPool, &computeCommandBuffer);
    createCommandbufferByBuffering(VK_COMMAND_BUFFER_LEVEL_PRIMARY, &transferCommandPool, &transferCommandBuffer);

    createSemaphoreByBuffering(&imageAvailableSemaphore);
    createSemaphoreByBuffering(&renderFinishedSemaphore);
 
    createFenceByBuffering(&graphicInFlightFence);

    createSemaphoreByBuffering(&computeFinishedSemaphore);

    createFenceByBuffering(&computeInFlightFences);
    
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
    loadStaticModel(&staticModelPool, 10, BoxObj, BoxPng, vertices3D, &vertices3DCount, indices3D, &indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_MODEL, modelDescriptorSets);
    loadStaticModel(&staticModelPool, 10, BottomObj, BottomPng, vertices3D, &vertices3DCount, indices3D, &indices3DCount, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_BOTTOM, modelDescriptorSets + 2);

    loadTexture(Loading1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_LOADING, graphicDescriptorSets);
    loadTexture(CirclePng, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_CIRCLE, graphicDescriptorSets + 2);
    loadTexture(MainFontPng, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, TEXTURE_FONT, graphicDescriptorSets + 4);
    
    G_Texture_P * loadingTexture = getTexture(TEXTURE_LOADING);
    G_Texture_P * circleTexture = getTexture(TEXTURE_CIRCLE);
    G_Texture_P * fontTexture = getTexture(TEXTURE_FONT);
    G_Texture_P * tileSetTexture = getTexture(TEXTURE_TILE_SET);
    G_Texture_P * modelTexture = getTexture(TEXTURE_MODEL);
    G_Texture_P * bottomTexture = getTexture(TEXTURE_BOTTOM);

    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, loadingTexture->pDescriptorSet, UIUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, circleTexture->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, fontTexture->pDescriptorSet, UIUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, tileSetTexture->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, modelTexture->pDescriptorSet, graphic3DUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, bottomTexture->pDescriptorSet, graphic3DUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_LOADING, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_CIRCLE, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_FONT, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_TILE_SET, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_MODEL, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, TEXTURE_BOTTOM, textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, particleDescriptorSets, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, computeDescriptorSets, computeUniformBuffers, 0, sizeof(ComputeUniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, computeDescriptorSets, shaderStorageBuffers, 0, sizeof(Particle) * PARTICLE_COUNT);
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, computeDescriptorSets, shaderStorageBuffers, 0, sizeof(Particle) * PARTICLE_COUNT);

    executeUpdateDescriptorSets();

    // initializeRecreate();

    resultVulkan(VK_SUCCESS, initializedF, 0);
}

void cleanVulkan(FuncCode code)
{
    logMessage("\nclean up");

    vkDeviceWaitIdle(device);

    switch (code)
    {
        case FuncCodeMax:
        case recreateSwapchainF:
        case queuePresentF:
        case queueSumbitF:
        case endCommandBufferF:
        case resetCommandBufferF:
        case acquireNextImageF:
        case resetFencesF:
        case waitForFencesF:
        case drawFrameF:
        case recordCommandBufferF:
        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyFence(device, computeInFlightFences[i], allInOne.pAllocationCallbacks);
        }
        logMessage("compute in flight fences destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, computeFinishedSemaphore[i], allInOne.pAllocationCallbacks);
        }
        logMessage("compute finished semaphore destroyed");
        /*fall through*/

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyFence(device, graphicInFlightFence[i], allInOne.pAllocationCallbacks);
        }
        logMessage("in flight fence destroyed");
        /*fall through*/

        case createFenceF:
        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphore[i], allInOne.pAllocationCallbacks);
        }
        logMessage("render finished semaphore destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, imageAvailableSemaphore[i], allInOne.pAllocationCallbacks);
        }
        logMessage("image available semaphore destroyed");
        /*fall through*/

        case createSemaphoreF:
        // SDL_free(graphicCommandBuffer);
        // logMessage("command buffer freed");

        // SDL_free(computeCommandBuffer);
        // logMessage("compute command buffer freed");

        // SDL_free(presentCommandBuffer);
        // logMessage("command buffer freed");

        // SDL_free(transferCommandBuffer);
        // logMessage("compute command buffer freed");
        // /*fall through*/
        case createCommandbufferByBufferingF:
        vkDestroyDescriptorPool(device, computeDescriptorPool, allInOne.pAllocationCallbacks);
        SDL_free(computeDescriptorPoolSize);
        logMessage("compute descriptro pool destroyed");

        vkDestroyDescriptorPool(device, particleDescriptorPool, allInOne.pAllocationCallbacks);
        SDL_free(particleDescriptorPoolSize);
        logMessage("particle descriptor pool destroyed");

        vkDestroyDescriptorPool(device, graphicDescriptorPool, allInOne.pAllocationCallbacks);
        SDL_free(graphicDescriptorPoolSize);
        logMessage("graphic descriptor pool destroyed");
        /*fall through*/

        case createDescriptorPoolF:
        vkDestroyPipeline(device, computePipeline, allInOne.pAllocationCallbacks);
        logMessage("compute pipeline destroyed");

        vkDestroyPipeline(device, particlePipeline, allInOne.pAllocationCallbacks);
        logMessage("particle pipeline destroyed");

        vkDestroyPipeline(device, modelPipeline, allInOne.pAllocationCallbacks);
        logMessage("model pipelne destroyed");

        vkDestroyPipeline(device, graphicPipeline, allInOne.pAllocationCallbacks);
        logMessage("graphic pipelne destroyed");
        /*fall through*/

        case createGraphicsPipelineF:
        vkDestroyPipelineLayout(device, computePipelineLayout, allInOne.pAllocationCallbacks);
        logMessage("compute pipeline layout destroyed");

        vkDestroyPipelineLayout(device, particlePipelineLayout, allInOne.pAllocationCallbacks);
        logMessage("particle pipeline layout destroyed");

        vkDestroyPipelineLayout(device, modelPipelineLayout, allInOne.pAllocationCallbacks);
        logMessage("model pipeline layout destroyed");

        vkDestroyPipelineLayout(device, graphicPipelineLayout, allInOne.pAllocationCallbacks);
        logMessage("graphic pipeline layout destroyed");
        /*fall through*/

        case createPipelineLayoutF:
        vkDestroyDescriptorSetLayout(device, *computeDescriptorSetLayout, allInOne.pAllocationCallbacks);
        SDL_free(computeDescriptorSetLayout);
        SDL_free(computeBindings);
        logMessage("compute descriptor set layout destroyed");

        vkDestroyDescriptorSetLayout(device, *particleDescriptorSetLayout, allInOne.pAllocationCallbacks);
        SDL_free(particleDescriptorSetLayout);
        SDL_free(particleBindings);
        logMessage("particle descriptoe set layout destroyed");

        vkDestroyDescriptorSetLayout(device, *modelDescriptorSetLayout, allInOne.pAllocationCallbacks);
        SDL_free(modelDescriptorSetLayout);
        logMessage("graphic descriptor set layout destroyed");
 
        vkDestroyDescriptorSetLayout(device, *graphicDescriptorSetLayout, allInOne.pAllocationCallbacks);
        SDL_free(graphicDescriptorSetLayout);
        SDL_free(graphicBindings);
        logMessage("graphic descriptor set layout destroyed");
        /*fall through*/

        case createDescriptorSetLayoutF:
        SDL_free(computeShaderStageCreateInfo);
        SDL_free(particleShaderStageCreateInfo);
        SDL_free(modelShaderStageCreateInfo);
        SDL_free(graphciShaderStageCreateInfo);

        VkShaderModule tempModule;
        do
        {
            tempModule = NULL;
            allInOne.shaderModuleStack.popFn(&allInOne.shaderModuleStack, &tempModule);
            vkDestroyShaderModule(device, tempModule, allInOne.pAllocationCallbacks);
        } while (tempModule != NULL);

        logMessage("shaderCode destroyed");
        /*fall through*/

        case createShaderModuleF:
        destroyBufferByBuffering(graphicUniformBuffers, graphicUniformBuffersMemory);
        logMessage("graphic uniform buffer destroyed");
        logMessage("graphic uniform buffer memory freed");

        destroyBufferByBuffering(graphic3DUniformBuffers, graphic3DUniformBuffersMemory);
        logMessage("graphic 3D uniform buffer destroyed");
        logMessage("graphic 3D uniform buffer memory freed");

        destroyBufferByBuffering(UIUniformBuffers, UIUniformBuffersMemory);
        logMessage("graphic 3D uniform buffer destroyed");
        logMessage("graphic 3D uniform buffer memory freed");

        destroyBufferByBuffering(computeUniformBuffers, computeUniformBuffersmemory);
        logMessage("compute uniform buffers destroyed");
        logMessage("compute uniform buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyBuffer(device, shaderStorageBuffers[i], allInOne.pAllocationCallbacks);
        }
        logMessage("shader storage buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, shaderStorageBuffersMem[i], allInOne.pAllocationCallbacks);
        }
        logMessage("shader storage buffer memory freed");
        /*fall through*/

        case createUniformBuffersF:
        vkDestroyBuffer(device, indexBuffer2D[0], allInOne.pAllocationCallbacks);
        logMessage("index buffer destroyed");

        vkFreeMemory(device, indexBuffer2DMem[0], allInOne.pAllocationCallbacks);
        logMessage("index buffer memory freed");

        SDL_free(indices3D);
        destroyBufferByBuffering(indexBuffer3D, indexBuffer3DMem);
        logMessage("index buffer destroyed");
        logMessage("index buffer memory freed");
        /*fall through*/

        case createIndexBufferF:
        SDL_free(vertices3D);
        destroyBufferByBuffering(vertexBuffer3D, vertexBuffer3DMem);
        logMessage("vertex buffer destroyed");
        logMessage("vertex buffer memory freed");

        destroyStaticModelPool(&staticModelPool);
        logMessage("static model pool destroyed");
 
        SDL_free(vertices2D);
        destroyBufferByBuffering(vertexBuffer2D, vertexBuffer2DMem);
        logMessage("vertex buffer destroyed");
        logMessage("vertex buffer memory freed");
        /*fall through*/

        case createVertexBufferF:
        vkDestroySampler(device, textureSampler, allInOne.pAllocationCallbacks);
        logMessage("texture sampler detroyed");
        /*fall through*/

        case createTextureSamplerF:
        unloadAllTexture();
        /*fall through*/

        case createTextureImageViewF:
        case createTextureImageF:
        destroyedFrameBuffer(imageCount2D, swapchain2DFramebuffer);
        SDL_free(swapchain2DFramebuffer);
        
        destroyedFrameBuffer(imageCount3D, swapchain3DFramebuffer);
        SDL_free(swapchain3DFramebuffer);
        logMessage("framebuffer destroyed");
        /*fall through*/

        case createFrameBufferF:
        // vkDestroyImageView(device, depthImageView, allInOne.pAllocationCallbacks);
        // logMessage("depth image view destroyed");

        // vkDestroyImage(device, depthImage, allInOne.pAllocationCallbacks);
        // logMessage("depth image destroyed");

        // vkFreeMemory(device, depthImageMemory, allInOne.pAllocationCallbacks);
        // logMessage("depth image memory freed");
        /*fall through*/

        case createDepthResouresF:
        vkDestroyCommandPool(device, graphicCommandPool, allInOne.pAllocationCallbacks);
        logMessage("commandpool destroyed");
        
        vkDestroyCommandPool(device, computeCommandPool, allInOne.pAllocationCallbacks);
        logMessage("compute commandpool destroyed");

        vkDestroyCommandPool(device, presentCommandPool, allInOne.pAllocationCallbacks);
        logMessage("commandpool destroyed");
        
        vkDestroyCommandPool(device, transferCommandPool, allInOne.pAllocationCallbacks);
        logMessage("compute commandpool destroyed");
        /*fall through*/

        case createCommandPoolF:
        vkDestroyRenderPass(device, renderPass, allInOne.pAllocationCallbacks);
        logMessage("renderPass destroyed");
        /*fall through*/

        case createGraphicRenderPassF:
        destroyImageViews(swapchain2DImageViews, imageCount2D);
        SDL_free(swapchain2DImageViews);
        
        destroyImageViews(swapchain3DImageViews, imageCount3D);
        SDL_free(swapchain3DImageViews);
        logMessage("swapchain2D image views destroyed");
        /*fall through*/

        case createSwapchainImageViewsF:
        SDL_free(swapchain2DImages);
        
        SDL_free(swapchain3DImages);
        logMessage("swapchainImages freed");
        /*fall through*/

        case createSwapchainImageF:
        case getSwapchainNumberF:
        vkDestroySwapchainKHR(device, swapchain2D, allInOne.pAllocationCallbacks);
        
        vkDestroySwapchainKHR(device, swapchain3D, allInOne.pAllocationCallbacks);
        logMessage("swapchain2D destroyed");
        /*fall through*/

        case createSwapchainF:
        vkDestroyDevice(device, allInOne.pAllocationCallbacks);
        logMessage("device destroyed");
        /*fall through*/

        case getSurfaceCapabilitiesF:
        case getPresentModesF:
        case getSurfaceFormatsF:
        case createLogicalDeviceF:
        case findQueueFamiliesF:
        case pickPhysicalDeviceF:
        vkDestroySurfaceKHR(instance, surface2D, allInOne.pAllocationCallbacks);
        
        vkDestroySurfaceKHR(instance, surface3D, allInOne.pAllocationCallbacks);
        logMessage("surface destroyed");
        /*fall through*/

        case createSurfaceF:
        vkDestroyInstance(instance, allInOne.pAllocationCallbacks);
        logMessage("instance destroyed");
        /*fall through*/

        case createInstanceF:
        case vulkanVersionF:
        SDL_DestroyWindow(window_2D);
        SDL_DestroyWindow(window_3D);
        logMessage("window_2D destroyed");
        break;

        default:
        break;
    }
}
    /*vertices2D[4] = (Vertex){{-0.0f, -0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices2D[5] = (Vertex){{1.0f, -0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices2D[6] = (Vertex){{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices2D[7] = (Vertex){{-0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};*/