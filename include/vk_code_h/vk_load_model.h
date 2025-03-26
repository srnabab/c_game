#include "SDL3/SDL_stdinc.h"
#include "vk_code_h/vk_struct.h"

#ifndef VK_LOAD_MODEL_H
#define VK_LOAD_MODEL_H 1

#include "SDL3/SDL_begin_code.h"

extern bool SDLCALL loadModelSetVertex(PathType modelPath, PathType texturePath, Vertex4 * vertices, Uint32 * pVertexIndex, Uint32 * indices, Uint32 * pIndexIndex, VkFormat textureFormat, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet);

#include "SDL3/SDL_close_code.h"

#endif // vk_load_model.h