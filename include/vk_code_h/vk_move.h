#include "vk_struct.h"

#ifndef VK_MOVE_H
#define VK_MOVE_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDL_UNUSED SDLCALL initializeMovingBuffer(VkPhysicalDevice * pPhysicalDevice, VkDevice * pDevice, VkBuffer * pMoveBuffer, VkDeviceMemory * pMoveBufferMemory, void ** ppMovingBufferMapped, Vertex * vertices, uint32_t verticesCount);
// extern void SDLCALL vertexPosInit(float x, float y, float width, float height, float depth, Uint32 pictureSequence, vec3 ** ppVertices_Pos);
// extern void SDLCALL vertexColorInit(Uint32 pictureSequence, vec3 ** ppVertices_Color);
// extern void SDLCALL vertexTexCoordInit(bool setUVZero, Uint32 pictureSequence, vec2 ** ppVertices_TexCoord);

extern void SDLCALL initVertexMutex(void);
extern void SDLCALL vertexInitialize(float x, float y, float width, float height, float depth, Uint32 vertexCount, Vertex * pVertices);
extern void SDLCALL textureVertexInit(float x, float y, float width, float height, float depth, Uint32 * pVertexCount, Vertex * pVertices, G_Texture_P * tempTexture);
extern void SDLCALL textureVertexInit_SetUV(float x, float y, float width, float height, float depth, Uint32 * pVertexCount, Vertex * pVertices, vec2 * UV, G_Texture_P * tempTexture);
extern void SDLCALL indexInitialize(Uint16 * indices, Uint32 indicesCount);
extern void SDLCALL texturePosUpdate(float x, float y, Vertex * pVertices, Uint32 offset);
extern void SDLCALL vertexPosUpdate(float x, float y, Vertex * pVertices, Uint32 vertexCount);
extern void SDLCALL updatePosition(float x, float y, vec3 ** ppVertices_Pos, uint32_t pictureSequence);
extern void SDLCALL deInitVertexMutex(void);

#include "SDL3/SDL_close_code.h"

#endif //vk_move.h