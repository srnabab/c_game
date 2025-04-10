#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_mutex.h"

#include "G_constants.h"

#include "vulkan/vulkan.h"

#include "cglm/mat4.h"

#include "vk_code_h/vk_struct.h"

#ifndef G_STATIC_MODEL_H
#define G_STATIC_MODEL_H 1

struct _G_StaticModel
{
    char innerName[16];

    // model matrix and inverse transpose model matrix
    mat4 * matrix;
    Uint32 matrixCount;
    Uint32 firstInstance;
};
typedef struct _G_StaticModel G_StaticModel;

struct _G_StaticModelPool
{
    VkBuffer instanceBuffer[1];
    VkDeviceMemory instanceBufferMem[1];
    void * instanceBufferMemMapped[1];
    Uint32 totalInstanceCount;

    Uint32 usedInstanceCount;
    Uint32 offsetCount;
    Uint32 * offsets;
    G_StaticModel * models;
    Uint32 modelCount;

    SDL_Mutex * mutex;
};
typedef struct _G_StaticModelPool G_StaticModelPool;

#include "SDL3/SDL_begin_code.h"

extern bool SDLCALL createStaticModelPool(G_StaticModelPool * pModelPool, Uint32 totalInstancecount);
extern G_StaticModel* SDLCALL loadStaticModel(G_StaticModelPool * pModelPool, Uint32 instanceCount, PathType modelPath, PathType texturePath, Vertex4 * vertices, Uint32 * pVertexIndex, Uint32 * indices, Uint32 * pIndexIndex, VkFormat textureFormat, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet, bool ground);
extern bool SDLCALL addModelMatrix(int32_t x, int32_t y, int32_t z, G_StaticModelPool * pModelPool, const char * innerName);
extern bool SDLCALL setModelMatrixByIndex(int32_t x, int32_t y, int32_t z, G_StaticModelPool * pModelPool, const char * innerName, Uint32 index);
extern bool SDLCALL getStaticModelDrawInfo(G_StaticModelPool * pModelPool, Uint32 * pFirstInstance, Uint32 * pInstanceCount, const char * innerName);
extern void SDLCALL destroyStaticModelPool(G_StaticModelPool * pModelPool);
 
#include "SDL3/SDL_close_code.h"

#endif // G_staticModel.h