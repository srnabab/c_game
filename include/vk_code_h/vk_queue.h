#include "vk_struct.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL findQueueFamilies(void);
extern void SDLCALL createQueue(Uint32 index, VkQueue * pQueue);

#include "SDL3/SDL_close_code.h"

#endif //vk_queue.h