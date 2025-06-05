#include "G_VkCommandPool.h"
#include "SDL_mutex.h"
#include "vk_code_h/vk_synchronize.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_synchronize.h"
#include "vulkan_core.h"

extern VK_ALL allInOne;

void G_CreateCommandPool(VkCommandPoolCreateFlags flag, Uint32 queueFamilyIndex, G_CommandPool * pCommandPool)
{
    createCommandPool(flag, queueFamilyIndex, &pCommandPool->commandPool);
    pCommandPool->queueFamilyIndex = queueFamilyIndex;
}
void G_CreateThreadCommandPool(G_CommandPool * pGraphicCommandPool, G_CommandPool * pComputeComandPool, G_ThreadCommandPool * pThreadCommandPool)
{
    if (pThreadCommandPool == NULL) return;

    if (pGraphicCommandPool) createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pGraphicCommandPool->commandPool, pThreadCommandPool->pGraphicCommandBuffer, MAX_FRAMES_IN_FLIGHT);
    if (pComputeComandPool) 
    {
        createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pComputeComandPool->commandPool, pThreadCommandPool->pComputeCommandBuffer, MAX_FRAMES_IN_FLIGHT);
        createNormalFences(pThreadCommandPool->pComputeFence, MAX_FRAMES_IN_FLIGHT);
    }

    pThreadCommandPool->pGraphicCommandPool = pGraphicCommandPool;
    pThreadCommandPool->pComputeCommandPool = pComputeComandPool;

    createNormalFences(pThreadCommandPool->pFence, MAX_FRAMES_IN_FLIGHT);
    createTimelineSemaphore(0, pThreadCommandPool->pSemaphore, MAX_FRAMES_IN_FLIGHT);
    createNormalSemaphore(pThreadCommandPool->pThreadDoneSemaphore, MAX_FRAMES_IN_FLIGHT);
    pThreadCommandPool->mutex = SDL_CreateMutex();
}
void G_DestroyThreadCommandPool(G_ThreadCommandPool * pThreadCommandPool)
{
    if (pThreadCommandPool->pGraphicCommandPool) vkFreeCommandBuffers(allInOne.device, pThreadCommandPool->pGraphicCommandPool->commandPool, MAX_FRAMES_IN_FLIGHT, pThreadCommandPool->pGraphicCommandBuffer);
    if (pThreadCommandPool->pComputeCommandPool) 
    {
        vkFreeCommandBuffers(allInOne.device, pThreadCommandPool->pComputeCommandPool->commandPool, MAX_FRAMES_IN_FLIGHT, pThreadCommandPool->pComputeCommandBuffer);
        destroyFence(pThreadCommandPool->pComputeFence, MAX_FRAMES_IN_FLIGHT);
    }

    destroyFence(pThreadCommandPool->pFence, MAX_FRAMES_IN_FLIGHT);
    destroySemaphore(pThreadCommandPool->pSemaphore, MAX_FRAMES_IN_FLIGHT);
    destroySemaphore(pThreadCommandPool->pThreadDoneSemaphore, MAX_FRAMES_IN_FLIGHT);
    SDL_DestroyMutex(pThreadCommandPool->mutex);
}
