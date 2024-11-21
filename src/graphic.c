#include "vulkan_code.h"
#include "graphic.h"

//declare a sdl window
SDL_Window * window = NULL;

//window's width and height
uint32_t width = 800;
uint32_t height = 600;

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

static VkInstance instance = VK_NULL_HANDLE;

static VkSurfaceKHR surface = VK_NULL_HANDLE;

static VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

static QueueFamilyIndices indices = {};
static VkDevice device = VK_NULL_HANDLE;
static VkQueue graphicQueue = VK_NULL_HANDLE;
static VkQueue presentQueue = VK_NULL_HANDLE;

static VkQueue computeQueue = VK_NULL_HANDLE;

static VkSurfaceFormatKHR surfaceFormat = {};
static VkPresentModeKHR presentMode = 0;
static VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
static VkExtent2D extent2D = {};
static VkExtent2D oldExtent2D = {};
static VkSwapchainKHR swapchain = VK_NULL_HANDLE;

static uint32_t imageCount = 0;
static VkFormat swapchainFormat = 0;

static VkImage * swapchainImages = VK_NULL_HANDLE;
static VkImageView * swapchainImageViews = VK_NULL_HANDLE;

static VkShaderModule vertShaderCode = VK_NULL_HANDLE;
static VkShaderModule fragShaderCode = VK_NULL_HANDLE;
static uint32_t graphicShaderCount = 0;
static VkPipelineShaderStageCreateInfo * graphciShaderStageCreateInfo = VK_NULL_HANDLE;

static VkShaderModule particleVertexShaderCode = VK_NULL_HANDLE;
static VkShaderModule particleFragmentShaderCode = VK_NULL_HANDLE;
static uint32_t particleShaderCount = 0;
static VkPipelineShaderStageCreateInfo * particleShaderStageCreateInfo = VK_NULL_HANDLE;

static VkShaderModule compShaderCode = VK_NULL_HANDLE;
static uint32_t computeShaderCount = 0;
static VkPipelineShaderStageCreateInfo * computeShaderStageCreateInfo = VK_NULL_HANDLE;

static VkDescriptorSetLayout * graphicDescriptorSetLayout = VK_NULL_HANDLE;

static VkDescriptorSetLayout * particleDescriptorSetLayout = VK_NULL_HANDLE;

static VkDescriptorSetLayout * computeDescriptorSetLayout = VK_NULL_HANDLE;

// static uint32_t graphicBinding = 0;
static VkDescriptorSetLayoutBinding * graphicBindings = VK_NULL_HANDLE;
static uint32_t graphicBindingCount = 0;
static VkPushConstantRange pushConstantRange = {};
static VkPipelineLayout graphicPipelineLayout = VK_NULL_HANDLE;

static VkDescriptorSetLayoutBinding * particleBindings = VK_NULL_HANDLE;
static uint32_t particleBindingCount = 0;
static VkPipelineLayout particlePipelineLayout = VK_NULL_HANDLE;

// static uint32_t computeBinding = 0;
static VkDescriptorSetLayoutBinding * computeBindings = VK_NULL_HANDLE;
static uint32_t computeBindingCount = 0;
static VkPipelineLayout computePipelineLayout = VK_NULL_HANDLE;

// static VkSubpassDependency dependency = {};
static VkRenderPass renderPass = VK_NULL_HANDLE;

static VkPipeline graphicPipeline = VK_NULL_HANDLE;

static VkPipeline particlePipeline = VK_NULL_HANDLE;

static VkPipeline computePipeline = VK_NULL_HANDLE;

static VkCommandPool swapchainCommandPool = VK_NULL_HANDLE;

static VkCommandPool computeCommandPool = VK_NULL_HANDLE;

static VkImage depthImage = VK_NULL_HANDLE;
static VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
static VkImageView depthImageView = VK_NULL_HANDLE;
static VkFormat depthFormat = 0;

static VkFramebuffer * swapchainFramebuffer = VK_NULL_HANDLE;

// static VkFramebuffer * particleFramebuffer = VK_NULL_HANDLE;

static VkImage texturesImage = VK_NULL_HANDLE;
static VkDeviceMemory textureImageMem = VK_NULL_HANDLE;
static VkImageView textureImageView = VK_NULL_HANDLE;

static VkImage loadingImage = VK_NULL_HANDLE;
static VkDeviceMemory loadingImageMem = VK_NULL_HANDLE;
static VkImageView loadingImageView = VK_NULL_HANDLE;

static VkImage textImage = VK_NULL_HANDLE;
static VkDeviceMemory textImageMem = VK_NULL_HANDLE;
static VkImageView textImageView = VK_NULL_HANDLE;

static VkSampler textureSampler = VK_NULL_HANDLE;

static VkBuffer vertexBuffer = VK_NULL_HANDLE;
static VkDeviceMemory vertexBufferMem = VK_NULL_HANDLE;
static void * vertexBufferMemMapped = VK_NULL_HANDLE;
static uint32_t verticesCount = 4;
//three vertices of a triangle and color
static Vertex * vertices = NULL;

static VkBuffer indexBuffer = VK_NULL_HANDLE;
static VkDeviceMemory indexBufferMem = VK_NULL_HANDLE;
static void * indexBufferMemMapped = NULL;
static uint32_t indicesCount = 6;
static uint16_t * indices_v = NULL;

static VkBuffer * graphicUniformBuffers = VK_NULL_HANDLE;
static VkDeviceMemory * graphicUniformBuffersMemory = VK_NULL_HANDLE;
static void ** graphicUniformBufferMapped = VK_NULL_HANDLE;
static UniformBufferObject ubo = {};

static VkDescriptorPoolSize * graphicDescriptorPoolSize = VK_NULL_HANDLE;
static uint32_t graphicPoolSizeCount = 0;

static VkBuffer * computeUniformBuffers = VK_NULL_HANDLE;
static VkDeviceMemory * computeUniformBuffersmemory = VK_NULL_HANDLE;
static void ** computeUniformBufferMapped = VK_NULL_HANDLE;
static ComputeUniformBufferObject computeUbo = {};

static VkDescriptorPoolSize * particleDescriptorPoolSize = VK_NULL_HANDLE;
static uint32_t particlePoolSizeCount = 0;

static VkDescriptorPoolSize * computeDescriptorPoolSize = VK_NULL_HANDLE;
static uint32_t computePoolSizeCount = 0;

static VkDescriptorPool graphicDescriptorPool = VK_NULL_HANDLE;
static VkDescriptorSet * graphicDescriptorSets = VK_NULL_HANDLE;

static VkDescriptorPool particleDescriptorPool = VK_NULL_HANDLE;
static VkDescriptorSet * particleDescriptorSets = VK_NULL_HANDLE;

static VkDescriptorPool computeDescriptorPool = VK_NULL_HANDLE;
static VkDescriptorSet * computeDescriptorSets = VK_NULL_HANDLE;

static VkCommandBuffer * commandBuffer = VK_NULL_HANDLE;

// static VkCommandBuffer * particleCommandBuffer = VK_NULL_HANDLE;

static VkCommandBuffer * computeCommandBuufer = VK_NULL_HANDLE;

static VkSemaphore * imageAvailableSemaphore = VK_NULL_HANDLE;
static VkSemaphore * renderFinishedSemaphore = VK_NULL_HANDLE;

static VkFence * inFlightFence = VK_NULL_HANDLE;

// static VkSemaphore * particleRenderFinishedSemaphore = VK_NULL_HANDLE;

// static VkFence * particleInFlightFence = VK_NULL_HANDLE;

static VkSemaphore * computeFinishedSemaphore = VK_NULL_HANDLE;
static VkFence * computeInFlightFences = VK_NULL_HANDLE;

static uint32_t currentFrame = 0;

static float camera_X = 0.0f;
static float camera_Y = 0.0f;

static float pictureX = 0;
static float pictureY = 0;

// static bool moveEnabled = false;

static VkBuffer * shaderStorageBuffers = VK_NULL_HANDLE;
static VkDeviceMemory * shaderStorageBuffersMem = VK_NULL_HANDLE;

static Particle * particles = VK_NULL_HANDLE;

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
    createGraphicsQueue(&device, indices.graphicsFamily, &graphicQueue);
    createPresentQueue(&device, indices.presentFamily, &presentQueue);
    createComputeQueue(&device, indices.computeFamily, &computeQueue);

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

    createPipelineLayout(&device, 1, &particleDescriptorSetLayout, 0, VK_NULL_HANDLE, &particlePipelineLayout);
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

    createPipelineLayout(&device, 1, &computeDescriptorSetLayout, 0, VK_NULL_HANDLE, &computePipelineLayout);
    createComputePipeline(&device, &computePipelineLayout, computeShaderStageCreateInfo, &computePipeline);

    createDescriptorPool(&device, computePoolSizeCount, computeDescriptorPoolSize, 3, &computeDescriptorPool);


    createCommandPool(&device, indices.graphicsFamily, &swapchainCommandPool);

    createCommandPool(&device, indices.computeFamily, &computeCommandPool);

    createDepthResoures(&physicalDevice, &device, &extent2D, &swapchainCommandPool, &graphicQueue, &depthImage, &depthImageMemory, &depthImageView);

    createFrameBuffer(&device, &extent2D, imageCount, swapchainImageViews, &depthImageView, &renderPass, &swapchainFramebuffer);

    createTextureImage(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, CirclePng, VK_FORMAT_R8G8B8A8_SRGB, &texturesImage, &textureImageMem);
    createTextureImageView(&device, &texturesImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, &textureImageView);

    /*loadingImage = (VkImage*)malloc(8 * sizeof(VkImage));
    loadingImageView = (VkImageView*)malloc(8 * sizeof(VkImageView));
    loadingImageMem = (VkDeviceMemory*)malloc(8 * sizeof(VkDeviceMemory));
    for (int i = 0;i < 8;i++)
    {
        char loadingPicPath[100];
        strcpy(loadingPicPath, "Textures\\loading1\\loading");
        char tempIndex = i + 49;
        char * tempPath = strcat(strncat(loadingPicPath, &tempIndex, 1), ".png");*/
    createTextureImage(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, LoadingPng, VK_FORMAT_R8G8B8A8_SRGB, &loadingImage, &loadingImageMem);
    createTextureImageView(&device, &loadingImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, &loadingImageView);
    //}

    createTextureImage(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, TextPng, VK_FORMAT_R8_UNORM, &textImage, &textImageMem);
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

    createUniformBuffers(&physicalDevice, &device, &graphicUniformBuffers, &graphicUniformBuffersMemory, &graphicUniformBufferMapped);

    createUniformBuffers(&physicalDevice, &device, &computeUniformBuffers, &computeUniformBuffersmemory, &computeUniformBufferMapped);

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

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyFence(device, computeInFlightFences[i], VK_NULL_HANDLE);
        }
        SDL_free(computeInFlightFences);
        logMessage("compute in flight fences destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyFence(device, inFlightFence[i], VK_NULL_HANDLE);
        }
        SDL_free(inFlightFence);
        logMessage("in flight fence destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, computeFinishedSemaphore[i], VK_NULL_HANDLE);
        }
        SDL_free(computeFinishedSemaphore);
        logMessage("compute finished semaphore destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphore[i], VK_NULL_HANDLE);
        }
        SDL_free(renderFinishedSemaphore);
        logMessage("render finished semaphore destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, imageAvailableSemaphore[i], VK_NULL_HANDLE);
        }
        SDL_free(imageAvailableSemaphore);
        logMessage("image available semaphore destroyed");

        SDL_free(computeCommandBuufer);
        logMessage("compute command buffer freed");

        SDL_free(commandBuffer);
        logMessage("command buffer freed");

        vkDestroyDescriptorPool(device, computeDescriptorPool, VK_NULL_HANDLE);
        SDL_free(computeDescriptorPoolSize);
        logMessage("compute descriptro pool destroyed");

        vkDestroyDescriptorPool(device, particleDescriptorPool, VK_NULL_HANDLE);
        SDL_free(particleDescriptorPoolSize);
        logMessage("particle descriptor pool destroyed");

        vkDestroyDescriptorPool(device, graphicDescriptorPool, VK_NULL_HANDLE);
        SDL_free(graphicDescriptorPoolSize);
        logMessage("graphic descriptor pool destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, computeUniformBuffersmemory[i]);
            vkDestroyBuffer(device, computeUniformBuffers[i], VK_NULL_HANDLE);
        }
        SDL_free(computeUniformBuffers);
        logMessage("compute uniform buffers destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, computeUniformBuffersmemory[i], VK_NULL_HANDLE);
        }
        SDL_free(computeUniformBuffersmemory);
        SDL_free(computeUniformBufferMapped);
        logMessage("compute uniform buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyBuffer(device, shaderStorageBuffers[i], VK_NULL_HANDLE);
        }
        SDL_free(shaderStorageBuffers);
        logMessage("shader storage buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, shaderStorageBuffersMem[i], VK_NULL_HANDLE);
        }
        SDL_free(shaderStorageBuffersMem);
        logMessage("shader storage buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, graphicUniformBuffersMemory[i]);
            vkDestroyBuffer(device, graphicUniformBuffers[i], VK_NULL_HANDLE);
        }
        SDL_free(graphicUniformBuffers);
        logMessage("graphic uniform buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, graphicUniformBuffersMemory[i], VK_NULL_HANDLE);
        }
        SDL_free(graphicUniformBuffersMemory);
        SDL_free(graphicUniformBufferMapped);
        logMessage("graphic uniform buffer memory freed");

        vkUnmapMemory(device, indexBufferMem);

        vkDestroyBuffer(device, indexBuffer, VK_NULL_HANDLE);
        SDL_free(indices_v);
        logMessage("index buffer destroyed");

        vkFreeMemory(device, indexBufferMem, VK_NULL_HANDLE);
        logMessage("index buffer memory freed");

        vkUnmapMemory(device, vertexBufferMem);

        vkDestroyBuffer(device, vertexBuffer, VK_NULL_HANDLE);
        SDL_free(vertices);
        logMessage("vertex buffer destroyed");

        vkFreeMemory(device, vertexBufferMem, VK_NULL_HANDLE);
        logMessage("vertex buffer memory freed");

        vkDestroySampler(device, textureSampler, VK_NULL_HANDLE);
        logMessage("texture sampler detroyed");

        vkDestroyImageView(device, textureImageView, VK_NULL_HANDLE);
        logMessage("texture image view destroyed");

        vkDestroyImage(device, texturesImage, VK_NULL_HANDLE);
        logMessage("texture image destroyed");

        vkFreeMemory(device, textureImageMem, VK_NULL_HANDLE);
        logMessage("texture image memory freed");

        vkDestroyImageView(device, loadingImageView, VK_NULL_HANDLE);
        logMessage("texture image view destroyed");

        vkDestroyImage(device, loadingImage, VK_NULL_HANDLE);
        logMessage("texture image destroyed");

        vkFreeMemory(device, loadingImageMem, VK_NULL_HANDLE);
        logMessage("texture image memory freed");

        vkDestroyImageView(device, textImageView, VK_NULL_HANDLE);
        logMessage("texture image view destroyed");

        vkDestroyImage(device, textImage, VK_NULL_HANDLE);
        logMessage("texture image destroyed");

        vkFreeMemory(device, textImageMem, VK_NULL_HANDLE);
        logMessage("texture image memory freed");

        destroyedFrameBuffer(&device, imageCount, swapchainFramebuffer);
        SDL_free(swapchainFramebuffer);
        logMessage("framebuffer destroyed");

        vkDestroyImageView(device, depthImageView, VK_NULL_HANDLE);
        logMessage("depth image view destroyed");

        vkDestroyImage(device, depthImage, VK_NULL_HANDLE);
        logMessage("depth image destroyed");

        vkFreeMemory(device, depthImageMemory, VK_NULL_HANDLE);
        logMessage("depth image memory freed");
        
        vkDestroyCommandPool(device, computeCommandPool, VK_NULL_HANDLE);
        logMessage("compute commandpool destroyed");

        vkDestroyCommandPool(device, swapchainCommandPool, VK_NULL_HANDLE);
        logMessage("commandpool destroyed");

        vkDestroyPipeline(device, computePipeline, VK_NULL_HANDLE);
        logMessage("compute pipeline destroyed");

        vkDestroyPipeline(device, particlePipeline, VK_NULL_HANDLE);
        logMessage("particle pipeline destroyed");

        vkDestroyPipeline(device, graphicPipeline, VK_NULL_HANDLE);
        logMessage("graphic pipelne destroyed");

        vkDestroyRenderPass(device, renderPass, VK_NULL_HANDLE);
        logMessage("renderPass destroyed");

        vkDestroyPipelineLayout(device, computePipelineLayout, VK_NULL_HANDLE);
        logMessage("compute pipeline layout destroyed");

        vkDestroyPipelineLayout(device, particlePipelineLayout, VK_NULL_HANDLE);
        logMessage("particle pipeline layout destroyed");

        vkDestroyPipelineLayout(device, graphicPipelineLayout, VK_NULL_HANDLE);
        logMessage("graphic pipeline layout destroyed");

        vkDestroyDescriptorSetLayout(device, *computeDescriptorSetLayout, VK_NULL_HANDLE);
        SDL_free(computeDescriptorSetLayout);
        SDL_free(computeBindings);
        logMessage("compute descriptor set layout destroyed");

        vkDestroyDescriptorSetLayout(device, *particleDescriptorSetLayout, VK_NULL_HANDLE);
        SDL_free(particleDescriptorSetLayout);
        SDL_free(particleBindings);
        logMessage("particle descriptoe set layout destroyed");

        vkDestroyDescriptorSetLayout(device, *graphicDescriptorSetLayout, VK_NULL_HANDLE);
        SDL_free(graphicDescriptorSetLayout);
        SDL_free(graphicBindings);
        logMessage("graphic descriptor set layout destroyed");

        vkDestroyShaderModule(device, compShaderCode, VK_NULL_HANDLE);
        SDL_free(computeShaderStageCreateInfo);
        logMessage("compute shader stage create info destroyed");

        vkDestroyShaderModule(device, particleFragmentShaderCode, VK_NULL_HANDLE);
        vkDestroyShaderModule(device, particleVertexShaderCode, VK_NULL_HANDLE);
        SDL_free(particleShaderStageCreateInfo);
        logMessage("particle shader stage create info destroyed");

        vkDestroyShaderModule(device, fragShaderCode, VK_NULL_HANDLE);
        vkDestroyShaderModule(device, vertShaderCode, VK_NULL_HANDLE);
        SDL_free(graphciShaderStageCreateInfo);
        logMessage("shaderCode destroyed");

        destroyImageViews(&device, swapchainImageViews, imageCount);
        SDL_free(swapchainImageViews);
        logMessage("swapchain image views destroyed");

        SDL_free(swapchainImages);
        logMessage("swapchainImages freed");

        vkDestroySwapchainKHR(device, swapchain, VK_NULL_HANDLE);
        logMessage("swapchain destroyed");

        vkDestroyDevice(device, VK_NULL_HANDLE);
        logMessage("device destroyed");

        //free(indices);
        logMessage("indices freed");

        vkDestroySurfaceKHR(instance, surface, VK_NULL_HANDLE);
        logMessage("surface destroyed");

        vkDestroyInstance(instance, VK_NULL_HANDLE);
        logMessage("instance destroyed");

        SDL_DestroyWindow(window);
        input_end = true;
        logMessage("window destroyed");
}