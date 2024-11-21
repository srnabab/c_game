#include "vk_computeShader.h"
#include <time.h>
#include "vk_buffer.h"

void createShaderStorageBuffers(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkCommandPool * pCommandPool, VkQueue * pGraphicQueue, VkExtent2D extent2D, VkBuffer ** ppShaderStorageBuffers, VkDeviceMemory ** ppShaderStorageBuffersMem, Particle ** ppParticles)
{
    VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

    initializeParticles(ppParticles, extent2D);

    *ppShaderStorageBuffers = (VkBuffer *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkBuffer));
    *ppShaderStorageBuffersMem = (VkDeviceMemory *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkDeviceMemory));

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    createBuffer(pPhysicalDevice, pDevice, &stagingBuffer, &stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void * data = VK_NULL_HANDLE;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, *ppParticles, bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);

    for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
    {
        createBuffer(pPhysicalDevice, pDevice, &(*ppShaderStorageBuffers)[i], &(*ppShaderStorageBuffersMem)[i], bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        copyBuffer(&stagingBuffer, &(*ppShaderStorageBuffers)[i], bufferSize, pDevice, pCommandPool, pGraphicQueue);

        //printf("result1: %d, result2: %d\n", result1, result2);
    }

    vkDestroyBuffer(*pDevice, stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(*pDevice, stagingBufferMemory, VK_NULL_HANDLE);
}
float randomFloat(void)
{
    return rand() / (float)RAND_MAX;
}
void initializeParticles(Particle ** ppParticles, VkExtent2D extent2D)
{
    *ppParticles = (Particle *)SDL_malloc(PARTICLE_COUNT * sizeof(Particle));
    srand((uint32_t)time(NULL));

    for (int i = 0;i < PARTICLE_COUNT;i++)
    {
        float r = 0.25f * sqrt(randomFloat());
        float theta = randomFloat() * 2 * M_PI;

        float x = r * cos(theta) * extent2D.height / extent2D.width;
        float y = r * sin(theta);

        vec3 xy = {x, y, 0.0f};

        (*ppParticles)[i].position[0] = x;
        (*ppParticles)[i].position[1] = y;

        //float magnitude = sqrt(x * x + y * y);
        glm_normalize(xy);
        (*ppParticles)[i].velocity[0] = xy[0] * 0.25f;
        (*ppParticles)[i].velocity[1] = xy[1] * 0.25f;

        (*ppParticles)[i].color[0] = randomFloat();
        (*ppParticles)[i].color[1] = randomFloat();
        (*ppParticles)[i].color[2] = randomFloat();
        (*ppParticles)[i].color[3] = 1.0f;
    }
}