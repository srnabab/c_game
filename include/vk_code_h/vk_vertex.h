#include "vk_struct.h"

#ifndef VK_VERTEX_H
#define VK_VERTEX_H 1

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createVertexBuffer(VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, void ** ppVertexBufferMemMapped, void * data, Uint32 bufferSize, bool staging);
extern void SDLCALL initVertices33(Uint32 width, Uint32 height, Uint32 row, Uint32 column, float depth, Vertex33_ * pVertices);

#include "SDL3/SDL_close_code.h"

#endif