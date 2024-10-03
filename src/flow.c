#include "std_c.h"
#include "vulkan_code.h"
#include "judge.h"
#include "flow.h"
#include "glfw3.h"

#define KEY_OUTPUT 0

//declare a sdl window
SDL_Window * window = NULL;

//window's width and height
uint32_t width = 800;
uint32_t height = 600;

bool initWindow(void)
{
    if(!glfwInit())
        return false;
    /*initialize sdl
    timer, audio, video, event, joysitck, haptic, gamecontroller, sensor*/
    if (!SDL_Init(SDL_INIT_EVENTS)) 
    {
        //todo
        //add result juadge for sdl

        //show a messagebox for error informations
        int temp = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "", "Error initializing SDL.\n", NULL);
        if (!temp)
        {
            fprintf(stderr, "Error show messagebox\n");
            return false;
        }
        return false;
    }
    //create sdl window and sign window as a vulkan window
    window = SDL_CreateWindow("Vulkan", width, height, SDL_WINDOW_VULKAN);
    if (window == NULL)
        return false;
    debug_printf("window initialized");

    uint32_t iconWidth, iconHeight;
    iconWidth = iconHeight = 0;
    uint8_t iconChannel;
    png_bytep iconPixels = readPNG("Textures\\icon.png", &iconWidth, &iconHeight, &iconChannel);
    SDL_Surface * iconSurface = SDL_CreateSurfaceFrom(iconWidth, iconHeight, SDL_PIXELFORMAT_RGBA32, iconPixels, iconWidth * 4);
    if (iconSurface == NULL)
    {
        free(iconPixels);
        return false;
    }

    if (!SDL_SetWindowIcon(window, iconSurface))
        return false;

    SDL_DestroySurface(iconSurface);
    free(iconPixels);
    glfwTerminate();

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
static VkSwapchainKHR swapchain = VK_NULL_HANDLE;

static uint32_t imageCount = 0;
static VkImage * swapchainImages = VK_NULL_HANDLE;
static VkFormat swapchainFormat = 0;
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

static uint32_t graphicBinding = 0;
static VkDescriptorSetLayoutBinding * graphicBindings = VK_NULL_HANDLE;
static uint32_t graphicBindingCount = 0;
static VkPipelineLayout graphicPipelineLayout = VK_NULL_HANDLE;

static VkDescriptorSetLayoutBinding * particleBindings = VK_NULL_HANDLE;
static uint32_t particleBindingCount = 0;
static VkPipelineLayout particlePipelineLayout = VK_NULL_HANDLE;

static uint32_t computeBinding = 0;
static VkDescriptorSetLayoutBinding * computeBindings = VK_NULL_HANDLE;
static uint32_t computeBindingCount = 0;
static VkPipelineLayout computePipelineLayout = VK_NULL_HANDLE;

static VkSubpassDependency dependency = {};
static VkRenderPass renderPass = VK_NULL_HANDLE;

static VkPipeline graphicPipeline = VK_NULL_HANDLE;

static VkPipeline particlePipeline = VK_NULL_HANDLE;

static VkPipeline computePipeline = VK_NULL_HANDLE;

static VkCommandPool swapchainCommandPool = VK_NULL_HANDLE;

static VkCommandPool particleCommandPool = VK_NULL_HANDLE;

static VkCommandPool computeCommandPool = VK_NULL_HANDLE;

static VkImage depthImage = VK_NULL_HANDLE;
static VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
static VkImageView depthImageView = VK_NULL_HANDLE;
static VkFormat depthFormat = 0;

static VkFramebuffer * swapchainFramebuffer = VK_NULL_HANDLE;

static VkFramebuffer * particleFramebuffer = VK_NULL_HANDLE;

static VkImage texturesImage = VK_NULL_HANDLE;
static VkDeviceMemory textureImageMem = VK_NULL_HANDLE;
static VkImageView textureImageView = VK_NULL_HANDLE;
static VkSampler textureSampler = VK_NULL_HANDLE;

static VkBuffer vertexBuffer = VK_NULL_HANDLE;
static VkDeviceMemory vertexBufferMem = VK_NULL_HANDLE;
static uint32_t verticesCount = 8;
//three vertices of a triangle and color
static Vertex * vertices = NULL;

static VkBuffer indexBuffer = VK_NULL_HANDLE;
static VkDeviceMemory indexBufferMem = VK_NULL_HANDLE;
static uint32_t indicesCount = 12;
static uint16_t indices_v[12] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

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

static VkCommandBuffer * particleCommandBuffer = VK_NULL_HANDLE;

static VkCommandBuffer * computeCommandBuufer = VK_NULL_HANDLE;

static VkSemaphore * imageAvailableSemaphore = VK_NULL_HANDLE;
static VkSemaphore * renderFinishedSemaphore = VK_NULL_HANDLE;

static VkFence * inFlightFence = VK_NULL_HANDLE;

static VkSemaphore * particleRenderFinishedSemaphore = VK_NULL_HANDLE;

static VkFence * particleInFlightFence = VK_NULL_HANDLE;

static VkSemaphore * computeFinishedSemaphore = VK_NULL_HANDLE;
static VkFence * computeInFlightFences = VK_NULL_HANDLE;

static uint32_t currentFrame = 0;

static VkBuffer movingStagingBuffer = VK_NULL_HANDLE;
static VkDeviceMemory movingStagingMemory = VK_NULL_HANDLE;
static void * movingBufferMapped = VK_NULL_HANDLE;

static float camera_X = 0.0f;
static float camera_Y = 0.0f;

static float pictureX = 0;
static float pictureY = 0;

static bool moveEnabled = false;

static VkBuffer * shaderStorageBuffers = VK_NULL_HANDLE;
static VkDeviceMemory * shaderStorageBuffersMem = VK_NULL_HANDLE;

static Particle * particles = VK_NULL_HANDLE;

Recreate recreateSwap = {};

//store all compoents for initialize vulkan in a struct
VK_ALL allInOne = {};

void initVulkan(void)
{
    debug_printf("initializing...");

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

    //graphic shader
    createShaderModule(&device, "Shaders\\triangle_vertex.spv", &vertShaderCode);
    addShaderStageCreateInfo(&vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT, &graphicShaderCount, &graphciShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, 0, &graphicBindingCount, &graphicBindings);//0
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT, &graphicPoolSizeCount, &graphicDescriptorPoolSize);

    createShaderModule(&device, "Shaders\\triangle_fragment.spv", &fragShaderCode);
    addShaderStageCreateInfo(&fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT, &graphicShaderCount, &graphciShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1, &graphicBindingCount, &graphicBindings);//1
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT, &graphicPoolSizeCount, &graphicDescriptorPoolSize);

    addDescriptorSetLayout(&device, graphicBindingCount, graphicBindings, 0, &graphicDescriptorSetLayout);

    createPipelineLayout(&device, 1, &graphicDescriptorSetLayout, &graphicPipelineLayout);
    createGraphicsPipeline(&device, &extent2D, graphicShaderCount, graphciShaderStageCreateInfo, &graphicPipelineLayout, &renderPass, &graphicPipeline);

    createDescriptorPool(&device, graphicPoolSizeCount, graphicDescriptorPoolSize, 2, &graphicDescriptorPool);

    //particle shader
    createShaderModule(&device, "Shaders\\particle_vertex.spv", &particleVertexShaderCode);
    addShaderStageCreateInfo(&particleVertexShaderCode, VK_SHADER_STAGE_VERTEX_BIT, &particleShaderCount, &particleShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, 1, &particleBindingCount, &particleBindings);//1
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT, &particlePoolSizeCount, &particleDescriptorPoolSize);

    createShaderModule(&device, "Shaders\\particle_fragment.spv", &particleFragmentShaderCode);
    addShaderStageCreateInfo(&particleFragmentShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT, &particleShaderCount, &particleShaderStageCreateInfo);

    addDescriptorSetLayout(&device, particleBindingCount, particleBindings, 0, &particleDescriptorSetLayout);

    createPipelineLayout(&device, 1, &particleDescriptorSetLayout, &particlePipelineLayout);
    createParticlePipeline(&device, &extent2D, particleShaderCount, particleShaderStageCreateInfo, &particlePipelineLayout, &renderPass, &particlePipeline);

    createDescriptorPool(&device, particlePoolSizeCount, particleDescriptorPoolSize, 2, &particleDescriptorPool);

    //compute shader
    createShaderModule(&device, "Shaders\\particle_compute.spv", &compShaderCode);
    addShaderStageCreateInfo(&compShaderCode, VK_SHADER_STAGE_COMPUTE_BIT, &computeShaderCount, &computeShaderStageCreateInfo);
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1, 0, &computeBindingCount, &computeBindings);//0
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT, &computePoolSizeCount, &computeDescriptorPoolSize);

    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1, 1, &computeBindingCount, &computeBindings);//1
    setDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1, 2, &computeBindingCount, &computeBindings);//2
    setDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_FRAMES_IN_FLIGHT * 2, &computePoolSizeCount, &computeDescriptorPoolSize);

    addDescriptorSetLayout(&device, computeBindingCount, computeBindings, 0, &computeDescriptorSetLayout);

    createPipelineLayout(&device, 1, &computeDescriptorSetLayout, &computePipelineLayout);
    createComputePipeline(&device, &computePipelineLayout, computeShaderStageCreateInfo, &computePipeline);

    createDescriptorPool(&device, computePoolSizeCount, computeDescriptorPoolSize, 3, &computeDescriptorPool);


    createCommandPool(&device, indices.graphicsFamily, &swapchainCommandPool);

    createCommandPool(&device, indices.computeFamily, &computeCommandPool);

    createDepthResoures(&physicalDevice, &device, &extent2D, &swapchainCommandPool, &graphicQueue, &depthImage, &depthImageMemory, &depthImageView);

    createFrameBuffer(&device, &extent2D, imageCount, swapchainImageViews, &depthImageView, &renderPass, &swapchainFramebuffer);

    createTextureImage(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, "Textures\\5.png", &texturesImage, &textureImageMem);
    createTextureImageView(&device, &texturesImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, &textureImageView);
    createTextureSampler(&physicalDevice, &device, &textureSampler);

    vertices = (Vertex *)malloc(8 * sizeof(Vertex));
    vertices[0] = (Vertex){{-0.5f, -0.5f, 0.2f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}};
    vertices[1] = (Vertex){{0.5f, -0.5f, 0.2f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}};
    vertices[2] = (Vertex){{0.5f, 0.5f, 0.2f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};
    vertices[3] = (Vertex){{-0.5f, 0.5f, 0.2f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};

    vertices[4] = (Vertex){{-0.0f, -0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};
    vertices[5] = (Vertex){{1.0f, -0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}};
    vertices[6] = (Vertex){{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};
    vertices[7] = (Vertex){{-0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};

    positionInitialize(-200, -150, 400, 300, extent2D, &vertices, 0);
    positionInitialize(0, 0, 400, 300, extent2D, &vertices, 1);

    createVertexBuffer(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, &vertexBuffer, &vertexBufferMem, vertices, verticesCount);

    createIndexBuffer(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, &indexBuffer, &indexBufferMem, indices_v, indicesCount);

    initializeMovingBuffer(&physicalDevice, &device, &swapchainCommandPool, &graphicQueue, &movingStagingBuffer, &movingStagingMemory, &movingBufferMapped, vertices, verticesCount);

    createUniformBuffers(&physicalDevice, &device, &graphicUniformBuffers, &graphicUniformBuffersMemory, &graphicUniformBufferMapped);

    createUniformBuffers(&physicalDevice, &device, &computeUniformBuffers, &computeUniformBuffersmemory, &computeUniformBufferMapped);

    createShaderStorageBuffers(&physicalDevice, &device, &swapchainCommandPool, &computeQueue, extent2D, &shaderStorageBuffers, &shaderStorageBuffersMem, &particles);

    //graphics
    createGraphicDescriptorSets(&device, &graphicUniformBuffers, graphicDescriptorSetLayout, &graphicDescriptorPool, &graphicDescriptorSets, &textureImageView, &textureSampler);

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

    initializeRecreate();

    initializeAllInOne();

    resultVulkan(VK_SUCCESS, initializedF, 0);
}
void cleanup(FuncCode code)
{
    debug_printf("\nclean up");

        vkDeviceWaitIdle(device);

        vkUnmapMemory(device, movingStagingMemory);

        vkDestroyBuffer(device, movingStagingBuffer, VK_NULL_HANDLE);
        debug_printf("moving staging buffer destroyed");

        vkFreeMemory(device, movingStagingMemory, VK_NULL_HANDLE);
        debug_printf("moving staging buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyFence(device, computeInFlightFences[i], VK_NULL_HANDLE);
        }
        free(computeInFlightFences);
        debug_printf("compute in flight fences destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyFence(device, inFlightFence[i], VK_NULL_HANDLE);
        }
        free(inFlightFence);
        debug_printf("in flight fence destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, computeFinishedSemaphore[i], VK_NULL_HANDLE);
        }
        free(computeFinishedSemaphore);
        debug_printf("compute finished semaphore destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphore[i], VK_NULL_HANDLE);
        }
        free(renderFinishedSemaphore);
        debug_printf("render finished semaphore destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroySemaphore(device, imageAvailableSemaphore[i], VK_NULL_HANDLE);
        }
        free(imageAvailableSemaphore);
        debug_printf("image available semaphore destroyed");

        free(computeCommandBuufer);
        debug_printf("compute command buffer freed");

        free(commandBuffer);
        debug_printf("command buffer freed");

        vkDestroyDescriptorPool(device, computeDescriptorPool, VK_NULL_HANDLE);
        free(computeDescriptorPoolSize);
        debug_printf("compute descriptro pool destroyed");

        vkDestroyDescriptorPool(device, particleDescriptorPool, VK_NULL_HANDLE);
        free(particleDescriptorPoolSize);
        debug_printf("particle descriptor pool destroyed");

        vkDestroyDescriptorPool(device, graphicDescriptorPool, VK_NULL_HANDLE);
        free(graphicDescriptorPoolSize);
        debug_printf("graphic descriptor pool destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, computeUniformBuffersmemory[i]);
            vkDestroyBuffer(device, computeUniformBuffers[i], VK_NULL_HANDLE);
        }
        free(computeUniformBuffers);
        debug_printf("compute uniform buffers destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, computeUniformBuffersmemory[i], VK_NULL_HANDLE);
        }
        free(computeUniformBuffersmemory);
        free(computeUniformBufferMapped);
        debug_printf("compute uniform buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkDestroyBuffer(device, shaderStorageBuffers[i], VK_NULL_HANDLE);
        }
        free(shaderStorageBuffers);
        debug_printf("shader storage buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, shaderStorageBuffersMem[i], VK_NULL_HANDLE);
        }
        free(shaderStorageBuffersMem);
        debug_printf("shader storage buffer memory freed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkUnmapMemory(device, graphicUniformBuffersMemory[i]);
            vkDestroyBuffer(device, graphicUniformBuffers[i], VK_NULL_HANDLE);
        }
        free(graphicUniformBuffers);
        debug_printf("graphic uniform buffer destroyed");

        for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
        {
            vkFreeMemory(device, graphicUniformBuffersMemory[i], VK_NULL_HANDLE);
        }
        free(graphicUniformBuffersMemory);
        free(graphicUniformBufferMapped);
        debug_printf("graphic uniform buffer memory freed");

        vkDestroyBuffer(device, indexBuffer, VK_NULL_HANDLE);
        debug_printf("index buffer destroyed");

        vkFreeMemory(device, indexBufferMem, VK_NULL_HANDLE);
        debug_printf("index buffer memory freed");

        vkDestroyBuffer(device, vertexBuffer, VK_NULL_HANDLE);
        free(vertices);
        debug_printf("vertex buffer destroyed");

        vkFreeMemory(device, vertexBufferMem, VK_NULL_HANDLE);
        debug_printf("vertex buffer memory freed");

        vkDestroySampler(device, textureSampler, VK_NULL_HANDLE);
        debug_printf("texture sampler detroyed");

        vkDestroyImageView(device, textureImageView, VK_NULL_HANDLE);
        debug_printf("texture image view destroyed");

        vkDestroyImage(device, texturesImage, VK_NULL_HANDLE);
        debug_printf("texture image destroyed");

        vkFreeMemory(device, textureImageMem, VK_NULL_HANDLE);
        debug_printf("texture image memory freed");

        destroyedFrameBuffer(&device, imageCount, swapchainFramebuffer);
        free(swapchainFramebuffer);
        debug_printf("framebuffer destroyed");

        vkDestroyImageView(device, depthImageView, VK_NULL_HANDLE);
        debug_printf("depth image view destroyed");

        vkDestroyImage(device, depthImage, VK_NULL_HANDLE);
        debug_printf("depth image destroyed");

        vkFreeMemory(device, depthImageMemory, VK_NULL_HANDLE);
        debug_printf("depth image memory freed");
        
        vkDestroyCommandPool(device, computeCommandPool, VK_NULL_HANDLE);
        debug_printf("compute commandpool destroyed");

        vkDestroyCommandPool(device, swapchainCommandPool, VK_NULL_HANDLE);
        debug_printf("commandpool destroyed");

        vkDestroyPipeline(device, computePipeline, VK_NULL_HANDLE);
        debug_printf("compute pipeline destroyed");

        vkDestroyPipeline(device, particlePipeline, VK_NULL_HANDLE);
        debug_printf("particle pipeline destroyed");

        vkDestroyPipeline(device, graphicPipeline, VK_NULL_HANDLE);
        debug_printf("graphic pipelne destroyed");

        vkDestroyRenderPass(device, renderPass, VK_NULL_HANDLE);
        debug_printf("renderPass destroyed");

        vkDestroyPipelineLayout(device, computePipelineLayout, VK_NULL_HANDLE);
        debug_printf("compute pipeline layout destroyed");

        vkDestroyPipelineLayout(device, particlePipelineLayout, VK_NULL_HANDLE);
        debug_printf("particle pipeline layout destroyed");

        vkDestroyPipelineLayout(device, graphicPipelineLayout, VK_NULL_HANDLE);
        debug_printf("graphic pipeline layout destroyed");

        vkDestroyDescriptorSetLayout(device, *computeDescriptorSetLayout, VK_NULL_HANDLE);
        free(computeDescriptorSetLayout);
        free(computeBindings);
        debug_printf("compute descriptor set layout destroyed");

        vkDestroyDescriptorSetLayout(device, *particleDescriptorSetLayout, VK_NULL_HANDLE);
        free(particleDescriptorSetLayout);
        free(particleBindings);
        debug_printf("particle descriptoe set layout destroyed");

        vkDestroyDescriptorSetLayout(device, *graphicDescriptorSetLayout, VK_NULL_HANDLE);
        free(graphicDescriptorSetLayout);
        free(graphicBindings);
        debug_printf("graphic descriptor set layout destroyed");

        vkDestroyShaderModule(device, compShaderCode, VK_NULL_HANDLE);
        free(computeShaderStageCreateInfo);
        debug_printf("compute shader stage create info destroyed");

        vkDestroyShaderModule(device, particleFragmentShaderCode, VK_NULL_HANDLE);
        vkDestroyShaderModule(device, particleVertexShaderCode, VK_NULL_HANDLE);
        free(particleShaderStageCreateInfo);
        debug_printf("particle shader stage create info destroyed");

        vkDestroyShaderModule(device, fragShaderCode, VK_NULL_HANDLE);
        vkDestroyShaderModule(device, vertShaderCode, VK_NULL_HANDLE);
        free(graphciShaderStageCreateInfo);
        debug_printf("shaderCode destroyed");

        destroyImageViews(&device, swapchainImageViews, imageCount);
        free(swapchainImageViews);
        debug_printf("swapchain image views destroyed");

        free(swapchainImages);
        debug_printf("swapchainImages freed");

        vkDestroySwapchainKHR(device, swapchain, VK_NULL_HANDLE);
        debug_printf("swapchain destroyed");

        vkDestroyDevice(device, VK_NULL_HANDLE);
        debug_printf("device destroyed");

        //free(indices);
        debug_printf("indices freed");

        vkDestroySurfaceKHR(instance, surface, VK_NULL_HANDLE);
        debug_printf("surface destroyed");

        vkDestroyInstance(instance, VK_NULL_HANDLE);
        debug_printf("instance destroyed");

        SDL_DestroyWindow(window);
        debug_printf("window destroyed");

        SDL_Quit();
        debug_printf("SDL quited");
}
static inline void initializeRecreate(void)
{
    recreateSwap.DevicePack.pDevice = &device;
    recreateSwap.DevicePack.pPhysicalDevice = &physicalDevice;

    recreateSwap.pSurfaceCapabilities = &surfaceCapabilities;
    recreateSwap.pSurfaceFormat = &surfaceFormat;
    recreateSwap.pPresentMode = &presentMode;

    recreateSwap.pSurface = &surface;
    recreateSwap.pExtent2D = &extent2D;

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

    allInOne.pIndexBuffer = &indexBuffer;
    allInOne.pIndicesCount = &indicesCount;

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

    allInOne.pMovingStagingBuffer = &movingStagingBuffer;
    allInOne.ppMovingBufferMapped = &movingBufferMapped;

    allInOne.pCamera_X = &camera_X;
    allInOne.pCamera_Y = &camera_Y;

    allInOne.pPictureX = &pictureX;
    allInOne.pPictureY = &pictureY;
}

/*void mainLoop(void)
{
    debug_printf("\nmain loop");
    bool running = false;

    SDL_StopTextInput(window);

    bool stop = false;
    
    uint32_t preKeyState = 0;

    while (!running)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {   
            if (event.type == SDL_EVENT_MOUSE_MOTION | event.type == SDL_EVENT_MOUSE_BUTTON_DOWN | event.type == SDL_EVENT_MOUSE_BUTTON_UP)
                continue;
            if (preKeyState == SDL_EVENT_KEY_DOWN && event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                if (key == SDLK_RIGHT)
                {
                    *allInOne.pCamera_X -= 0.05f;
                }
                else if (key == SDLK_LEFT)
                {
                    *allInOne.pCamera_X += 0.05f;
                }
                else if (key == SDLK_UP)
                {
                    *allInOne.pCamera_Y -= 0.05f;
                }
                else if (key == SDLK_DOWN)
                {
                    *allInOne.pCamera_Y += 0.05f;
                }
                else if (key == SDLK_A)
                {
                    *allInOne.pPictureX -= 10;
                    *allInOne.pMoveEnabled = true;
                }
                else if (key == SDLK_D)
                {
                    *allInOne.pPictureX += 10;
                    *allInOne.pMoveEnabled = true;
                }
                else if (key == SDLK_W)
                {
                    *allInOne.pPictureY += 10;
                    *allInOne.pMoveEnabled = true;
                }
                else if (key == SDLK_S)
                {
                    *allInOne.pPictureY -= 10;
                    *allInOne.pMoveEnabled = true;
                }
            }

            if (preKeyState == SDL_EVENT_KEY_UP && event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                if (key == SDLK_RIGHT)
                {
                    *allInOne.pCamera_X -= 0.05f;
                }
                else if (key == SDLK_LEFT)
                {
                    *allInOne.pCamera_X += 0.05f;
                }
                else if (key == SDLK_UP)
                {
                    *allInOne.pCamera_Y -= 0.05f;
                }
                else if (key == SDLK_DOWN)
                {
                    *allInOne.pCamera_Y += 0.05f;
                }
                else if (key == SDLK_A)
                {
                    *allInOne.pPictureX -= 10;
                    *allInOne.pMoveEnabled = true;
                }
                else if (key == SDLK_D)
                {
                    *allInOne.pPictureX += 10;
                    *allInOne.pMoveEnabled = true;
                }
                else if (key == SDLK_W)
                {
                    *allInOne.pPictureY += 10;
                    *allInOne.pMoveEnabled = true;
                }
                else if (key == SDLK_S)
                {
                    *allInOne.pPictureY -= 10;
                    *allInOne.pMoveEnabled = true;
                }
            }


            switch(event.type)
            {
                case SDL_EVENT_QUIT:
                running = true;
                break;

                case SDL_EVENT_WINDOW_MINIMIZED:
                
                    SDL_MinimizeWindow(window);
                    stop = true;
                
                case SDL_EVENT_WINDOW_MAXIMIZED:
                
                    stop = false;
                
                


                case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    running = true;
                }
                else if (event.key.key == SDLK_F11)
                {
                    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                    allInOne.pExtent2D->width = 1600;
                    allInOne.pExtent2D->height = 900;
                    SDL_SetWindowSize(window, allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                    printf("sdl width: %u, height: %u\n", allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                }
                else if (event.key.key == SDLK_1)
                {
                    allInOne.pExtent2D->width++;
                    allInOne.pExtent2D->height++;
                    SDL_SetWindowSize(window, allInOne.pExtent2D->width, allInOne.pExtent2D->height);
                }
                break;
            }
            if (KEY_OUTPUT)
                printf("key:\npre: %d, now: %d\n", preKeyState, event.type);
            preKeyState = event.type;
        }
        if (!stop)
            drawFrame(&allInOne);
    }
}*/