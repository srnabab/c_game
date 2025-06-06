#include "G_staticModel.h"
#include "G_allocator.h"

#include "vk_code_h/vk_load_model.h"
#include "vk_code_h/vk_uniform.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_all_struct.h"

#define METER_PER_PIXEL (1.0f / 300)

extern VK_ALL allInOne;

bool createStaticModelPool(G_StaticModelPool * pModelPool, G_BufferPool * pBufferPool, Uint32 totalInstancecount)
{
    VkDeviceSize bufferSize = sizeof(mat4) * totalInstancecount * 2;

    // *pModelPool = (G_StaticModelPool)G_malloc(sizeof(G_StaticModelPool_T));

    // model matrix buffer and inverse transpose matrix buffer
    pModelPool->instanceBuffer = allocateBuffer(bufferSize, pBufferPool);
    // createBuffer(pModelPool->instanceBuffer + 0, pModelPool->instanceBufferMem + 0, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // vkMapMemory(allInOne.device, pModelPool->instanceBufferMem[0], 0, bufferSize, 0, pModelPool->instanceBufferMemMapped + 0);
    // memset(pModelPool->instanceBufferMemMapped[0], 0, bufferSize);

    pModelPool->totalInstanceCount = totalInstancecount;
    pModelPool->usedInstanceCount = 0;
    pModelPool->models = NULL;
    pModelPool->modelCount = 0;
    pModelPool->offsets = (Uint32*)G_malloc(sizeof(Uint32));
    pModelPool->offsets[0] = 0;
    pModelPool->offsetCount = 1;

    pModelPool->mutex = SDL_CreateMutex();

    return true;
}
G_StaticModel * loadStaticModel(G_StaticModelPool * pModelPool, Uint32 instanceCount, PathType modelPath, PathType texturePath, Vertex3323 * vertices, Uint32 * pVertexIndex, Uint32 * indices, Uint32 * pIndexIndex\
, VkFormat textureFormat, VkImageAspectFlags flags, const char * innerName, G_DescriptorSets * pDescriptorSet, void * data, bool draw)
{
#warning find if have same name first
    SDL_LockMutex(pModelPool->mutex);

    bool res;
    void * ptr;
    Uint32 modelCount = pModelPool->modelCount;
    Uint32 offsetCount = pModelPool->offsetCount;

    if (pModelPool->usedInstanceCount + instanceCount > pModelPool->totalInstanceCount) 
    {
        SDL_UnlockMutex(pModelPool->mutex);

        return NULL;
    }

    ptr = (G_StaticModel*)G_realloc(pModelPool->models, (pModelPool->modelCount + 1) * sizeof(G_StaticModel));
    if (ptr == NULL)
    {
        SDL_UnlockMutex(pModelPool->mutex);

        return NULL;
    }
    pModelPool->models = ptr;
    pModelPool->modelCount++;

    ptr = (Uint32*)G_realloc(pModelPool->offsets, (pModelPool->offsetCount + 1) * sizeof(Uint32));
    if (ptr == NULL)
    {
        pModelPool->modelCount--;

        SDL_UnlockMutex(pModelPool->mutex);

        return NULL;
    }
    pModelPool->offsets = ptr;
    pModelPool->offsets[pModelPool->offsetCount] = pModelPool->offsets[pModelPool->offsetCount - 1] + instanceCount; 
    pModelPool->offsetCount++;

    SDL_UnlockMutex(pModelPool->mutex);

    res = loadModelSetVertex(modelPath, texturePath, vertices, pVertexIndex, indices, pIndexIndex, textureFormat, flags, innerName, pDescriptorSet, data, draw);
    if (res == false)
    {
        SDL_LockMutex(pModelPool->mutex);

        pModelPool->modelCount--;
        pModelPool->offsetCount--;

        SDL_UnlockMutex(pModelPool->mutex);

        return NULL;
    }

    pModelPool->models[modelCount].firstInstance = pModelPool->offsets[offsetCount - 1];
    pModelPool->models[modelCount].matrix = (mat4*)G_malloc(sizeof(mat4) * instanceCount * 2);
    pModelPool->models[modelCount].matrixCount = 0;
    pModelPool->models[modelCount].totalMatrixCount = instanceCount;
    SDL_strlcpy(pModelPool->models[modelCount].innerName, innerName, 16);

    return pModelPool->models + modelCount;
}
static G_StaticModel * findStaticModel(G_StaticModelPool * pModelPool, const char * innerName)
{
    Uint32 i;
    for (i = 0;i < pModelPool->modelCount;i++)
    {
        if (SDL_strcmp(pModelPool->models[i].innerName, innerName) == 0)
        {
            return pModelPool->models + i;
        }
    }

    return NULL;
}
bool addModelMatrix(int32_t x, int32_t y, int32_t z, float scale_x, float scale_y, float scale_z, float z_angle, G_StaticModelPool * pModelPool, const char * innerName)
{
    SDL_LockMutex(pModelPool->mutex);

    Uint32 i;
    Uint32 modelCount = pModelPool->modelCount;
    Uint32 totalMatrixCount;
    G_StaticModel * pModel;
    vec3 tempVec3;
    tempVec3[0] = x * METER_PER_PIXEL;
    tempVec3[1] = y * METER_PER_PIXEL;
    tempVec3[2] = z * METER_PER_PIXEL;

    for (i = 0;i < modelCount;i++)
    {
        if (SDL_strcmp(pModelPool->models[i].innerName, innerName) == 0)
        {
            pModel = pModelPool->models + i;

            break;
        }
    }

    if (i == modelCount)
    {
        SDL_UnlockMutex(pModelPool->mutex);
        return false;
    }

    totalMatrixCount = pModelPool->models[i].totalMatrixCount;

    if (pModel->matrixCount == totalMatrixCount)
    {
        SDL_UnlockMutex(pModelPool->mutex);
        return false;
    }

    glm_mat4_identity(pModel->matrix[pModel->matrixCount]);

    // translate
    glm_translate(pModel->matrix[pModel->matrixCount], tempVec3);

    // rotate
    glm_rotate(pModel->matrix[pModel->matrixCount], glm_rad(180.0f), (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(pModel->matrix[pModel->matrixCount], glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(pModel->matrix[pModel->matrixCount], glm_rad(z_angle), (vec3){0.0f, 0.0f, 1.0f});

    // scale
    glm_scale(pModel->matrix[pModel->matrixCount], (vec3){scale_x, scale_z, scale_y});

    glm_mat4_copy(pModel->matrix[pModel->matrixCount], pModel->matrix[pModel->matrixCount + totalMatrixCount]);
    glm_inv_tr(pModel->matrix[pModel->matrixCount + totalMatrixCount]);

    G_Buffer * stagingBuffer = allocateStagingBuffer(sizeof(mat4) * 2, &allInOne.stagingBufferPool);
#warning error processing needed
    // bufferMemcpy(pModelPool->instanceBuffer, (pModel->firstInstance + pModel->matrixCount) * sizeof(mat4), pModel->matrix + pModel->matrixCount, sizeof(mat4));
    bufferMemcpy(stagingBuffer, 0, pModel->matrix + pModel->matrixCount, sizeof(mat4));
    addBufferCopy(stagingBuffer, 0, pModelPool->instanceBuffer, (pModel->firstInstance + pModel->matrixCount) * sizeof(mat4), sizeof(mat4), allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.currentFrame);

    // bufferMemcpy(pModelPool->instanceBuffer, (pModel->firstInstance + pModelPool->totalInstanceCount + pModel->matrixCount) * sizeof(mat4), pModel->matrix + totalMatrixCount + pModel->matrixCount, sizeof(mat4));
    bufferMemcpy(stagingBuffer, sizeof(mat4), pModel->matrix + totalMatrixCount + pModel->matrixCount, sizeof(mat4));
    addBufferCopy(stagingBuffer, sizeof(mat4), pModelPool->instanceBuffer, (pModel->firstInstance + pModelPool->totalInstanceCount + pModel->matrixCount) * sizeof(mat4), sizeof(mat4), allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.currentFrame);

    pModel->matrixCount++;

    SDL_UnlockMutex(pModelPool->mutex);

    freeStagingBuffer(stagingBuffer);

    return true;
}
bool deleteModelMatrixByIndex(G_StaticModelPool * pModelPool, const char * innerName, Uint32 index)
{
    SDL_LockMutex(pModelPool->mutex);

    G_StaticModel * pModel = findStaticModel(pModelPool, innerName);

    if (index >= pModel->matrixCount)
    {
        SDL_UnlockMutex(pModelPool->mutex);

        return false;
    }
    else if (index == pModel->matrixCount - 1)
    {
        pModel->matrixCount--;

        SDL_UnlockMutex(pModelPool->mutex);

        return true;
    }

    addBufferCopy(pModelPool->instanceBuffer, (pModel->firstInstance + index + 1) * sizeof(mat4), pModelPool->instanceBuffer, (pModel->firstInstance + index) * sizeof(mat4), sizeof(mat4) * (pModel->matrixCount - index - 1)\
    , allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.currentFrame);
    // bufferMemmove(pModelPool->instanceBuffer, (pModel->firstInstance + index + 1) * sizeof(mat4), (pModel->firstInstance + index) * sizeof(mat4), sizeof(mat4) * (pModel->matrixCount - index - 1));
    // memmove((mat4*)pModelPool->instanceBufferMemMapped[0] + (pModel->firstInstance + index), (mat4*)pModelPool->instanceBufferMemMapped[0] + (pModel->firstInstance + index + 1), sizeof(mat4) * (pModel->matrixCount - index - 1)); 
    addBufferCopy(pModelPool->instanceBuffer, (pModel->firstInstance + index + 1 + pModel->totalMatrixCount) * sizeof(mat4), pModelPool->instanceBuffer, (pModel->firstInstance + index + pModel->totalMatrixCount) * sizeof(mat4)\
    , sizeof(mat4) * (pModel->matrixCount - index - 1), allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.currentFrame);
    // bufferMemmove(pModelPool->instanceBuffer, (pModel->firstInstance + index + 1 + pModel->totalMatrixCount) * sizeof(mat4), (pModel->firstInstance + index + pModel->totalMatrixCount) * sizeof(mat4), sizeof(mat4) * (pModel->matrixCount - index - 1));

    pModel->matrixCount--;

    SDL_UnlockMutex(pModelPool->mutex);

    return true;
}
// bool setModelMatrixByIndex(int32_t x, int32_t y, int32_t z, G_StaticModelPool * pModelPool, const char * innerName, Uint32 index)
// {
//     G_StaticModel * pModel;
//     Uint32 totalMatrixCount; 
//     vec3 tempVec3;
//     tempVec3[0] = x * METER_PER_PIXEL;
//     tempVec3[1] = y * METER_PER_PIXEL;
//     tempVec3[2] = z * METER_PER_PIXEL;

//     SDL_LockMutex(pModelPool->mutex);

//     pModel = findStaticModel(pModelPool, innerName);
//     if (pModel == NULL)
//     {
//         SDL_UnlockMutex(pModelPool->mutex);

//         return false;
//     }

//     totalMatrixCount = pModel->totalMatrixCount;

//     if (index >= pModel->matrixCount)
//     {
//         SDL_UnlockMutex(pModelPool->mutex);
//         return false;
//     }

//     glm_mat4_identity(pModel->matrix[index]);
//     glm_translate(pModel->matrix[index], tempVec3);
//     glm_rotate(pModel->matrix[index], glm_rad(180.0f), (vec3){0.0f, 1.0f, 0.0f});
//     glm_rotate(pModel->matrix[index], glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});

//     glm_mat4_copy(pModel->matrix[index], pModel->matrix[index + totalMatrixCount]);
//     glm_inv_tr(pModel->matrix[pModel->matrixCount + totalMatrixCount]);

//     G_Buffer * stagingBuffer = allocateStagingBuffer(sizeof(mat4) * 2, &allInOne.stagingBufferPool);
// #warning error processing needed
//     bufferMemcpy(stagingBuffer, 0, pModel->matrix + index, sizeof(mat4));
//     addBufferCopy(stagingBuffer, 0, pModelPool->instanceBuffer, (pModel->firstInstance + index) * sizeof(mat4), sizeof(mat4), allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.currentFrame);

//     bufferMemcpy(stagingBuffer, sizeof(mat4), pModel->matrix + totalMatrixCount + index, sizeof(mat4));
//     addBufferCopy(stagingBuffer, sizeof(mat4), pModelPool->instanceBuffer, (pModel->firstInstance + pModelPool->totalInstanceCount + index) * sizeof(mat4), sizeof(mat4), allInOne.queueFamilyIndices.graphicsFamily.familyIndice, allInOne.currentFrame);
//     // bufferMemcpy(pModelPool->instanceBuffer, (pModel->firstInstance + index) * sizeof(mat4), pModel->matrix + index, sizeof(mat4));
//     // bufferMemcpy(pModelPool->instanceBuffer, (pModel->firstInstance + pModelPool->totalInstanceCount + index) * sizeof(mat4), pModel->matrix + totalMatrixCount + index, sizeof(mat4));
    
//     SDL_UnlockMutex(pModelPool->mutex);

//     freeStagingBuffer(stagingBuffer);

//     return true;
// }
bool getStaticModelDrawInfo(G_StaticModelPool * pModelPool, Uint32 * pFirstInstance, Uint32 * pInstanceCount, const char * innerName)
{
    SDL_LockMutex(pModelPool->mutex);

    Uint32 i;
    Uint32 modelCount = pModelPool->modelCount;
    G_StaticModel * pModel;

    for (i = 0;i < modelCount;i++)
    {
        if (SDL_strcmp(pModelPool->models[i].innerName, innerName) == 0)
        {
            pModel = pModelPool->models + i;

            break;
        }
    }

    if (i == modelCount)
    {
        SDL_UnlockMutex(pModelPool->mutex);
        return false;
    }

    *pFirstInstance = pModel->firstInstance;
    *pInstanceCount = pModel->matrixCount;

    SDL_UnlockMutex(pModelPool->mutex);

    return true;
}
void destroyStaticModelPool(G_StaticModelPool * pModelPool)
{
    if (pModelPool == NULL)
    {
        return;
    }

    if (pModelPool->models == NULL)
    {
        return;
    }

    // destroyBuffer(pModelPool->instanceBuffer[0], pModelPool->instanceBufferMem[0]);

    for (Uint32 i = 0;i < pModelPool->modelCount;i++)
    {
        G_free(pModelPool->models[i].matrix);
    }
    G_free(pModelPool->models);
    G_free(pModelPool->offsets);

    SDL_DestroyMutex(pModelPool->mutex);

    pModelPool->totalInstanceCount = 0;
    pModelPool->modelCount = 0;
}
