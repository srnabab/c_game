#include "G_constants.h"
#include "G_resource.h"

#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_struct.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_image.h"

static G_Texture_P globalTexture[10];

static SDL_Mutex * textureMutex = NULL;

extern VK_ALL allInOne;

static void emptyTexture(G_Texture_P * pTexture)
{
    pTexture->innerName[0] = '\0';
    pTexture->pathType = None;
    pTexture->source_width = 0;
    pTexture->source_height = 0;
    pTexture->format = VK_FORMAT_UNDEFINED;
    pTexture->image = VK_NULL_HANDLE;
    pTexture->imageView = VK_NULL_HANDLE;
    pTexture->imageMem = VK_NULL_HANDLE;
    pTexture->refCount = 0;
}
void initGlobalTexture()
{
    int i;
    int textureCount = sizeof(globalTexture) / sizeof(G_Texture_P);

    for (i = 0;i < textureCount;i++)
    {
        emptyTexture(globalTexture + i);
    }

    textureMutex = SDL_CreateMutex();

    allInOne.pGlobalTexture = (G_Texture*)globalTexture;
}
bool loadTexture(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName)
{
    int i;
    for (i = 0;i < 10;i++)    
    {
        if (globalTexture[i].pathType == None) break;
    }

    if (i == 10) return false;

    SDL_LockMutex(textureMutex);

    Uint8 channel;
    void * pixels = (void*)readPNG(path, &globalTexture[i].source_width, &globalTexture[i].source_height, &channel);
    if (pixels == NULL) return false;
    VkDeviceSize imageSize = globalTexture[i].source_width * globalTexture[i].source_height * channel;
    globalTexture[i].format = format;

    createTextureImageFromMem(pixels, globalTexture[i].source_width, globalTexture[i].source_height, imageSize, format, &globalTexture[i].image, &globalTexture[i].imageMem);
    createTextureImageView(&globalTexture[i].image, format, flags, &globalTexture[i].imageView);

    SDL_strlcpy(globalTexture[i].innerName, innerName, 16);
    globalTexture[i].pathType = path;

    SDL_UnlockMutex(textureMutex);

    return true;
}
bool loadDepthResource(const char * innerName)
{
    int i;
    for (i = 0;i < 10;i++)    
    {
        if (globalTexture[i].pathType == None) break;
    }

    VkFormat depthFormat;
    findDepthFormat(VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, &depthFormat);
    
    createImage(allInOne.pExtent2D->width, allInOne.pExtent2D->height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &globalTexture[i].image, &globalTexture[i].imageMem);

    createImageView(&globalTexture[i].image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &globalTexture[i].imageView);

    transitionImageLayout(&globalTexture[i].image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    globalTexture[i].source_width = allInOne.pExtent2D->width;
    globalTexture[i].source_height = allInOne.pExtent2D->height;
    globalTexture[i].format = depthFormat;
    SDL_strlcpy(globalTexture[i].innerName, innerName, 16);
    globalTexture[i].pathType = DepthImage;

    return true;
}
G_Texture_P const * getTexture(const char * innerName, PathType type)
{
    int i;
    if (innerName)
    {
        for (i = 0;i < 10;i++)
        {
            if (SDL_strcmp(innerName, globalTexture[i].innerName) == 0) break;
        }
    }
    else
    {
        for (i = 0;i < 10;i++)
        {
            if (globalTexture[i].pathType == type) break;
        }
    }

    if (i == 10) return NULL;

    globalTexture[i].refCount++;

    return globalTexture + i;
}
static G_Texture_P * innerGetTexture(PathType type)
{
    int i;
    for (i = 0;i < 10;i++)
    {
        if (globalTexture[i].pathType == type) break;
    }

    if (i == 10) return NULL;

    globalTexture[i].refCount++;

    return globalTexture + i;
}
bool deRefTexture(G_Texture_P const * pTexture_P, PathType type)
{
    if (pTexture_P)
    {
        if (pTexture_P->refCount == 0) 
        {
            unloadTexture(NULL, pTexture_P->pathType);
            return true;
        }

        for (int i = 0;i < 10;i++)
        {
            if (globalTexture + i == pTexture_P)
            {
                SDL_LockMutex(textureMutex);
                globalTexture[i].refCount--;
                SDL_UnlockMutex(textureMutex);

                break;
            }
        }
    }
    else
    {
        G_Texture_P * tempTexture = innerGetTexture(type);
        
        if (tempTexture->refCount == 0) 
        {
            unloadTexture(NULL, tempTexture->pathType);
            return true;
        }

        for (int i = 0;i < 10;i++)
        {
            if (globalTexture + i == tempTexture)
            {
                SDL_LockMutex(textureMutex);
                globalTexture[i].refCount--;
                SDL_UnlockMutex(textureMutex);

                break;
            }
        }
    }

    return false;
}
bool unloadTexture(const char * innerName, PathType type)
{
    int i;
    if (innerName)
    {
        for (i = 0;i < 10;i++)
        {
            if (SDL_strcmp(innerName, globalTexture[i].innerName) == 0) break;
        }
    }
    else
    {
        for (i = 0;i < 10;i++)
        {
            if (globalTexture[i].pathType == type) break;
        }
    }

    if (i == 10) return false;

    if (globalTexture[i].refCount == 0)
    {
        vkFreeMemory(*allInOne.pDevice, globalTexture[i].imageMem, allInOne.pAllocationCallbacks);
        vkDestroyImageView(*allInOne.pDevice, globalTexture[i].imageView, allInOne.pAllocationCallbacks);
        vkDestroyImage(*allInOne.pDevice, globalTexture[i].image, allInOne.pAllocationCallbacks);
        emptyTexture(globalTexture + i);

        return true;
    }

    return false;
}
void unloadAllTexture(void)
{
    for (int i = 0;i < 10;i++)
    {
        if (globalTexture[i].pathType != None)
        {
            vkFreeMemory(*allInOne.pDevice, globalTexture[i].imageMem, allInOne.pAllocationCallbacks);
            vkDestroyImageView(*allInOne.pDevice, globalTexture[i].imageView, allInOne.pAllocationCallbacks);
            vkDestroyImage(*allInOne.pDevice, globalTexture[i].image, allInOne.pAllocationCallbacks);
        }
    }
}