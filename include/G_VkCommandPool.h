#include "SDL3/SDL_stdinc.h"
#include "G_constants.h"
#include "SDL_mutex.h"
#include "vk_code_h/vk_commandPool.h"

#ifndef G_VKCOMMANDPOOL_H
#define G_VKCOMMANDPOOL_H 1

struct _G_CommandPool
{
    VkCommandPool commandPool;
    Uint32 queueFamilyIndex;
};
typedef struct _G_CommandPool G_CommandPool;

struct _G_ThreadCommandPool
{
    G_CommandPool * pGraphicCommandPool;
    VkCommandBuffer pGraphicCommandBuffer[MAX_FRAMES_IN_FLIGHT];
    G_CommandPool * pComputeCommandPool;
    VkCommandBuffer pComputeCommandBuffer[MAX_FRAMES_IN_FLIGHT];
    VkFence pFence[MAX_FRAMES_IN_FLIGHT];
    VkFence pComputeFence[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore pSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore pThreadDoneSemaphore[MAX_FRAMES_IN_FLIGHT];
    SDL_Mutex * mutex;
};
typedef struct _G_ThreadCommandPool G_ThreadCommandPool;

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL G_CreateCommandPool(VkCommandPoolCreateFlags flag, Uint32 queueFamilyIndex, G_CommandPool * pCommandPool);
extern void SDLCALL G_CreateThreadCommandPool(G_CommandPool * pGraphicCommandPool, G_CommandPool * pComputeComandPool, G_ThreadCommandPool * pThreadCommandPool);
extern void SDLCALL G_DestroyThreadCommandPool(G_ThreadCommandPool * pThreadCommandPool);

#include "SDL3/SDL_close_code.h"

#endif // G_VkCommandPool.h
