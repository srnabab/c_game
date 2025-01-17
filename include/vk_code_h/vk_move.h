#include "vk_struct.h"

#ifndef VK_MOVE_H
#define VK_MOVE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, uint32_t verticesCount);
extern void SDLCALL positionInitialize(int x, int y, int width, int height, VkExtent2D extent2D, Vertex ** ppVertices, uint32_t pictureSequence);
extern void SDLCALL reInitializePosition(int width, int height, VkExtent2D extent2D, Vertex ** ppVertices, uint32_t pictureSequence);

#include "SDL3/SDL_close_code.h"

#endif //vk_move.h