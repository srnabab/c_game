#include "vk_struct.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL findQueueFamilies(void);
extern void SDLCALL createGraphicsQueue(void);
extern void SDLCALL createPresentQueue(void);
extern void SDLCALL createComputeQueue(void);

#include "SDL3/SDL_close_code.h"

#endif //vk_queue.h