#include "G_constants.h"
#include "G_allocator.h"

#include "vk_code_h/vk_computeShader.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"

#include "SDL3/SDL_stdinc.h"

extern VK_ALL allInOne;

static void initializeParticles(Particle ** ppParticles, VkExtent2D extent2D)
{
    SDL_srand(0);

    for (int i = 0;i < PARTICLE_COUNT;i++)
    {
        float r = 0.25f * SDL_sqrtf(SDL_randf());
        float theta = SDL_randf() * 2 * M_PI;

        float x = r * SDL_cosf(theta) * extent2D.height / extent2D.width;
        float y = r * SDL_sinf(theta);

        vec3 xy = {x, y, 0.0f};

        (*ppParticles)[i].position[0] = x;
        (*ppParticles)[i].position[1] = y;

        //float magnitude = sqrt(x * x + y * y);
        glm_normalize(xy);
        (*ppParticles)[i].velocity[0] = xy[0] * 0.25f;
        (*ppParticles)[i].velocity[1] = xy[1] * 0.25f;

        (*ppParticles)[i].color[0] = SDL_randf();
        (*ppParticles)[i].color[1] = SDL_randf();
        (*ppParticles)[i].color[2] = SDL_randf();
        (*ppParticles)[i].color[3] = 1.0f;
    }
}
void createShaderStorageBuffers(VkBuffer (*ppShaderStorageBuffers)[2], VkDeviceMemory (*ppShaderStorageBuffersMem)[2])
{
    Particle * particles = NULL;
    particles = (Particle *)G_malloc(PARTICLE_COUNT * sizeof(Particle));
    initializeParticles(&particles, allInOne.extent2D);

    VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

    VkBuffer stagingBuffer = NULL;
    VkDeviceMemory stagingBufferMemory = NULL;
    createBuffer(&stagingBuffer, &stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, NULL, 0, 0);

    void * data = NULL;
    vkMapMemory(allInOne.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, particles, bufferSize);
    vkUnmapMemory(allInOne.device, stagingBufferMemory);

    createBuffer((*ppShaderStorageBuffers) + 0, (*ppShaderStorageBuffersMem) + 0, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, NULL, 0, 0);
    copyBuffer(NULL, allInOne.computeCommandPool, &stagingBuffer, (*ppShaderStorageBuffers) + 0, bufferSize);

    createBuffer((*ppShaderStorageBuffers) + 1, (*ppShaderStorageBuffersMem) + 1, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, NULL, 0, 0);
    copyBuffer(NULL, allInOne.graphicCommandPool, &stagingBuffer, (*ppShaderStorageBuffers) + 1, bufferSize);
    releaseBufferFromQueue(allInOne.graphicCommandPool, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.queueFamilyIndices.computeFamily.familyIndice\
    , (*ppShaderStorageBuffers)[1], bufferSize);

    vkDestroyBuffer(allInOne.device, stagingBuffer, allInOne.pAllocationCallbacks);
    vkFreeMemory(allInOne.device, stagingBufferMemory, allInOne.pAllocationCallbacks);
    G_free(particles);
}