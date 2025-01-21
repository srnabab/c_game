#include "vk_struct.h"

#ifndef VK_MOVE_H
#define VK_MOVE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDL_UNUSED SDLCALL initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, uint32_t verticesCount);
extern void SDLCALL vertexInitialize(float x, float y, float width, float height, float depth, bool setUVZero, VkExtent2D extent2D, Vertex ** ppVertices, uint32_t pictureSequence);
extern void SDLCALL indexInitialize(Uint16 * indices, Uint32 indicesCount);
extern void SDLCALL updatePosition(float x, float y, VkExtent2D * pExtent2D, Vertex ** ppVertices, uint32_t pictureSequence);

#include "SDL3/SDL_close_code.h"

#endif //vk_move.h