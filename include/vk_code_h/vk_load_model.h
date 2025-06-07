#include "SDL3/SDL_stdinc.h"
#include "vk_code_h/vk_struct.h"
#include "vk_code_h/vk_descriptorPool.h"
#include "G_image.h"

#ifndef VK_LOAD_MODEL_H
#define VK_LOAD_MODEL_H 1

#include "SDL3/SDL_begin_code.h"

extern bool SDLCALL loadModelSetVertex(PathType modelPath, PathType texturePath, Vertex3323 * vertices, Uint32 * pVertexIndex, Uint32 * indices, Uint32 * pIndexIndex, VkFormat textureFormat, VkImageAspectFlags flags, const char * innerName, G_DescriptorSets * pDescriptorSet, void * data);
// extern bool SDLCALL loadModel(PathType modelPath);

#include "SDL3/SDL_close_code.h"

#endif // vk_load_model.h
