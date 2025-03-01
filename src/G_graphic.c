#include "G_constants.h"
#include "G_log.h"
#include "G_graphic.h"
#include "G_resource.h"

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
#include "vk_code_h/vk_index.h"
#include "vk_code_h/vk_vertex.h"
#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_depth.h"
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
#include "vk_code_h/vk_judge.h"

#include "spirv_reflect/shader_resolve.h"

//declare a sdl window
SDL_Window * window = NULL;
SDL_DisplayID displayId = 0;

//window's width and height
uint32_t width = 800;
uint32_t height = 600;

float physicalCoffectX = 1.0f;
float physicalCoffectY = 1.0f;

bool initWindow(void)
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
    //create sdl window and sign window as a vulkan window
    window = SDL_CreateWindow("Vulkan", width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (window == NULL)
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

    logMessage("window initialized");

    uint32_t iconWidth, iconHeight;
    iconWidth = iconHeight = 0;         
    uint8_t iconChannel;
    void * iconPixels = readPNG(IconPng, &iconWidth, &iconHeight, &iconChannel);
    SDL_Surface * iconSurface = SDL_CreateSurfaceFrom(iconWidth, iconHeight, SDL_PIXELFORMAT_RGBA32, iconPixels, iconWidth * iconChannel);
    if (iconSurface == NULL)
    {
        SDL_free(iconPixels);
        return false;
    }

    if (!SDL_SetWindowIcon(window, iconSurface))
        return false;

    SDL_DestroySurface(iconSurface);
    SDL_free(iconPixels);
    //glfwTerminate();

    return true;
}

static VkInstance instance = NULL;

static VkSurfaceKHR surface = NULL;

static VkPhysicalDevice physicalDevice = NULL;

static QueueFamilyIndices queueIndices = {};
static VkDevice device = NULL;
static VkQueue graphicQueue = NULL;
static VkQueue presentQueue = NULL;
static VkQueue computeQueue = NULL;
static VkQueue transferQueue = NULL;

static VkAllocationCallbacks SDL_allocationCallBacks = {};

static VkSurfaceFormatKHR surfaceFormat = {};
static VkPresentModeKHR presentMode = 0;
static VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
static VkExtent2D extent2D = {};
static VkExtent2D oldExtent2D = {};
static VkSwapchainKHR swapchain = NULL;

static uint32_t imageCount = 0;
static VkFormat swapchainFormat = 0;

static VkImage * swapchainImages = NULL;
static VkImageView * swapchainImageViews = NULL;

static VkShaderModule vertShaderCode = NULL;
static VkShaderModule fragShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * graphciShaderStageCreateInfo = NULL;

static VkShaderModule particleVertexShaderCode = NULL;
static VkShaderModule particleFragmentShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * particleShaderStageCreateInfo = NULL;

static VkShaderModule compShaderCode = NULL;
static VkPipelineShaderStageCreateInfo * computeShaderStageCreateInfo = NULL;

static VkDescriptorSetLayout * graphicDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * particleDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * computeDescriptorSetLayout = NULL;

// static uint32_t graphicBinding = 0;
static VkDescriptorSetLayoutBinding * graphicBindings = NULL;
static VkPipelineLayout graphicPipelineLayout = NULL;

static VkDescriptorSetLayoutBinding * particleBindings = NULL;
static VkPipelineLayout particlePipelineLayout = NULL;

// static uint32_t computeBinding = 0;
static VkDescriptorSetLayoutBinding * computeBindings = NULL;
static VkPipelineLayout computePipelineLayout = NULL;

static VkRenderPass renderPass = NULL;

static VkPipeline graphicPipeline = NULL;

static VkPipeline particlePipeline = NULL;

static VkPipeline computePipeline = NULL;

static VkCommandPool graphicCommandPool = NULL;

static VkCommandPool presentCommandPool = NULL;

static VkCommandPool transferCommandPool = NULL;

static VkCommandPool computeCommandPool = NULL;

// static VkImage depthImage = NULL;
// static VkDeviceMemory depthImageMemory = NULL;
// static VkImageView depthImageView = NULL;
// static VkFormat depthFormat = 0;

static VkFramebuffer * swapchainFramebuffer = NULL;

// //circle.png
// static VkImage texturesImage = NULL;
// static VkDeviceMemory textureImageMem = NULL;
// static VkImageView textureImageView = NULL;

// //loading1.png
// static VkImage loadingImage = NULL;
// static VkDeviceMemory loadingImageMem = NULL;
// static VkImageView loadingImageView = NULL;

// //MainFont.png
// static VkImage textImage = NULL;
// static VkDeviceMemory textImageMem = NULL;
// static VkImageView textImageView = NULL;

static VkSampler textureSampler = NULL;

static VkBuffer vertexBuffer[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory vertexBufferMem[MAX_FRAMES_IN_FLIGHT];
static void * vertexBufferMemMapped[MAX_FRAMES_IN_FLIGHT];
static uint32_t verticesCount = 0;

//three vertices of a triangle and color
static Vertex * vertices = NULL;
// static vec3 * vertices_Pos = NULL;
// static vec3 * vertices_Color = NULL;
// static vec2 * vertices_TexCoord = NULL;

static VkBuffer indexBuffer[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory indexBufferMem[MAX_FRAMES_IN_FLIGHT];
static void * indexBufferMemMapped [MAX_FRAMES_IN_FLIGHT];
static uint16_t * indices_v = NULL;

static VkBuffer graphicUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory graphicUniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
static void* graphicUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static UniformBufferObject ubo = {};

static VkDescriptorPoolSize * graphicDescriptorPoolSize = NULL;

static VkBuffer computeUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory computeUniformBuffersmemory[MAX_FRAMES_IN_FLIGHT];
static void* computeUniformBufferMapped[MAX_FRAMES_IN_FLIGHT];
static ComputeUniformBufferObject computeUbo = {};

static VkDescriptorPoolSize * particleDescriptorPoolSize = NULL;

static VkDescriptorPoolSize * computeDescriptorPoolSize = NULL;

static VkDescriptorPool graphicDescriptorPool = NULL;
static VkDescriptorSet * graphicDescriptorSets = NULL;

static VkDescriptorPool particleDescriptorPool = NULL;
static VkDescriptorSet * particleDescriptorSets = NULL;

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

static uint32_t currentFrame = 0;

static float camera_X = 0.0f;
static float camera_Y = 0.0f;

static float pictureX = 0;
static float pictureY = 0;

// static bool moveEnabled = false;

static VkBuffer shaderStorageBuffers[MAX_FRAMES_IN_FLIGHT];
static VkDeviceMemory shaderStorageBuffersMem[MAX_FRAMES_IN_FLIGHT];

static ImageRotate pictureImageRotate = {0.0f};

//store all compoents for initialize vulkan in a struct
VK_ALL allInOne = {};

static void initializeAllInOne(void)
{
    allInOne.pAllocationCallbacks = &SDL_allocationCallBacks;

    allInOne.pInstance = &instance;

    allInOne.pPhysicalDevice = &physicalDevice;

    allInOne.pDevice = &device;

    allInOne.pSurfaceCapabilities = &surfaceCapabilities;
    allInOne.pSurfaceFormat = &surfaceFormat;
    allInOne.pPresentMode = &presentMode;

    allInOne.pSurface = &surface;

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
    allInOne.pImageCount = &imageCount;

    allInOne.pSwapchain = &swapchain;

    allInOne.pGraphicPipelineLayout = &graphicPipelineLayout;

    allInOne.pParticlePipelineLayout = &particlePipelineLayout;

    allInOne.pComputePipelineLayout = &computePipelineLayout;

    allInOne.pRenderPass = &renderPass;

    allInOne.pGraphicPipeline = &graphicPipeline;

    allInOne.pParticlePipeline = &particlePipeline;

    allInOne.pComputePipeline = &computePipeline;

    allInOne.ppSwapchainImages = &swapchainImages;
    allInOne.ppSwapchainImageViews = &swapchainImageViews;
    allInOne.ppSwapchainFramebuffer = &swapchainFramebuffer;
    
    // allInOne.pDepthFormat = &depthFormat;
    // allInOne.pDepthImage = &depthImage;
    // allInOne.pDepthImageView = &depthImageView;
    // allInOne.pDepthImageMem = &depthImageMemory;

    allInOne.pVertexBuffer = &vertexBuffer;
    allInOne.maxVerticesCount = (BALLCOUNT + MAX_CHARACTERS) * 4 * 2;
    allInOne.ppVertices = &vertices;
    // allInOne.ppVertices_Pos = &vertices_Pos;
    // allInOne.ppVertices_Color = &vertices_Color;
    // allInOne.ppVertices_TexCoord = &vertices_TexCoord;
    allInOne.pVerticesCount = &verticesCount;
    allInOne.pVertexBufferMem = &vertexBufferMem;
    allInOne.ppVertexBufferMemMapped = &vertexBufferMemMapped;

    allInOne.pIndexBuffer = &indexBuffer;
    allInOne.ppIndices = &indices_v;
    allInOne.pIndexBufferMem = &indexBufferMem;
    allInOne.ppIndexBufferMemMapped = &indexBufferMemMapped;

    allInOne.pTextureSampler = &textureSampler;

    allInOne.ppGraphicUniformBuffer = &graphicUniformBuffers;
    allInOne.pppGraphicUniformBufferMapped = &graphicUniformBufferMapped;

    allInOne.pGraphicUbo = &ubo;

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

    allInOne.pImageRotate = &pictureImageRotate;
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

    createSurface(window);

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

    getSurfaceFormats();
    getPresentModes();
    getSurfaceCapabilities();
    
    extent2D.width = width;
    extent2D.height = height;

    createSwapchain();

    getSwapchainNumber();
    createSwapchainImage();

    swapchainFormat = surfaceFormat.format;

    //swapchain image view
    createSwapchainImageView(VK_IMAGE_ASPECT_COLOR_BIT);

    loadDepthResource("depth");
    G_Texture_P const * depthTexutre = getTexture("depth");
    createRenderPass(swapchainFormat, depthTexutre->format);

    // createDepthResoures(&depthImage, &depthImageMemory, &depthImageView);

    createFrameBuffer(imageCount, swapchainImageViews, &depthTexutre->imageView, &renderPass, &swapchainFramebuffer);
    
    createTextureSampler(&physicalDevice, &device, &textureSampler);

    // vertices = SDL_calloc((BALLCOUNT * 4 + MAX_CHARACTERS * 4) * 2, sizeof(vec3) + sizeof(vec3) + sizeof(vec2));
    vertices = (Vertex*)SDL_calloc((BALLCOUNT * 4 + MAX_CHARACTERS * 4) * 2, sizeof(Vertex));
    allInOne.maxVerticesCount = (BALLCOUNT + MAX_CHARACTERS) * 4 * 2;
    // vertices_Pos = (vec3 *)vertices;
    // vertices_Color = (vec3 *)(vertices + allInOne.maxVerticesCount * sizeof(vec3));
    // vertices_TexCoord = (vec2 *)(vertices + allInOne.maxVerticesCount * (sizeof(vec3) + sizeof(vec3)));

    // vertices_Pos[0] = (vec3){-0.5f, -0.5f, 0.0f};
    // vertices_Pos[1] = (vec3){0.5f, -0.5f, 0.0f};
    // vertices_Pos[2] = (vec3){0.5f, 0.5f, 0.0f};
    // vertices_Pos[3] = (vec3){-0.5f, 0.5f, 0.0f};

    // vertices_Color[0] = (vec3){1.0f, 0.0f, 0.0f};
    // vertices_Color[1] = (vec3){0.0f, 1.0f, 0.0f};
    // vertices_Color[2] = (vec3){0.0f, 0.0f, 1.0f};
    // vertices_Color[3] = (vec3){1.0f, 1.0f, 1.0f};

    // vertices_TexCoord[0] = (vec2){0.0f, 1.0f};
    // vertices_TexCoord[1] = (vec2){1.0f, 1.0f};
    // vertices_TexCoord[2] = (vec2){1.0f, 0.0f};
    // vertices_TexCoord[3] = (vec2){0.0f, 0.0f};

    /*vertices[4] = (Vertex){{-0.0f, -0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[5] = (Vertex){{1.0f, -0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[6] = (Vertex){{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[7] = (Vertex){{-0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};*/

    //positionInitialize(-24, -24, 16, 16, extent2D, &vertices, 1);

    createVertexBuffer(&physicalDevice, &device, vertexBuffer, vertexBufferMem, vertexBufferMemMapped, vertices, (BALLCOUNT * 4 + MAX_CHARACTERS * 4) * 2);
    createVertexBuffer(&physicalDevice, &device, vertexBuffer + 1, vertexBufferMem + 1, vertexBufferMemMapped + 1, vertices, (BALLCOUNT * 4 + MAX_CHARACTERS * 4) * 2);

    indices_v = (uint16_t *)SDL_calloc(BALLCOUNT * 6 + MAX_CHARACTERS * 6, sizeof(uint16_t));
    indexInitialize(indices_v, BALLCOUNT + MAX_CHARACTERS);

    createIndexBuffer(&physicalDevice, &device, indexBuffer, indexBufferMem, indexBufferMemMapped, indices_v, BALLCOUNT * 6 + MAX_CHARACTERS * 6);
    createIndexBuffer(&physicalDevice, &device, indexBuffer + 1, indexBufferMem + 1, indexBufferMemMapped + 1, indices_v, BALLCOUNT * 6 + MAX_CHARACTERS * 6);

    SDL_free(indices_v);

    // for (int i = 0;i < MAX_CHARACTERS;i++)
    // {
    //     vertexInitialize(-300 + i * 24, -100, 24, 24, 0.1f, true, i + BALLCOUNT, allInOne.ppVertices_Pos, allInOne.ppVertices_Color, allInOne.ppVertices_TexCoord);
    // }

    // initializeMovingBuffer(&physicalDevice, &device, &graphicCommandPool, &graphicQueue, &movingStagingBuffer, &movingStagingMemory, &movingBufferMapped, vertices, verticesCount);

    createUniformBufferByBuffering(&physicalDevice, &device, &graphicUniformBuffers, &graphicUniformBuffersMemory, &graphicUniformBufferMapped, sizeof(UniformBufferObject));

    createUniformBufferByBuffering(&physicalDevice, &device, &computeUniformBuffers, &computeUniformBuffersmemory, &computeUniformBufferMapped, sizeof(ComputeUniformBufferObject));

    createShaderStorageBuffers(&physicalDevice, &device, &shaderStorageBuffers, &shaderStorageBuffersMem);

    /*unfixed code*/

    //graphic shader
    graphicDescriptorPoolSize = (VkDescriptorPoolSize*)SDL_malloc(2 * sizeof(VkDescriptorPoolSize));
    graphicDescriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    graphicDescriptorPoolSize[0].descriptorCount = 6;
    graphicDescriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    graphicDescriptorPoolSize[1].descriptorCount = 18;
    createDescriptorPool(&device, 2, graphicDescriptorPoolSize, 6, &graphicDescriptorPool);

    PathType graphicTypes[] = {TriangleVertShader, TriangleFragShader};
    VkShaderModule * graphicTempModule = NULL;
    char ** entryName = NULL;
    Uint32 graphicSetCount = CreateShaderModulesAndDescriptorSets(graphicTypes, 2, &graphicTempModule, &graphciShaderStageCreateInfo, &graphicDescriptorSetLayout, &graphicPipelineLayout, &entryName);
    vertShaderCode = graphicTempModule[0];
    fragShaderCode = graphicTempModule[1];

    createDescriptorSets(&graphicDescriptorPool, graphicDescriptorSetLayout, graphicSetCount, 3, &graphicDescriptorSets);

    createGraphicsPipeline(&device, &extent2D, 2, graphciShaderStageCreateInfo, &graphicPipelineLayout, &renderPass, &graphicPipeline);

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
    particleVertexShaderCode = particleTempModule[0];
    particleFragmentShaderCode = particleTempModule[1];

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
    compShaderCode = computeTempModule[0];
    
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
    
    loadTexture(Loading1Png, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, "loading", graphicDescriptorSets);
    loadTexture(CirclePng, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, "circle", graphicDescriptorSets + 2);
    loadTexture(MainFontPng, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, "font", graphicDescriptorSets + 4);
    
    G_Texture_P const * loadingTexture = getTexture("loading");
    G_Texture_P const * circleTexture = getTexture("circle");
    G_Texture_P const * fontTexture = getTexture("font");

    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, loadingTexture->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, circleTexture->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, fontTexture->pDescriptorSet, graphicUniformBuffers, 0, sizeof(UniformBufferObject));
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, "loading", textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, "circle", textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    addDescriptorUpdate_Texture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, "font", textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

        vkDestroyPipeline(device, graphicPipeline, allInOne.pAllocationCallbacks);
        logMessage("graphic pipelne destroyed");
        /*fall through*/

        case createGraphicsPipelineF:
        vkDestroyPipelineLayout(device, computePipelineLayout, allInOne.pAllocationCallbacks);
        logMessage("compute pipeline layout destroyed");

        vkDestroyPipelineLayout(device, particlePipelineLayout, allInOne.pAllocationCallbacks);
        logMessage("particle pipeline layout destroyed");

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

        vkDestroyDescriptorSetLayout(device, *graphicDescriptorSetLayout, allInOne.pAllocationCallbacks);
        SDL_free(graphicDescriptorSetLayout);
        SDL_free(graphicBindings);
        logMessage("graphic descriptor set layout destroyed");
        /*fall through*/

        case createDescriptorSetLayoutF:
        vkDestroyShaderModule(device, compShaderCode, allInOne.pAllocationCallbacks);
        SDL_free(computeShaderStageCreateInfo);
        logMessage("compute shader stage create info destroyed");

        vkDestroyShaderModule(device, particleFragmentShaderCode, allInOne.pAllocationCallbacks);
        vkDestroyShaderModule(device, particleVertexShaderCode, allInOne.pAllocationCallbacks);
        SDL_free(particleShaderStageCreateInfo);
        logMessage("particle shader stage create info destroyed");

        vkDestroyShaderModule(device, fragShaderCode, allInOne.pAllocationCallbacks);
        vkDestroyShaderModule(device, vertShaderCode, allInOne.pAllocationCallbacks);
        SDL_free(graphciShaderStageCreateInfo);
        logMessage("shaderCode destroyed");
        /*fall through*/

        case createShaderModuleF:
        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, graphicUniformBuffersMemory[i]);
            vkDestroyBuffer(device, graphicUniformBuffers[i], allInOne.pAllocationCallbacks);
        }
        logMessage("graphic uniform buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, graphicUniformBuffersMemory[i], allInOne.pAllocationCallbacks);
        }
        logMessage("graphic uniform buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, computeUniformBuffersmemory[i]);
            vkDestroyBuffer(device, computeUniformBuffers[i], allInOne.pAllocationCallbacks);
        }
        logMessage("compute uniform buffers destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, computeUniformBuffersmemory[i], allInOne.pAllocationCallbacks);
        }
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
        vkDestroyBuffer(device, indexBuffer[0], allInOne.pAllocationCallbacks);
        vkDestroyBuffer(device, indexBuffer[1], allInOne.pAllocationCallbacks);
        logMessage("index buffer destroyed");

        vkFreeMemory(device, indexBufferMem[0], allInOne.pAllocationCallbacks);
        vkFreeMemory(device, indexBufferMem[1], allInOne.pAllocationCallbacks);
        logMessage("index buffer memory freed");
        /*fall through*/

        case createIndexBufferF:
        SDL_free(vertices);
        
        vkUnmapMemory(device, vertexBufferMem[1]);
        vkUnmapMemory(device, vertexBufferMem[0]);

        vkDestroyBuffer(device, vertexBuffer[1], allInOne.pAllocationCallbacks);
        vkDestroyBuffer(device, vertexBuffer[0], allInOne.pAllocationCallbacks);
        logMessage("vertex buffer destroyed");

        vkFreeMemory(device, vertexBufferMem[0], allInOne.pAllocationCallbacks);
        vkFreeMemory(device, vertexBufferMem[1], allInOne.pAllocationCallbacks);
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
        destroyedFrameBuffer(imageCount, swapchainFramebuffer);
        SDL_free(swapchainFramebuffer);
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

        case createRenderPassF:
        destroyImageViews(swapchainImageViews, imageCount);
        SDL_free(swapchainImageViews);
        logMessage("swapchain image views destroyed");
        /*fall through*/

        case createSwapchainImageViewsF:
        SDL_free(swapchainImages);
        logMessage("swapchainImages freed");
        /*fall through*/

        case createSwapchainImageF:
        case getSwapchainNumberF:
        vkDestroySwapchainKHR(device, swapchain, allInOne.pAllocationCallbacks);
        logMessage("swapchain destroyed");
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
        vkDestroySurfaceKHR(instance, surface, allInOne.pAllocationCallbacks);
        logMessage("surface destroyed");
        /*fall through*/

        case createSurfaceF:
        vkDestroyInstance(instance, allInOne.pAllocationCallbacks);
        logMessage("instance destroyed");
        /*fall through*/

        case createInstanceF:
        case vulkanVersionF:
        SDL_DestroyWindow(window);
        logMessage("window destroyed");
        break;

        default:
        break;
    }
}