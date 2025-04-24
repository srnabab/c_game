#include "vk_struct.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H 1

#define GRAPHIC_QUEUE_COUNT 2
#define COMPUTE_QUEUE_COUNT 2
#define TRANSFER_QUEUE_COUNT 1
#define PRESENT_QUEUE_COUNT 1

#include "SDL3/SDL_begin_code.h"

extern bool SDLCALL findQueueFamilies(void);
extern void SDLCALL createQueue(void);
extern VkQueue SDLCALL getFirstQueueByCommandPool(VkCommandPool commandPool);
extern VkQueue SDLCALL getGraphic2dQueue(void);
extern VkQueue SDLCALL getGraphic3dQueue(void);
extern VkQueue SDLCALL getPresentQueue(void);
extern VkQueue SDLCALL getComputeQueue(void);
extern VkQueue SDLCALL getSSGIComputeQueue(void);
extern VkQueue SDLCALL getTransferQueue(void);

#include "SDL3/SDL_close_code.h"

#endif //vk_queue.h