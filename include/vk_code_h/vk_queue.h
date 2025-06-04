#include "vk_struct.h"
#include "SDL3/SDL_mutex.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H 1

#define GRAPHIC_QUEUE_COUNT 2
#define COMPUTE_QUEUE_COUNT 2
#define TRANSFER_QUEUE_COUNT 1
#define PRESENT_QUEUE_COUNT 1

#define GRAPHIC_3D_QUEUE 0
#define GRAPHIC_2D_QUEUE 1

#define COMPUTE_QUEUE 0

#define TRANSFER_QUEUE 0

#define PRESENT_QUEUE 0

struct _G_VkQueue
{
    VkQueue queue;
    SDL_Mutex * mutex;
};
typedef struct _G_VkQueue G_VkQueue;

#include "SDL3/SDL_begin_code.h"

extern bool SDLCALL findQueueFamilies(void);
extern void SDLCALL createQueue(void);
extern G_VkQueue * SDLCALL getFirstQueueByIndex(Uint32 index);
extern VkResult SDLCALL G_vkQueueSubmit(G_VkQueue * queue, Uint32 submitCount, const VkSubmitInfo * pSubmits, VkFence fence);

#include "SDL3/SDL_close_code.h"

#endif //vk_queue.h