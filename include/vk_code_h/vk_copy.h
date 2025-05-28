#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"
#include "G_buffer.h"

#ifndef VK_COPY_H
#define VK_COPY_H 1

struct _BufferCopyPack
{
    VkBuffer srcBuffer;
    VkBuffer dstBuffer;
    VkBufferCopy regions;
    int32_t queueFamilyindex;
    int32_t used;
};
typedef struct _BufferCopyPack bufferCopyPack;

#include "SDL3/SDL_begin_code.h"

extern Uint8 SDLCALL recordBufferCopy(Uint32 currentFrame);
extern bool SDLCALL addBufferCopy(G_Buffer * srcBuffer, VkDeviceSize srcOffset, G_Buffer * dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, int32_t queueFamilyindex, Uint32 currentFrame);

#include "SDL3/SDL_close_code.h"

#endif // vk_copy.h