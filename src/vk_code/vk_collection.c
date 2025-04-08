#include "vk_code_h/vk_collection.h"

#include "vk_code_h/vk_all_struct.h"

static VK_COLLECTION CO;

extern VK_ALL allInOne;

void initCollection(void)
{
    CO.windowCount = 0;

    CO.instance = NULL;

    CO.surfaceCount = 0;

    CO.device = NULL;

    CO.swapchainCount = 0;

    CO.swapchainImageMem[0] = NULL;
    CO.swapchainImageMem[1] = NULL;
    CO.swapchainImageMemCount = 0;

    CO.swapchainImageViews = NULL;
    CO.swapchainImageViewCount = 0;
    CO.swapchainImageViewMem = NULL;
    CO.swapchainImageViewMemCount = 0;

    CO.renderPasses = NULL;
    CO.renderPassCount = 0;

    CO.commandPools = NULL;
    CO.commandPoolCount = 0;

    CO.frameBuffers = NULL;
    CO.frameBufferCount = 0;
    CO.frameBufferMem = NULL;
    CO.frameBufferMemCount = 0;

    CO.samplers = NULL;
    CO.samplerCount = 0;

    CO.shaderModules = NULL;
    CO.shaderModuleCount = 0;

    CO.shaderStageCreateInfos = NULL;
    CO.shaderStageCreateInfoCount = 0;

    CO.descriptorSetLayouts = NULL;
    CO.descriptorSetLayoutCount = 0;
    CO.descriptorSetLayoutMem = NULL;
    CO.descriptorSetLayoutMemCount = 0;

    CO.pipelineLayouts = NULL;
    CO.pipelineLayoutCount = 0;

    CO.pipelines = NULL;
    CO.pipelineCount = 0;

    CO.descriptorPool = NULL;
    CO.descriptorPoolCount = 0;

    CO.semaphores = NULL;
    CO.semaphoreCount = 0;

    CO.fences = NULL;
    CO.fenceCount = 0;

    CO.buffers = NULL;
    CO.bufferCount = 0;
}
bool CO_addWindow(SDL_Window * window)
{
    if (CO.windowCount == 2) return false;

    CO.windows[CO.windowCount] = window;
    CO.windowCount++;

    return true;
}
bool CO_addInstance(VkInstance instance)
{
    if (CO.instance == NULL) CO.instance = instance;
    else return false;

    return true;
}
bool CO_addSurface(VkSurfaceKHR surface)
{
    if (CO.surfaceCount == 2) return false;

    CO.surface[CO.surfaceCount] = surface;
    CO.surfaceCount++;

    return true;
}
bool CO_addDevice(VkDevice device)
{
    if (CO.device == NULL) CO.device = device;
    else return false;

    return true;
}
bool CO_addSwapchain(VkSwapchainKHR swapchain)
{
    if (CO.swapchainCount == 2) return false;

    CO.swapchains[CO.swapchainCount] = swapchain;
    CO.swapchainCount++;

    return true;
}
bool CO_addSwapchainImage(void * swapchainImage)
{
    if (CO.swapchainImageMemCount == 2) return false;

    CO.swapchainImageMem[CO.swapchainImageMemCount] = swapchainImage;
    CO.swapchainImageMemCount++;

    return true;
}
bool CO_addSwapchainImageView(Uint32 count, VkImageView * swapchainImageView)
{
    void * ptr1, * ptr2;
    Uint32 i;
    ptr1 = SDL_realloc(CO.swapchainImageViews, (CO.swapchainImageViewCount + count) * sizeof(VkImageView));
    if (ptr1 == NULL) return false;

    ptr2 = SDL_realloc(CO.swapchainImageViewMem, (CO.swapchainImageViewMemCount + 1) * sizeof(void*));
    if (ptr2 == NULL) return false;

    CO.swapchainImageViews = ptr1;

    for (i = 0;i < count;i++)
    {
        CO.swapchainImageViews[CO.swapchainImageViewCount + i] = swapchainImageView[i];
    }
    CO.swapchainImageViewCount += count;

    CO.swapchainImageViewMem = ptr2;
    CO.swapchainImageViewMem[CO.swapchainImageViewMemCount] = swapchainImageView;
    CO.swapchainImageViewMemCount++;

    return true;
}
bool CO_addRenderPass(VkRenderPass renderPass)
{
    void * ptr;
    ptr = SDL_realloc(CO.renderPasses, (CO.renderPassCount + 1) * sizeof(VkRenderPass));
    if (ptr == NULL) return false;

    CO.renderPasses = ptr;

    CO.renderPasses[CO.renderPassCount] = renderPass;
    CO.renderPassCount++;

    return true;
}
bool CO_addFrameBuffer(Uint32 count, VkFramebuffer * pFrameBuffer)
{
    void * ptr1, * ptr2;
    Uint32 i;
    ptr1 = SDL_realloc(CO.frameBuffers, (CO.frameBufferCount + count) * sizeof(VkFramebuffer));
    if (ptr1 == NULL) return false;

    ptr2 = SDL_realloc(CO.frameBufferMem, (CO.frameBufferMemCount + 1) * sizeof(void*));
    if (ptr2 == NULL) return false;

    CO.frameBuffers = ptr1;
    for (i = 0;i < count;i++)
    {
        CO.frameBuffers[CO.frameBufferCount + i] = pFrameBuffer[i];
    }
    CO.frameBufferCount += count;

    CO.frameBufferMem = ptr2;
    CO.frameBufferMem[CO.frameBufferMemCount] = pFrameBuffer;
    CO.frameBufferMemCount++;

    return true;
}
bool CO_addCommandPool(VkCommandPool commandPool)
{
    void * ptr;
    ptr = SDL_realloc(CO.commandPools, (CO.commandPoolCount + 1) * sizeof(VkCommandPool));
    if (ptr == NULL) return false;

    CO.commandPools = ptr;

    CO.commandPools[CO.commandPoolCount] = commandPool;
    CO.commandPoolCount++;

    return true;
}
bool CO_addSampler(VkSampler sampler)
{
    void * ptr;
    ptr = SDL_realloc(CO.samplers, (CO.samplerCount + 1) * sizeof(VkSampler));
    if (ptr == NULL) return NULL;

    CO.samplers = ptr;

    CO.samplers[CO.samplerCount] = sampler;
    CO.samplerCount++;

    return true;
}
bool CO_addShaderModule(VkShaderModule shaderModule)
{
    void * ptr;
    ptr = SDL_realloc(CO.shaderModules, (CO.shaderModuleCount + 1) * sizeof(VkShaderModule));
    if (ptr == NULL) return false;

    CO.shaderModules = ptr;

    CO.shaderModules[CO.shaderModuleCount] = shaderModule;
    CO.shaderModuleCount++;

    return true;
}
bool CO_addShaderStageCreateInfo(VkPipelineShaderStageCreateInfo * pShaderStageCreateInfo)
{
    void * ptr;
    ptr = SDL_realloc(CO.shaderStageCreateInfos, (CO.shaderStageCreateInfoCount + 1) * sizeof(VkPipelineShaderStageCreateInfo*));
    if (ptr == NULL) return false;

    CO.shaderStageCreateInfos = ptr;

    CO.shaderStageCreateInfos[CO.shaderStageCreateInfoCount] = pShaderStageCreateInfo;
    CO.shaderStageCreateInfoCount++;

    return true;
}
bool CO_addDescriptorSetLayout(Uint32 count, VkDescriptorSetLayout * pDescriptorSetLayout)
{
    void * ptr1, * ptr2;
    Uint32 i;
    ptr1 = SDL_realloc(CO.descriptorSetLayouts, (CO.descriptorSetLayoutCount + count) * sizeof(VkDescriptorSetLayout));
    if (ptr1 == NULL) return false;

    ptr2 = SDL_realloc(CO.descriptorSetLayoutMem, (CO.descriptorSetLayoutMemCount + 1) * sizeof(void*));
    if (ptr2 == NULL) return false;

    CO.descriptorSetLayouts = ptr1;
    for (i = 0;i < count;i++)
    {
        CO.descriptorSetLayouts[CO.descriptorSetLayoutCount + i] = pDescriptorSetLayout[i];
    }
    CO.descriptorSetLayoutCount += count;

    CO.descriptorSetLayoutMem = ptr2;
    CO.descriptorSetLayoutMem[CO.descriptorSetLayoutMemCount] = pDescriptorSetLayout;
    CO.descriptorSetLayoutMemCount++;

    return true;
}
bool CO_addPieplineLayout(VkPipelineLayout pipelineLayout)
{
    void * ptr;
    ptr = SDL_realloc(CO.pipelineLayouts, (CO.pipelineLayoutCount + 1) * sizeof(VkPipelineLayout));
    if (ptr == NULL) return false;

    CO.pipelineLayouts = ptr;

    CO.pipelineLayouts[CO.pipelineLayoutCount] = pipelineLayout;
    CO.pipelineLayoutCount++;

    return true;
}
bool CO_addPiepline(VkPipeline pipeline)
{
    void * ptr;
    ptr = SDL_realloc(CO.pipelines, (CO.pipelineCount + 1) * sizeof(VkPipeline));
    if (ptr == NULL) return false;

    CO.pipelines = ptr;

    CO.pipelines[CO.pipelineCount] = pipeline;
    CO.pipelineCount++;

    return true;
}
bool CO_addDescriptorPool(VkDescriptorPool descriptorPool)
{
    void * ptr;
    ptr = SDL_realloc(CO.descriptorPool, (CO.descriptorPoolCount + 1) * sizeof(VkDescriptorPool));
    if (ptr == NULL) return false;

    CO.descriptorPool = ptr;

    CO.descriptorPool[CO.descriptorPoolCount] = descriptorPool;
    CO.descriptorPoolCount++;

    return true;
}
bool CO_addBuffer(bool mapped, VkBuffer buffer, VkDeviceMemory bufferMemory, void* cpuMem)
{
    void * ptr;
    ptr = SDL_realloc(CO.buffers, (CO.bufferCount + 1) * sizeof(BUFFER_PACK));
    if (ptr == NULL) return false;

    CO.buffers = ptr;
    CO.buffers[CO.bufferCount].buffer = buffer;
    CO.buffers[CO.bufferCount].bufferMemory = bufferMemory;
    CO.buffers[CO.bufferCount].cpuMem = cpuMem;
    CO.buffers[CO.bufferCount].mapped = mapped;
    CO.bufferCount++;

    return true;
}
bool CO_addImageView(VkImageView imageView)
{
    void * ptr;
    ptr = SDL_realloc(CO.imageViews, (CO.imageViewCount + 1) * sizeof(VkImageView));
    if (ptr == NULL) return false;

    CO.imageViews = ptr;

    CO.imageViews[CO.imageViewCount] = imageView;
    CO.imageViewCount++;

    return true;
}
bool CO_addSemaphore(VkSemaphore semaphore)
{
    void * ptr;
    ptr = SDL_realloc(CO.semaphores, (CO.semaphoreCount + 1) * sizeof(VkSemaphore));
    if (ptr == NULL) return false;

    CO.semaphores = ptr;

    CO.semaphores[CO.semaphoreCount] = semaphore;
    CO.semaphoreCount++;

    return true;
}
bool CO_addFence(VkFence fence)
{
    void * ptr;
    ptr = SDL_realloc(CO.fences, (CO.fenceCount + 1) * sizeof(VkFence));
    if (ptr == NULL) return false;

    CO.fences = ptr;

    CO.fences[CO.fenceCount] = fence;
    CO.fenceCount++;

    return true;
}
bool CO_cleanFramebuffer(Uint32 count, VkFramebuffer * framebuffer)
{
    Uint32 i, j;
    for (i = 0;i < CO.frameBufferCount;i++)
    {
        if (CO.frameBuffers[i] == *framebuffer)
        {
            for (j = 0;j < count;j++)
            {
                vkDestroyFramebuffer(CO.device, CO.frameBuffers[i + j], allInOne.pAllocationCallbacks);
            }
            memcpy(CO.frameBuffers + i, CO.frameBuffers + i + count, (CO.frameBufferCount - i - count) * sizeof(VkFramebuffer));
            break;
        }
    }

    if (i == CO.frameBufferCount) return false;
    CO.frameBufferCount -= count;

    for (i = 0;i < CO.frameBufferMemCount;i++)
    {
        if (CO.frameBufferMem[i] == framebuffer)
        {
            SDL_free(CO.frameBufferMem[i]);
            CO.frameBufferMem[i] = CO.frameBufferMem[CO.frameBufferMemCount - 1];
            CO.frameBufferMemCount--;
            return true;
        }
    }
    return false;
}
bool CO_cleanSwapchainImageView(Uint32 count, VkImageView * swapchainImageView)
{
    Uint32 i, j;
    for (i = 0;i < CO.swapchainImageViewCount;i++)
    {
        if (CO.swapchainImageViews[i] == *swapchainImageView)
        {
            for (j = 0;j < count;j++)
            {
                vkDestroyImageView(CO.device, CO.swapchainImageViews[i + j], allInOne.pAllocationCallbacks);
            }
            memcpy(CO.swapchainImageViews + i, CO.swapchainImageViews + i + count, (CO.swapchainImageViewCount - i - count) * sizeof(VkImageView));
            break;
        }
    }

    if (i == CO.swapchainImageViewCount) return false;
    CO.swapchainImageViewCount -= count;

    for (i = 0;i < CO.swapchainImageViewMemCount;i++)
    {
        if (CO.swapchainImageViewMem[i] == swapchainImageView)
        {
            SDL_free(CO.swapchainImageViewMem[i]);
            CO.swapchainImageViewMem[i] = CO.swapchainImageViewMem[CO.swapchainImageViewMemCount - 1];
            CO.swapchainImageViewMemCount--;
            return true;
        }
    }
    return false;
}
bool CO_cleanSwapchainImage(void * imageMem)
{
    Uint32 i;
    for (i = 0;i < CO.swapchainImageMemCount;i++)
    {
        if (CO.swapchainImageMem[i] == imageMem)
        {
            SDL_free(CO.swapchainImageMem[i]);
            CO.swapchainImageMem[i] = CO.swapchainImageMem[CO.swapchainImageMemCount - 1];
            CO.swapchainImageMemCount--;
            return true;
        }
    }
    return false;
}
bool CO_cleanSwapchain(VkSwapchainKHR swapchain)
{
    Uint32 i;
    for (i = 0;i < CO.swapchainCount;i++)
    {
        if (CO.swapchains[i] == swapchain)
        {
            vkDestroySwapchainKHR(CO.device, CO.swapchains[i], allInOne.pAllocationCallbacks);
            CO.swapchains[i] = CO.swapchains[CO.swapchainCount - 1];
            CO.swapchainCount--;
            return true;
        }
    }
    return false;
}
void CO_CleanAllVkResource(void)
{
    Uint32 i;

    if (CO.device != NULL)
    {
        if (CO.bufferCount)
        {
            for (i = 0;i < CO.bufferCount;i++)
            {
                if (CO.buffers[i].mapped)
                {
                    vkUnmapMemory(CO.device, CO.buffers[i].bufferMemory);
                }
                vkDestroyBuffer(CO.device, CO.buffers[i].buffer, allInOne.pAllocationCallbacks);
                vkFreeMemory(CO.device, CO.buffers[i].bufferMemory, allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.buffers);
            CO.buffers = NULL;
            CO.bufferCount = 0;
        }

        if (CO.imageViewCount)
        {
            for (i = 0;i < CO.imageViewCount;i++)
            {
                vkDestroyImageView(CO.device, CO.imageViews[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.imageViews);
            CO.imageViews = NULL;
            CO.imageViewCount = 0;
        }

        if (CO.swapchainCount)
        {
            if (CO.swapchainImageViewCount)
            {
                for (i = 0;i < CO.swapchainImageViewCount;i++)
                {
                    vkDestroyImageView(CO.device, CO.swapchainImageViews[i], allInOne.pAllocationCallbacks);
                }
                SDL_free(CO.swapchainImageViews);
                CO.swapchainImageViews = NULL;
                CO.swapchainImageViewCount = 0;
            }

            if (CO.swapchainImageMemCount)
            {
                for (i = 0;i < CO.swapchainImageViewMemCount;i++)
                {
                    SDL_free(CO.swapchainImageViewMem[i]);
                }
                SDL_free(CO.swapchainImageViewMem);
                CO.swapchainImageViewMem = NULL;
                CO.swapchainImageViewMemCount = 0;
            }   

            for (i = 0;i < CO.swapchainCount;i++)
            {
                vkDestroySwapchainKHR(CO.device, CO.swapchains[i], allInOne.pAllocationCallbacks);
            }
            CO.swapchainCount = 0;
        }

        if (CO.renderPassCount)
        {
            if (CO.frameBufferCount)
            {
                for (i = 0;i < CO.frameBufferCount;i++)
                {
                    vkDestroyFramebuffer(CO.device, CO.frameBuffers[i], allInOne.pAllocationCallbacks);
                }
                SDL_free(CO.frameBuffers);
                CO.frameBuffers = NULL;
                CO.frameBufferCount = 0;
                for (i = 0;i < CO.frameBufferMemCount;i++)
                {
                    SDL_free(CO.frameBufferMem[i]);
                }
                SDL_free(CO.frameBufferMem);
                CO.frameBufferMem = NULL;
                CO.frameBufferMemCount = 0;
            }

            for (i = 0;i < CO.renderPassCount;i++)
            {
                vkDestroyRenderPass(CO.device, CO.renderPasses[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.renderPasses);
            CO.renderPasses = NULL;
            CO.renderPassCount = 0;
        }

        if (CO.shaderModuleCount)
        {
            if (CO.shaderStageCreateInfoCount)
            {
                for (i = 0;i < CO.shaderStageCreateInfoCount;i++)
                {
                    SDL_free(CO.shaderStageCreateInfos[i]);
                }
                SDL_free(CO.shaderStageCreateInfos);
                CO.shaderStageCreateInfos = NULL;
                CO.shaderStageCreateInfoCount = 0;
            }

            if (CO.descriptorSetLayoutCount)
            {
                if (CO.pipelineLayoutCount)
                {
                    if (CO.pipelineCount)
                    {
                        for (i = 0;i < CO.pipelineCount;i++)
                        {
                            vkDestroyPipeline(CO.device, CO.pipelines[i], allInOne.pAllocationCallbacks);
                        }
                        SDL_free(CO.pipelines);
                        CO.pipelines = NULL;
                        CO.pipelineCount = 0;
                    }

                    for (i = 0;i < CO.pipelineLayoutCount;i++)
                    {
                        vkDestroyPipelineLayout(CO.device, CO.pipelineLayouts[i], allInOne.pAllocationCallbacks);
                    }
                    SDL_free(CO.pipelineLayouts);
                    CO.pipelineLayouts = NULL;
                    CO.pipelineLayoutCount = 0;
                }

                for (i = 0;i < CO.descriptorSetLayoutCount;i++)
                {
                    vkDestroyDescriptorSetLayout(CO.device, CO.descriptorSetLayouts[i], allInOne.pAllocationCallbacks);
                }
                SDL_free(CO.descriptorSetLayouts);
                CO.descriptorSetLayouts = NULL;
                CO.descriptorSetLayoutCount = 0;
                for (i = 0;i < CO.descriptorSetLayoutMemCount;i++)
                {
                    SDL_free(CO.descriptorSetLayoutMem[i]);
                }
                SDL_free(CO.descriptorSetLayoutMem);
                CO.descriptorSetLayoutMem = NULL;
                CO.descriptorSetLayoutMemCount = 0;
            }

            for (i = 0;i < CO.shaderModuleCount;i++)
            {
                vkDestroyShaderModule(CO.device, CO.shaderModules[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.shaderModules);
            CO.shaderModules = NULL;
            CO.shaderModuleCount = 0;
        }

        if (CO.samplerCount)
        {
            for (i = 0;i < CO.samplerCount;i++)
            {
                vkDestroySampler(CO.device, CO.samplers[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.samplers);
            CO.samplers = NULL;
            CO.samplerCount = 0;
        }

        if (CO.descriptorPoolCount)
        {
            for (i = 0;i < CO.descriptorPoolCount;i++)
            {
                vkDestroyDescriptorPool(CO.device, CO.descriptorPool[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.descriptorPool);
            CO.descriptorPool = NULL;
            CO.descriptorPoolCount = 0;
        }

        if (CO.semaphoreCount)
        {
            for (i = 0;i < CO.semaphoreCount;i++)
            {
                vkDestroySemaphore(CO.device, CO.semaphores[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.semaphores);
            CO.semaphores = NULL;
            CO.semaphoreCount = 0;
        }

        if (CO.fenceCount)
        {
            for (i = 0;i < CO.fenceCount;i++)
            {
                vkDestroyFence(CO.device, CO.fences[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.fences);
            CO.fences = NULL;
            CO.fenceCount = 0;
        }

        if (CO.commandPoolCount)
        {
            for (i = 0;i < CO.commandPoolCount;i++)
            {
                vkDestroyCommandPool(CO.device, CO.commandPools[i], allInOne.pAllocationCallbacks);
            }
            SDL_free(CO.commandPools);
            CO.commandPools = NULL;
            CO.commandPoolCount = 0;
        }

        vkDestroyDevice(CO.device, allInOne.pAllocationCallbacks);
        CO.device = NULL;
    }

    if (CO.instance != NULL)
    {
        if (CO.surfaceCount)
        {
            for (i = 0;i < CO.surfaceCount;i++)
            {
                vkDestroySurfaceKHR(CO.instance, CO.surface[i], allInOne.pAllocationCallbacks);
            }
        }

        vkDestroyInstance(CO.instance, allInOne.pAllocationCallbacks);
        CO.instance = NULL;
    }

    if (CO.windowCount)
    {
        for (i = 0;i < CO.windowCount;i++)
        {
            SDL_DestroyWindow(CO.windows[i]);
        }
        CO.windowCount = 0;
    }
}