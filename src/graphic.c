#include "vulkan_code.h"
#include "graphic.h"

//declare a sdl window
SDL_Window * window = NULL;
SDL_DisplayID displayId = 0;

//window's width and height
uint32_t width = 800;
uint32_t height = 600;

const uint32_t logical_width = 800;
const uint32_t logical_height = 600;

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

    physicalCoffectX = (float)width / logical_width;
    physicalCoffectY = (float)height / logical_height;

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

static QueueFamilyIndices indices = {};
static VkDevice device = NULL;
static VkQueue graphicQueue = NULL;
static VkQueue presentQueue = NULL;

static VkQueue computeQueue = NULL;

static VkAllocationCallbacks SDL_allocationCallBacks = {};
static VmaAllocator vmaAllocator = NULL;

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
static uint32_t graphicShaderCount = 0;
static VkPipelineShaderStageCreateInfo * graphciShaderStageCreateInfo = NULL;

static VkShaderModule particleVertexShaderCode = NULL;
static VkShaderModule particleFragmentShaderCode = NULL;
static uint32_t particleShaderCount = 0;
static VkPipelineShaderStageCreateInfo * particleShaderStageCreateInfo = NULL;

static VkShaderModule compShaderCode = NULL;
static uint32_t computeShaderCount = 0;
static VkPipelineShaderStageCreateInfo * computeShaderStageCreateInfo = NULL;

static VkDescriptorSetLayout * graphicDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * particleDescriptorSetLayout = NULL;

static VkDescriptorSetLayout * computeDescriptorSetLayout = NULL;

// static uint32_t graphicBinding = 0;
static VkDescriptorSetLayoutBinding * graphicBindings = NULL;
static uint32_t graphicBindingCount = 0;
static VkPushConstantRange pushConstantRange = {};
static VkPipelineLayout graphicPipelineLayout = NULL;

static VkDescriptorSetLayoutBinding * particleBindings = NULL;
static uint32_t particleBindingCount = 0;
static VkPipelineLayout particlePipelineLayout = NULL;

// static uint32_t computeBinding = 0;
static VkDescriptorSetLayoutBinding * computeBindings = NULL;
static uint32_t computeBindingCount = 0;
static VkPipelineLayout computePipelineLayout = NULL;

// static VkSubpassDependency dependency = {};
static VkRenderPass renderPass = NULL;

static VkPipeline graphicPipeline = NULL;

static VkPipeline particlePipeline = NULL;

static VkPipeline computePipeline = NULL;

static VkCommandPool swapchainCommandPool = NULL;

static VkCommandPool computeCommandPool = NULL;

static VkImage depthImage = NULL;
static VkDeviceMemory depthImageMemory = NULL;
static VkImageView depthImageView = NULL;
static VkFormat depthFormat = 0;

static VkFramebuffer * swapchainFramebuffer = NULL;

// static VkFramebuffer * particleFramebuffer = NULL;

static VkImage texturesImage = NULL;
static VkDeviceMemory textureImageMem = NULL;
static VkImageView textureImageView = NULL;

static VkImage loadingImage = NULL;
static VkDeviceMemory loadingImageMem = NULL;
static VkImageView loadingImageView = NULL;

static VkImage textImage = NULL;
static VkDeviceMemory textImageMem = NULL;
static VkImageView textImageView = NULL;

static VkSampler textureSampler = NULL;

static VkBuffer vertexBuffer = NULL;
static VkDeviceMemory vertexBufferMem = NULL;
static void * vertexBufferMemMapped = NULL;
static uint32_t verticesCount = 4;
//three vertices of a triangle and color
static Vertex * vertices = NULL;

static VkBuffer indexBuffer = NULL;
static VkDeviceMemory indexBufferMem = NULL;
static void * indexBufferMemMapped = NULL;
static uint32_t indicesCount = 6;
static uint16_t * indices_v = NULL;

static VkBuffer * graphicUniformBuffers = NULL;
static VkDeviceMemory * graphicUniformBuffersMemory = NULL;
static void ** graphicUniformBufferMapped = NULL;
static UniformBufferObject ubo = {};

static VkDescriptorPoolSize * graphicDescriptorPoolSize = NULL;
static uint32_t graphicPoolSizeCount = 0;

static VkBuffer * computeUniformBuffers = NULL;
static VkDeviceMemory * computeUniformBuffersmemory = NULL;
static void ** computeUniformBufferMapped = NULL;
static ComputeUniformBufferObject computeUbo = {};

static VkDescriptorPoolSize * particleDescriptorPoolSize = NULL;
static uint32_t particlePoolSizeCount = 0;

static VkDescriptorPoolSize * computeDescriptorPoolSize = NULL;
static uint32_t computePoolSizeCount = 0;

static VkDescriptorPool graphicDescriptorPool = NULL;
static VkDescriptorSet * graphicDescriptorSets = NULL;

static VkDescriptorPool particleDescriptorPool = NULL;
static VkDescriptorSet * particleDescriptorSets = NULL;

static VkDescriptorPool computeDescriptorPool = NULL;
static VkDescriptorSet * computeDescriptorSets = NULL;

static VkCommandBuffer * commandBuffer = NULL;

// static VkCommandBuffer * particleCommandBuffer = NULL;

static VkCommandBuffer * computeCommandBuufer = NULL;

static VkSemaphore * imageAvailableSemaphore = NULL;
static VkSemaphore * renderFinishedSemaphore = NULL;

static VkFence * inFlightFence = NULL;

// static VkSemaphore * particleRenderFinishedSemaphore = NULL;

// static VkFence * particleInFlightFence = NULL;

static VkSemaphore * computeFinishedSemaphore = NULL;
static VkFence * computeInFlightFences = NULL;

static uint32_t currentFrame = 0;

static float camera_X = 0.0f;
static float camera_Y = 0.0f;

static float pictureX = 0;
static float pictureY = 0;

// static bool moveEnabled = false;

static VkBuffer * shaderStorageBuffers = NULL;
static VkDeviceMemory * shaderStorageBuffersMem = NULL;

static Particle * particles = NULL;

static ImageRotate pictureImageRotate = {0.0f};

Recreate recreateSwap = {};

//store all compoents for initialize vulkan in a struct
VK_ALL allInOne = {};


static inline void initializeRecreate(void)
{
    recreateSwap.DevicePack.pDevice = &device;
    recreateSwap.DevicePack.pPhysicalDevice = &physicalDevice;

    recreateSwap.pSurfaceCapabilities = &surfaceCapabilities;
    recreateSwap.pSurfaceFormat = &surfaceFormat;
    recreateSwap.pPresentMode = &presentMode;

    recreateSwap.pSurface = &surface;
    recreateSwap.pOldExtent2D = &oldExtent2D;
    recreateSwap.pExtent2D = &extent2D;
    recreateSwap.ppVertices = &vertices;

    recreateSwap.pIndices = &indices;
    recreateSwap.pGraphicQueue = &graphicQueue;

    recreateSwap.swapchainFormat = swapchainFormat;
    recreateSwap.pSwapchain = &swapchain;
    recreateSwap.pSwapchainCommandPool = &swapchainCommandPool;

    recreateSwap.imageCount = &imageCount;
    recreateSwap.ppSwapchainImages = &swapchainImages;
    recreateSwap.ppSwapchainImageViews = &swapchainImageViews;
    
    recreateSwap.ppSwapchainFramebuffer = &swapchainFramebuffer;

    recreateSwap.pDepthImage = &depthImage;
    recreateSwap.pDepthImageView = &depthImageView;
    recreateSwap.pDepthImageMem = &depthImageMemory;

    recreateSwap.pRenderPass = &renderPass;
}
static inline void initializeAllInOne(void)
{
    allInOne.pPhysicalDevice = &physicalDevice;

    allInOne.pDevice = &device;

    allInOne.pGraphicQueue = &graphicQueue;
    allInOne.pPresentQueue = &presentQueue;

    allInOne.pComputeQueue = &computeQueue;

    allInOne.pExtent2D = &extent2D;
    allInOne.pSwapchain = &swapchain;

    allInOne.pGraphicPipelineLayout = &graphicPipelineLayout;

    allInOne.pParticlePipelineLayout = &particlePipelineLayout;

    allInOne.pComputePipelineLayout = &computePipelineLayout;

    allInOne.pRenderPass = &renderPass;

    allInOne.pGraphicPipeline = &graphicPipeline;

    allInOne.pParticlePipeline = &particlePipeline;

    allInOne.pComputePipeline = &computePipeline;

    allInOne.ppSwapchainFramebuffer = &swapchainFramebuffer;
    allInOne.pSwapchainCommandPool = &swapchainCommandPool;

    allInOne.pVertexBuffer = &vertexBuffer;
    allInOne.ppVertices = &vertices;
    allInOne.pVerticesCount = &verticesCount;
    allInOne.pVertexBufferMem = &vertexBufferMem;
    allInOne.ppVertexBufferMemMapped = &vertexBufferMemMapped;

    allInOne.pIndexBuffer = &indexBuffer;
    allInOne.ppIndices = &indices_v;
    allInOne.pIndicesCount = &indicesCount;
    allInOne.pIndexBufferMem = &indexBufferMem;
    allInOne.ppIndexBufferMemMapped = &indexBufferMemMapped;

    allInOne.pppGraphicUniformBufferMapped = &graphicUniformBufferMapped;

    allInOne.pGraphicUbo = &ubo;

    allInOne.ppGraphicDescriptorSets = &graphicDescriptorSets;

    allInOne.ppParticleDescriptorSets = &particleDescriptorSets;

    allInOne.pComputeUbo = &computeUbo;

    allInOne.pppComputeUniformBufferMapped = &computeUniformBufferMapped;

    allInOne.ppComputeDescriptorSets = &computeDescriptorSets;

    allInOne.ppShaderStorageBuffers = &shaderStorageBuffers;

    allInOne.ppCommandBuffer = &commandBuffer;

    allInOne.ppComputeCommandBuffer = &computeCommandBuufer;

    allInOne.ppImageAvailableSemaphore = &imageAvailableSemaphore;
    allInOne.ppRenderFinishedSemaphore = &renderFinishedSemaphore;

    allInOne.ppInFlightFence = &inFlightFence;

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

    vulkanVersion();

    createInstance(&instance);

    createSurface(window, &instance, &surface);

    pickPhysicalDevice(&instance, &physicalDevice);

    findQueueFamilies(&physicalDevice, &surface, &indices);
    createLogicalDevice(&physicalDevice, indices, &device);

    createGraphicsQueue(&device, indices.graphicsFamily.familyIndice, &graphicQueue);
    createPresentQueue(&device, indices.presentFamily.familyIndice, &presentQueue);
    createComputeQueue(&device, indices.computeFamily.familyIndice, &computeQueue);


    

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.flags = 0;
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = device;
    allocatorInfo.preferredLargeHeapBlockSize = 0;

    SDL_allocationCallBacks.pUserData = NULL;
    SDL_allocationCallBacks.pfnAllocation = SDL_VK_allocationFunc;
    SDL_allocationCallBacks.pfnReallocation = SDL_VK_reallocationFunc;
    SDL_allocationCallBacks.pfnFree = SDL_VK_freeFunc;
    SDL_allocationCallBacks.pfnInternalAllocation = NULL;
    SDL_allocationCallBacks.pfnInternalFree = NULL;

    allocatorInfo.pAllocationCallbacks = &SDL_allocationCallBacks;
    allocatorInfo.pDeviceMemoryCallbacks = NULL;
    allocatorInfo.pHeapSizeLimit = NULL;
    allocatorInfo.pVulkanFunctions = NULL;
    allocatorInfo.instance = instance;
    allocatorInfo.pTypeExternalMemoryHandleTypes = NULL;

    VkResult result = vmaCreateAllocator(&allocatorInfo, &vmaAllocator);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VMA allocator\n");
        // exit(EXIT_FAILURE);
    }

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = NULL;
    bufferInfo.flags = 0;
    bufferInfo.size = 556400;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = 0;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = NULL;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = 0;
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.requiredFlags = 0;
    allocInfo.preferredFlags = 0;
    allocInfo.memoryTypeBits = 0;
    allocInfo.pool = NULL;
    allocInfo.pUserData = NULL;
    allocInfo.priority = 0;

    VkBuffer largeBuffer = NULL;
    VmaAllocation largeBufferAllocation = NULL;

    vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocInfo, &largeBuffer, &largeBufferAllocation, NULL);

    vmaDestroyBuffer(vmaAllocator, largeBuffer, largeBufferAllocation);

    

    getSurfaceFormats(&physicalDevice, &surface, &surfaceFormat);
    getPresentModes(&physicalDevice, &surface, &presentMode);
    getSurfaceCapabilities(&physicalDevice, &surface, &surfaceCapabilities);
    extent2D.width = width;
    extent2D.height = height;
    //chooseSwapExtent(&physicalDevice, &surface, &surfaceCapabilities, &extent2D);
    createSwapchain(&device, &surface, &surfaceFormat, &presentMode, &surfaceCapabilities, &extent2D, indices, &swapchain);

    getSwapchainNumber(&device, &swapchain, &imageCount);
    createSwapchainImage(&device, &swapchain, imageCount, &swapchainImages);
    swapchainFormat = surfaceFormat.format;

    //swapchain image view
    createSwapchainImageView(&device, &swapchainImages, imageCount, swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, &swapchainImageViews);

    findDepthFormat(&physicalDevice, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, &depthFormat);
    createRenderPass(&device, &swapchainFormat, &depthFormat, &renderPass);



    /*unfixed code*/

    //graphic shader
    createShaderModule(&device, TriangleVertexShader, &vertShaderCode);
    addShaderStageCreateInfo(&vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT, &graphicShaderCount, &graphciShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, 0, &graphicBindingCount, &graphicBindings);//0
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT, &graphicPoolSizeCount, &graphicDescriptorPoolSize);

    createShaderModule(&device, TriangleFragmentShader, &fragShaderCode);
    addShaderStageCreateInfo(&fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT, &graphicShaderCount, &graphciShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 3, 1, &graphicBindingCount, &graphicBindings);//1
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT * 3, &graphicPoolSizeCount, &graphicDescriptorPoolSize);

    addDescriptorSetLayout(&device, graphicBindingCount, graphicBindings, 0, &graphicDescriptorSetLayout);

    createPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ImageRotate), &pushConstantRange);
    createPipelineLayout(&device, 1, &graphicDescriptorSetLayout, 1, &pushConstantRange, &graphicPipelineLayout);
    createGraphicsPipeline(&device, &extent2D, graphicShaderCount, graphciShaderStageCreateInfo, &graphicPipelineLayout, &renderPass, &graphicPipeline);

    createDescriptorPool(&device, graphicPoolSizeCount, graphicDescriptorPoolSize, 2, &graphicDescriptorPool);

    //particle shader
    createShaderModule(&device, ParticleVertexShader, &particleVertexShaderCode);
    addShaderStageCreateInfo(&particleVertexShaderCode, VK_SHADER_STAGE_VERTEX_BIT, &particleShaderCount, &particleShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, 1, &particleBindingCount, &particleBindings);//1
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT, &particlePoolSizeCount, &particleDescriptorPoolSize);

    createShaderModule(&device, ParticleFragmentShader, &particleFragmentShaderCode);
    addShaderStageCreateInfo(&particleFragmentShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT, &particleShaderCount, &particleShaderStageCreateInfo);

    addDescriptorSetLayout(&device, particleBindingCount, particleBindings, 0, &particleDescriptorSetLayout);

    createPipelineLayout(&device, 1, &particleDescriptorSetLayout, 0, NULL, &particlePipelineLayout);
    createParticlePipeline(&device, &extent2D, particleShaderCount, particleShaderStageCreateInfo, &particlePipelineLayout, &renderPass, &particlePipeline);

    createDescriptorPool(&device, particlePoolSizeCount, particleDescriptorPoolSize, 2, &particleDescriptorPool);

    //compute shader
    createShaderModule(&device, ParticleComputeShader, &compShaderCode);
    addShaderStageCreateInfo(&compShaderCode, VK_SHADER_STAGE_COMPUTE_BIT, &computeShaderCount, &computeShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1, 0, &computeBindingCount, &computeBindings);//0
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT, &computePoolSizeCount, &computeDescriptorPoolSize);

    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1, 1, &computeBindingCount, &computeBindings);//1
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1, 2, &computeBindingCount, &computeBindings);//2
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_FRAMES_IN_FLIGHT * 2, &computePoolSizeCount, &computeDescriptorPoolSize);

    addDescriptorSetLayout(&device, computeBindingCount, computeBindings, 0, &computeDescriptorSetLayout);

    createPipelineLayout(&device, 1, &computeDescriptorSetLayout, 0, NULL, &computePipelineLayout);
    createComputePipeline(&device, &computePipelineLayout, computeShaderStageCreateInfo, &computePipeline);

    createDescriptorPool(&device, computePoolSizeCount, computeDescriptorPoolSize, 3, &computeDescriptorPool);


    createCommandPool(&device, indices.graphicsFamily.familyIndice, &swapchainCommandPool);

    createCommandPool(&device, indices.computeFamily.familyIndice, &computeCommandPool);

    createDepthResoures(&physicalDevice, &device, &extent2D, &swapchainCommandPool, &graphicQueue, &depthImage, &depthImageMemory, &depthImageView);

    createFrameBuffer(&device, &extent2D, imageCount, swapchainImageViews, &depthImageView, &renderPass, &swapchainFramebuffer);

    createTextureImage(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, CirclePng, VK_FORMAT_R8G8B8A8_SRGB, &texturesImage, &textureImageMem);
    createTextureImageView(&device, &texturesImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, &textureImageView);

    // loadingImage = (VkImage*)malloc(8 * sizeof(VkImage));
    // loadingImageView = (VkImageView*)malloc(8 * sizeof(VkImageView));
    // loadingImageMem = (VkDeviceMemory*)malloc(8 * sizeof(VkDeviceMemory));
    // for (int i = 0;i < 8;i++)
    // {
    //     char loadingPicPath[100];
    //     strcpy(loadingPicPath, "Textures\\loading1\\loading");
    //     char tempIndex = i + 49;
    //     char * tempPath = strcat(strncat(loadingPicPath, &tempIndex, 1), ".png");
    createTextureImage(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, Loading1Png, VK_FORMAT_R8G8B8A8_SRGB, &loadingImage, &loadingImageMem);
    createTextureImageView(&device, &loadingImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, &loadingImageView);
    //}

    createTextureImage(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, Font1Png, VK_FORMAT_R8_UNORM, &textImage, &textImageMem);
    createTextureImageView(&device, &textImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &textImageView);
    
    createTextureSampler(&physicalDevice, &device, &textureSampler);

    vertices = (Vertex *)SDL_calloc(BALLCOUNT * 4 + 100 * 4, sizeof(Vertex));
    vertices[0] = (Vertex){{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[1] = (Vertex){{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[2] = (Vertex){{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[3] = (Vertex){{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};

    /*vertices[4] = (Vertex){{-0.0f, -0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[5] = (Vertex){{1.0f, -0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[6] = (Vertex){{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[7] = (Vertex){{-0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};*/

    verticesCount = 4;

    positionInitialize(-32, -32, 64, 64, extent2D, &vertices, 0);
    vertices[0].pos[2] = 0.0f; vertices[1].pos[2] = 0.0f; vertices[2].pos[2] = 0.0f; vertices[3].pos[2] = 0.0f;
    vertices[4].pos[2] = 0.9f; vertices[5].pos[2] = 0.9f; vertices[6].pos[2] = 0.9f; vertices[7].pos[2] = 0.9f;
    vertices[8].pos[2] = 0.9f; vertices[9].pos[2] = 0.9f; vertices[10].pos[2] = 0.9f; vertices[11].pos[2] = 0.9f;
    //positionInitialize(-24, -24, 16, 16, extent2D, &vertices, 1);

    createVertexBuffer(&physicalDevice, &device, &vertexBuffer, &vertexBufferMem, &vertexBufferMemMapped, vertices, BALLCOUNT * 4 + 100 * 4);

    indices_v = (uint16_t *)SDL_calloc(BALLCOUNT * 6 + 100 * 6, sizeof(uint16_t));
    indices_v[0] = 0; indices_v[1] = 1; indices_v[2] = 2; indices_v[3] = 2; indices_v[4] = 3; indices_v[5] = 0;
    //indices_v[6] = 4; indices_v[7] = 5; indices_v[8] = 6; indices_v[9] = 6; indices_v[10] = 7; indices_v[11] = 4;
    indicesCount = 6;

    createIndexBuffer(&physicalDevice, &device, &indexBuffer, &indexBufferMem, &indexBufferMemMapped, indices_v, BALLCOUNT * 6 + 100 * 6);

    for (int i = 0;i < 50;i++)
    {
        positionInitialize(-300 + i * 24, -100, 24, 24, extent2D, &vertices, i + BALLCOUNT);
        for (int j = 0;j < 4;j++)
        {
            vertices[(i + BALLCOUNT) * 4 + j].texCoord[0] = 0.0f;
            vertices[(i + BALLCOUNT) * 4 + j].texCoord[1] = 0.0f;
            vertices[(i + BALLCOUNT) * 4 + j].pos[2] = 0.1f;
        }
        int index = (i + BALLCOUNT) * 6;
        int serial = (i + BALLCOUNT) * 4;
        indices_v[index] = serial;
        indices_v[index + 1] = serial + 1;
        indices_v[index + 2] = serial + 2;
        indices_v[index + 3] = serial + 2;
        indices_v[index + 4] = serial + 3;
        indices_v[index + 5] = serial;
    }
    for (int i = 0;i < 50;i++)
    {
        positionInitialize(-300 + i * 12, -114, 12, 12, extent2D, &vertices, i + BALLCOUNT + 50);
        for (int j = 0;j < 4;j++)
        {
            vertices[(i + BALLCOUNT + 50) * 4 + j].texCoord[0] = 0.0f;
            vertices[(i + BALLCOUNT + 50) * 4 + j].texCoord[1] = 0.0f;
            vertices[(i + BALLCOUNT + 50) * 4 + j].pos[2] = 0.1f;
        }
        int index = (i + BALLCOUNT + 50) * 6;
        int serial = (i + BALLCOUNT + 50) * 4;
        indices_v[index] = serial;
        indices_v[index + 1] = serial + 1;
        indices_v[index + 2] = serial + 2;
        indices_v[index + 3] = serial + 2;
        indices_v[index + 4] = serial + 3;
        indices_v[index + 5] = serial;
    }

    //initializeMovingBuffer(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, &movingStagingBuffer, &movingStagingMemory, &movingBufferMapped, vertices, verticesCount);

    createUniformBuffers(&physicalDevice, &device, &graphicUniformBuffers, &graphicUniformBuffersMemory, &graphicUniformBufferMapped, sizeof(UniformBufferObject));

    createUniformBuffers(&physicalDevice, &device, &computeUniformBuffers, &computeUniformBuffersmemory, &computeUniformBufferMapped, sizeof(ComputeUniformBufferObject));

    createShaderStorageBuffers(&physicalDevice, &device, &swapchainCommandPool, &computeQueue, extent2D, &shaderStorageBuffers, &shaderStorageBuffersMem, &particles);

    //graphics
    VkImageView tempImageView[3];
    tempImageView[0] = loadingImageView;
    tempImageView[1] = textureImageView;
    tempImageView[2] = textImageView;
    createGraphicDescriptorSets(&device, &graphicUniformBuffers, graphicDescriptorSetLayout, &graphicDescriptorPool, &graphicDescriptorSets, tempImageView, &textureSampler);

    createCommandbuffer(&device, &swapchainCommandPool, &commandBuffer);

    createSemaphore(&device, &imageAvailableSemaphore);
    createSemaphore(&device, &renderFinishedSemaphore);
 
    createFence(&device, &inFlightFence);

    //particle
    createParticleDescriptorSets(&device, &graphicUniformBuffers, particleDescriptorSetLayout, &particleDescriptorPool, &particleDescriptorSets);

    //compute
    createComputeDescriptorSets(&device, &computeUniformBuffers, &shaderStorageBuffers, computeDescriptorSetLayout, &computeDescriptorPool, &computeDescriptorSets);

    createCommandbuffer(&device, &computeCommandPool, &computeCommandBuufer);

    createSemaphore(&device, &computeFinishedSemaphore);

    createFence(&device, &computeInFlightFences);

    initializeAllInOne();

    initializeRecreate();

    resultVulkan(VK_SUCCESS, initializedF, 0);
}

extern bool input_end;

void cleanup(FuncCode code)
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
            vkDestroyFence(device, computeInFlightFences[i], NULL);
        }
        SDL_free(computeInFlightFences);
        logMessage("compute in flight fences destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, computeFinishedSemaphore[i], NULL);
        }
        SDL_free(computeFinishedSemaphore);
        logMessage("compute finished semaphore destroyed");
        /*fall through*/

        case createFenceF:
        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyFence(device, inFlightFence[i], NULL);
        }
        SDL_free(inFlightFence);
        logMessage("in flight fence destroyed");
        /*fall through*/

        case createSemaphoreF:
        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphore[i], NULL);
        }
        SDL_free(renderFinishedSemaphore);
        logMessage("render finished semaphore destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, imageAvailableSemaphore[i], NULL);
        }
        SDL_free(imageAvailableSemaphore);
        logMessage("image available semaphore destroyed");

        SDL_free(computeCommandBuufer);
        logMessage("compute command buffer freed");
        /*fall through*/

        case createCommandbufferF:
        SDL_free(commandBuffer);
        logMessage("command buffer freed");

        vkDestroyDescriptorPool(device, computeDescriptorPool, NULL);
        SDL_free(computeDescriptorPoolSize);
        logMessage("compute descriptro pool destroyed");

        vkDestroyDescriptorPool(device, particleDescriptorPool, NULL);
        SDL_free(particleDescriptorPoolSize);
        logMessage("particle descriptor pool destroyed");
        /*fall through*/

        case createDescriptorPoolF:
        vkDestroyDescriptorPool(device, graphicDescriptorPool, NULL);
        SDL_free(graphicDescriptorPoolSize);
        logMessage("graphic descriptor pool destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, computeUniformBuffersmemory[i]);
            vkDestroyBuffer(device, computeUniformBuffers[i], NULL);
        }
        SDL_free(computeUniformBuffers);
        logMessage("compute uniform buffers destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, computeUniformBuffersmemory[i], NULL);
        }
        SDL_free(computeUniformBuffersmemory);
        SDL_free(computeUniformBufferMapped);
        logMessage("compute uniform buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyBuffer(device, shaderStorageBuffers[i], NULL);
        }
        SDL_free(shaderStorageBuffers);
        logMessage("shader storage buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, shaderStorageBuffersMem[i], NULL);
        }
        SDL_free(shaderStorageBuffersMem);
        logMessage("shader storage buffer memory freed");
        /*fall through*/

        case createUniformBuffersF:
        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, graphicUniformBuffersMemory[i]);
            vkDestroyBuffer(device, graphicUniformBuffers[i], NULL);
        }
        SDL_free(graphicUniformBuffers);
        logMessage("graphic uniform buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, graphicUniformBuffersMemory[i], NULL);
        }
        SDL_free(graphicUniformBuffersMemory);
        SDL_free(graphicUniformBufferMapped);
        logMessage("graphic uniform buffer memory freed");
        /*fall through*/

        case createIndexBufferF:
        vkUnmapMemory(device, indexBufferMem);

        vkDestroyBuffer(device, indexBuffer, NULL);
        SDL_free(indices_v);
        logMessage("index buffer destroyed");

        vkFreeMemory(device, indexBufferMem, NULL);
        logMessage("index buffer memory freed");
        /*fall through*/

        case createVertexBufferF:
        vkUnmapMemory(device, vertexBufferMem);

        vkDestroyBuffer(device, vertexBuffer, NULL);
        SDL_free(vertices);
        logMessage("vertex buffer destroyed");

        vkFreeMemory(device, vertexBufferMem, NULL);
        logMessage("vertex buffer memory freed");
        /*fall through*/

        case createTextureSamplerF:
        vkDestroySampler(device, textureSampler, NULL);
        logMessage("texture sampler detroyed");
        /*fall through*/

        case createTextureImageViewF:
        vkDestroyImageView(device, textureImageView, NULL);
        logMessage("texture image view destroyed");
        /*fall through*/

        case createTextureImageF:
        vkDestroyImage(device, texturesImage, NULL);
        logMessage("texture image destroyed");

        vkFreeMemory(device, textureImageMem, NULL);
        logMessage("texture image memory freed");

        vkDestroyImageView(device, loadingImageView, NULL);
        logMessage("texture image view destroyed");

        vkDestroyImage(device, loadingImage, NULL);
        logMessage("texture image destroyed");

        vkFreeMemory(device, loadingImageMem, NULL);
        logMessage("texture image memory freed");

        vkDestroyImageView(device, textImageView, NULL);
        logMessage("texture image view destroyed");

        vkDestroyImage(device, textImage, NULL);
        logMessage("texture image destroyed");

        vkFreeMemory(device, textImageMem, NULL);
        logMessage("texture image memory freed");
        /*fall through*/

        case createFrameBufferF:
        destroyedFrameBuffer(&device, imageCount, swapchainFramebuffer);
        SDL_free(swapchainFramebuffer);
        logMessage("framebuffer destroyed");
        /*fall through*/

        case createDepthResouresF:
        vkDestroyImageView(device, depthImageView, NULL);
        logMessage("depth image view destroyed");

        vkDestroyImage(device, depthImage, NULL);
        logMessage("depth image destroyed");

        vkFreeMemory(device, depthImageMemory, NULL);
        logMessage("depth image memory freed");
        
        vkDestroyCommandPool(device, computeCommandPool, NULL);
        logMessage("compute commandpool destroyed");
        /*fall through*/

        case createCommandPoolF:
        vkDestroyCommandPool(device, swapchainCommandPool, NULL);
        logMessage("commandpool destroyed");

        vkDestroyPipeline(device, computePipeline, NULL);
        logMessage("compute pipeline destroyed");

        vkDestroyPipeline(device, particlePipeline, NULL);
        logMessage("particle pipeline destroyed");
        /*fall through*/

        case createGraphicsPipelineF:
        vkDestroyPipeline(device, graphicPipeline, NULL);
        logMessage("graphic pipelne destroyed");
        /*fall through*/

        case createRenderPassF:
        vkDestroyRenderPass(device, renderPass, NULL);
        logMessage("renderPass destroyed");

        vkDestroyPipelineLayout(device, computePipelineLayout, NULL);
        logMessage("compute pipeline layout destroyed");

        vkDestroyPipelineLayout(device, particlePipelineLayout, NULL);
        logMessage("particle pipeline layout destroyed");
        /*fall through*/

        case createPipelineLayoutF:
        vkDestroyPipelineLayout(device, graphicPipelineLayout, NULL);
        logMessage("graphic pipeline layout destroyed");

        vkDestroyDescriptorSetLayout(device, *computeDescriptorSetLayout, NULL);
        SDL_free(computeDescriptorSetLayout);
        SDL_free(computeBindings);
        logMessage("compute descriptor set layout destroyed");

        vkDestroyDescriptorSetLayout(device, *particleDescriptorSetLayout, NULL);
        SDL_free(particleDescriptorSetLayout);
        SDL_free(particleBindings);
        logMessage("particle descriptoe set layout destroyed");
        /*fall through*/

        case createDescriptorSetLayoutF:
        vkDestroyDescriptorSetLayout(device, *graphicDescriptorSetLayout, NULL);
        SDL_free(graphicDescriptorSetLayout);
        SDL_free(graphicBindings);
        logMessage("graphic descriptor set layout destroyed");

        vkDestroyShaderModule(device, compShaderCode, NULL);
        SDL_free(computeShaderStageCreateInfo);
        logMessage("compute shader stage create info destroyed");

        vkDestroyShaderModule(device, particleFragmentShaderCode, NULL);
        vkDestroyShaderModule(device, particleVertexShaderCode, NULL);
        SDL_free(particleShaderStageCreateInfo);
        logMessage("particle shader stage create info destroyed");
        /*fall through*/

        case createShaderModuleF:
        vkDestroyShaderModule(device, fragShaderCode, NULL);
        vkDestroyShaderModule(device, vertShaderCode, NULL);
        SDL_free(graphciShaderStageCreateInfo);
        logMessage("shaderCode destroyed");
        /*fall through*/

        case createSwapchainImageViewsF:
        destroyImageViews(&device, swapchainImageViews, imageCount);
        SDL_free(swapchainImageViews);
        logMessage("swapchain image views destroyed");
        /*fall through*/

        case createSwapchainImageF:
        SDL_free(swapchainImages);
        logMessage("swapchainImages freed");
        vmaDestroyAllocator(vmaAllocator);
        /*fall through*/

        case createSwapchainF:
        vkDestroySwapchainKHR(device, swapchain, NULL);
        logMessage("swapchain destroyed");
        /*fall through*/

        case createLogicalDeviceF:
        vkDestroyDevice(device, NULL);
        logMessage("device destroyed");
        /*fall through*/

        case createSurfaceF:
        vkDestroySurfaceKHR(instance, surface, NULL);
        logMessage("surface destroyed");
        /*fall through*/

        case createInstanceF:
        vkDestroyInstance(instance, NULL);
        logMessage("instance destroyed");

        SDL_DestroyWindow(window);
        input_end = true;
        logMessage("window destroyed");
        /*fall through*/

        default:
    }
}