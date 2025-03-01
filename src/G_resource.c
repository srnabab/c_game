#include "G_constants.h"
#include "G_resource.h"
#include "G_struct.h"

#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_struct.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_image.h"

#define UINT32_MAX_PRIME 4294967291

static G_Texture_P * globalTexture;
static Uint32 tableCount = 2;

extern VK_ALL allInOne;
extern G_SYNC allSync;

static void emptyTexture(G_Texture_P * pTexture)
{
    pTexture->innerName[0] = '\0';
    pTexture->ID = 0;
    pTexture->source_width = 0;
    pTexture->source_height = 0;
    pTexture->format = VK_FORMAT_UNDEFINED;
    pTexture->image = VK_NULL_HANDLE;
    pTexture->imageView = VK_NULL_HANDLE;
    pTexture->imageMem = VK_NULL_HANDLE;
    pTexture->pDescriptorSet = VK_NULL_HANDLE;
    pTexture->frameBuffer = VK_NULL_HANDLE;
    pTexture->offsets = NULL;
    pTexture->offsetSize = 0;
    pTexture->refCount = 0;
}
void initGlobalTexture()
{
    int i;
    globalTexture = (G_Texture_P*)SDL_calloc(tableCount, sizeof(G_Texture_P));
    if (globalTexture == NULL) return;

    for (i = 0;i < tableCount;i++) emptyTexture(globalTexture + i);

    allInOne.pGlobalTexture = globalTexture;
}
static bool resizeTexture(Uint32 newSize)
{
    void * tempPtr = SDL_realloc(globalTexture, sizeof(G_Texture_P) * newSize);
    if (tempPtr == NULL) return false;

    globalTexture = (G_Texture_P*)tempPtr;
    for (Uint32 i = tableCount;i < newSize;i++) emptyTexture(globalTexture + i);

    return true;
}
static Uint32 HashID(const char * innerName)
{
    Uint32 hash = 0;
    int len = SDL_strlen(innerName);
    for (int i = 0;i < len;i++)
    {
        hash = (hash * 131 + innerName[i]) % UINT32_MAX_PRIME;
    }

    return hash;
}
bool loadTexture(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet)
{
    SDL_LockMutex(allSync.textureMutex);

    int i;
    for (i = 0;i < tableCount;i++)
    {
        if (SDL_strcmp(innerName, globalTexture[i].innerName) == 0) return false;
    }
    for (i = 0;i < tableCount;i++)
    {
        if (globalTexture[i].innerName[0] == '\0') break;
    }

    if (i == tableCount)
    {
        if (!resizeTexture(tableCount * 2)) return false;
        tableCount *= 2;
    }
    
    Uint32 ID = HashID(innerName);

    Uint8 channel;
    void * pixels = (void*)readPNG(path, &globalTexture[i].source_width, &globalTexture[i].source_height, &channel);
    if (pixels == NULL) return false;
    VkDeviceSize imageSize = globalTexture[i].source_width * globalTexture[i].source_height * channel;
    globalTexture[i].format = format;

    createTextureImageFromMem(pixels, globalTexture[i].source_width, globalTexture[i].source_height, imageSize, format, &globalTexture[i].image, &globalTexture[i].imageMem);
    createTextureImageView(&globalTexture[i].image, format, flags, &globalTexture[i].imageView);
    
    SDL_free(pixels);

    SDL_strlcpy(globalTexture[i].innerName, innerName, 16);

    globalTexture[i].offsets = SDL_calloc(1, offsetof(G_Texture_P, offsets));
    if (globalTexture[i].offsets == NULL)
    {
        vkFreeMemory(*allInOne.pDevice, globalTexture[i].imageMem, allInOne.pAllocationCallbacks);
        vkDestroyImageView(*allInOne.pDevice, globalTexture[i].imageView, allInOne.pAllocationCallbacks);
        vkDestroyImage(*allInOne.pDevice, globalTexture[i].image, allInOne.pAllocationCallbacks);

        SDL_UnlockMutex(allSync.textureMutex);

        return false;
    }

    globalTexture[i].offsetSize = 1;

    globalTexture[i].pDescriptorSet = pDescriptorSet;

    globalTexture[i].ID = ID;

    SDL_UnlockMutex(allSync.textureMutex);

    return true;
}
bool loadDepthResource(const char * innerName)
{
    SDL_LockMutex(allSync.textureMutex);

    int i;
    for (i = 0;i < tableCount;i++)
    {
        if (SDL_strcmp(innerName, globalTexture[i].innerName) == 0) return false;
    }
    for (i = 0;i < tableCount;i++)
    {
        if (globalTexture[i].innerName[0] == '\0') break;
    }

    if (i == tableCount)
    {
        if (!resizeTexture(tableCount * 2)) return false;
        tableCount *= 2;
    }
    
    Uint32 ID = HashID(innerName);

    VkFormat depthFormat;
    findDepthFormat(VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, &depthFormat);
    
    createImage(allInOne.pExtent2D->width, allInOne.pExtent2D->height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &globalTexture[i].image, &globalTexture[i].imageMem);

    createImageView(&globalTexture[i].image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &globalTexture[i].imageView);

    transitionImageLayout(&globalTexture[i].image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    globalTexture[i].source_width = allInOne.pExtent2D->width;
    globalTexture[i].source_height = allInOne.pExtent2D->height;
    globalTexture[i].format = depthFormat;
    SDL_strlcpy(globalTexture[i].innerName, innerName, 16);

    globalTexture[i].ID = ID;

    SDL_UnlockMutex(allSync.textureMutex);

    return true;
}
G_Texture_P * getTexture(const char * innerName)
{
    SDL_LockMutex(allSync.textureMutex);
    
    Uint32 i;
    Uint32 ID = HashID(innerName);
    for (i = 0;i < tableCount;i++)
    {
        if (ID == globalTexture[i].ID) break;
    }


    if (i == tableCount)
    {
        SDL_UnlockMutex(allSync.textureMutex);
        return NULL;
    }

    SDL_UnlockMutex(allSync.textureMutex);

    return globalTexture + i;
}
bool textureOffsetsAdd(G_Texture_P * pTexture, Uint32 offset)
{
    SDL_LockMutex(allSync.textureMutex);
    
    if (pTexture->refCount && (offset == pTexture->offsets[pTexture->refCount - 1].count * 4 + pTexture->offsets[pTexture->refCount - 1].offset))
    {
        pTexture->offsets[pTexture->refCount - 1].count += 1;
    }
    else
    {
        if (pTexture->offsetSize == pTexture->refCount)
        {
            void * tempPtr;
    
            if (pTexture->offsetSize > 127) tempPtr = SDL_realloc(pTexture->offsets, offsetof(G_Texture_P, offsets) * (pTexture->offsetSize + 128));
            else tempPtr = SDL_realloc(pTexture->offsets, offsetof(G_Texture_P, offsets) * pTexture->offsetSize * 2);
    
            if (tempPtr == NULL)
            {
                SDL_UnlockMutex(allSync.textureMutex);
                return false;
            }
    
            pTexture->offsets = tempPtr;
            if (pTexture->offsetSize > 127) pTexture->offsetSize += 128;
            else pTexture->offsetSize *= 2;
        }
        pTexture->offsets[pTexture->refCount].offset = offset;
        pTexture->offsets[pTexture->refCount].count = 1;
        pTexture->refCount++;
    }

    SDL_UnlockMutex(allSync.textureMutex);

    return true;
}
void emptyTextureRefCount(void)
{
    SDL_LockMutex(allSync.textureMutex);

    for (int i = 0;i < tableCount;i++)
    {
        globalTexture[i].refCount = 0;
    }

    SDL_UnlockMutex(allSync.textureMutex);
}
bool unloadTexture(const char * innerName)
{
    SDL_LockMutex(allSync.textureMutex);

    int i;
    
    Uint32 ID = HashID(innerName);

    for (i = 0;i < tableCount;i++)
    {
        if (ID == globalTexture[i].ID) break;
    }

    if (i == tableCount) return false;

    if (globalTexture[i].refCount == 0)
    {
        vkFreeMemory(*allInOne.pDevice, globalTexture[i].imageMem, allInOne.pAllocationCallbacks);
        vkDestroyImageView(*allInOne.pDevice, globalTexture[i].imageView, allInOne.pAllocationCallbacks);
        vkDestroyImage(*allInOne.pDevice, globalTexture[i].image, allInOne.pAllocationCallbacks);
        emptyTexture(globalTexture + i);

        return true;
    }

    SDL_UnlockMutex(allSync.textureMutex);

    return false;
}
void unloadAllTexture(void)
{
    for (int i = 0;i < tableCount;i++)
    {
        if (globalTexture[i].innerName[0] != '\0')
        {
            vkDestroyImageView(*allInOne.pDevice, globalTexture[i].imageView, allInOne.pAllocationCallbacks);
            vkDestroyImage(*allInOne.pDevice, globalTexture[i].image, allInOne.pAllocationCallbacks);
            vkFreeMemory(*allInOne.pDevice, globalTexture[i].imageMem, allInOne.pAllocationCallbacks);
        }
    }
}




// static bool reHashID(Uint32 oldSetSize, Uint32 newSetSize)
// {
//     Uint32 i, j;

//     G_Texture_P tempTexture; 

//     Uint32 * old_new = (Uint32*)SDL_calloc(oldSetSize, sizeof(Uint32));
//     if (old_new == NULL) return false;

//     for (i = 0;i < oldSetSize;i++)
//     {
//         old_new[i] = HashID(globalTexture[i].innerName, newSetSize);
//         globalTexture[i].ID = old_new[i];
//     }

//     for (i = 0, j = 0;i < oldSetSize;i++)
//     {
//         while (i != globalTexture[i].ID)
//         {
//             tempTexture = globalTexture[old_new[i]];
//             globalTexture[old_new[i]] = globalTexture[i];
//             globalTexture[i] = tempTexture;
            
//             if (old_new[i] < oldSetSize)
//             {
//                 if (old_new[old_new[i]] == old_new[i]) break;
//                 old_new[i] = old_new[old_new[i]];
//             }

//             if (globalTexture[i].ID > oldSetSize)
//             {
//                 tempTexture = globalTexture[old_new[i]];
//                 globalTexture[old_new[i]] = globalTexture[i];
//                 globalTexture[i] = tempTexture;

//                 globalTexture[i].ID = i;
//             }
//         }
//         old_new[i] = i;
//     }
// }

// // test
// #ifdef TEST

// #include "SDL3/SDL_test.h"

// static G_Texture_P * globalTexture_test;
// static Uint32 tableCount_test = 2;

// static SDL_Mutex * textureMutex_test = NULL;

// extern VK_ALL allInOne;

// #endif