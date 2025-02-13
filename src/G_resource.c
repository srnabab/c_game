#include "G_constants.h"
#include "G_resource.h"

#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_struct.h"

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
    VkDeviceSize imageSize = globalTexture[i].source_width * globalTexture[i].source_height * channel;
    globalTexture[i].format = format;

    createTextureImageFromMem(pixels, globalTexture[i].source_width, globalTexture[i].source_height, imageSize, format, &globalTexture[i].image, &globalTexture[i].imageMem);
    createTextureImageView(&globalTexture[i].image, format, flags, &globalTexture[i].imageView);

    SDL_strlcpy(globalTexture[i].innerName, innerName, 16);
    globalTexture[i].pathType = path;

    SDL_UnlockMutex(textureMutex);

    return true;
}
G_Texture_P const * getTextureByName(const char * innerName)
{
    for (int i = 0;i < 10;i++)
    {
        if (SDL_strcmp(innerName, globalTexture[i].innerName) == 0)
        {
            globalTexture[i].refCount++;

            return globalTexture + i;
        }
    }

    return NULL;
}
void deRefTexture(G_Texture_P * pTexture_P)
{
    if (pTexture_P->refCount == 0) return;

    SDL_LockMutex(textureMutex);

    pTexture_P->refCount--;

    SDL_UnlockMutex(textureMutex);
}
bool unloadTexture(const char * innerName)
{
    for (int i = 0;i < 10;i++)
    {
        if (SDL_strcmp(innerName, globalTexture[i].innerName) == 0)
        {
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
    }
}
// bool setDescriptorSet(G_DescriptorSetLayoutConfigCreateInfo * pCreateInfo)
// {
//     if (pCreateInfo == NULL)
//         return false;

//     int i;
//     int descriptorSetCount = sizeof(globalDescriptorSet) / sizeof(G_DescriptorSet);

//     for (i = 0;i < descriptorSetCount;i++)
//     {
//         if (globalDescriptorSet[i].descriptorSet == VK_NULL_HANDLE)
//         {
//             globalDescriptorSet[i].config = (G_DescriptorSetLayoutConfig *)SDL_malloc(sizeof(G_DescriptorSetLayoutConfig));
//             globalDescriptorSet[i].config->createInfo = (G_DescriptorSetLayoutConfigCreateInfo *)SDL_malloc(sizeof(G_DescriptorSetLayoutConfigCreateInfo));
//             globalDescriptorSet[i].config->binding = (VkDescriptorSetLayoutBinding *)SDL_malloc(sizeof(VkDescriptorSetLayoutBinding) * pCreateInfo->bindingCount);

//             globalDescriptorSet[i].config->createInfo->bindingCount = pCreateInfo->bindingCount;

//             for (int j = 0;j < pCreateInfo->bindingCount;j++)
//             {
//                 globalDescriptorSet[i].config->createInfo->type[j] = pCreateInfo->type[j];
//                 globalDescriptorSet[i].config->createInfo->setBinding[j] = pCreateInfo->setBinding[j];
//                 globalDescriptorSet[i].config->createInfo->descriptorCount[j] = pCreateInfo->descriptorCount[j];
//             }

//             createDescriptorSetLayout(&allInOne.pDevice, pCreateInfo->bindingCount, globalDescriptorSet[i].config->binding, 0, &globalDescriptorSet[i].descriptorSetLayout);
//             createDescriptorPool(&allInOne.pDevice, pCreateInfo->bindingCount, globalDescriptorSet[i].config->poolSize, 3, &globalDescriptorSet[i].descriptorSet);
//         }
//     }
// }
// bool loadTexture(PathType type, const char * innerName, Uint8 descriptorSetIndex, VkFramebuffer ** ppFrameBuffer)
// {
//     if (descriptorSetIndex < 0 || descriptorSetIndex >= sizeof(globalDescriptorSet) / sizeof(G_DescriptorSet) || globalDescriptorSet[descriptorSetIndex].descriptorSet == VK_NULL_HANDLE)
//         return false;

//     int textureCount = sizeof(globalTexture) / sizeof(G_Texture);

//     SDL_LockMutex(textureMutex);

//     for (int i = 0;i < textureCount;i++)
//     {
//         if (globalTexture[i].pathType == None)
//         {
//             Uint8 channel;
            
//             res = readPNG(type, &globalTexture[i].source_width, &globalTexture[i].source_height, &channel);

//             if (res == NULL)
//             {
//                 globalTexture[i].source_width = 0;
//                 globalTexture[i].source_height = 0;
//                 return false;
//             }

//             VkFormat format = getVulkanFormat(channel);

//             if (format == VK_FORMAT_UNDEFINED)
//             {
//                 globalTexture[i].source_width = 0;
//                 globalTexture[i].source_height = 0;
//                 SDL_free(res);
//                 return false;
//             }

//             createTextureImage(allInOne.pPhysicalDevice, allInOne.pDevice, &allInOne.commandPool, &allInOne.graphicQueue, type, format, &globalTexture[i].image, &globalTexture[i].imageMem);
//             createTextureImageView(allInOne.pDevice, &globalTexture[i].image, format, VK_IMAGE_ASPECT_COLOR_BIT, &globalTexture[i].imageView);

//             globalTexture[i].channel = channel;
//             globalTexture[i].descriptorSetIndex = descriptorSetIndex;
//             globalTexture[i].pathType = type;
//             globalTexture[i].ppFrameBuffer = ppFrameBuffer;

//             SDL_strlcpy(globalTexture[i].innerName, innerName, sizeof(globalTexture[i].innerName));
//             return true;
//         }
//     }
//     SDL_UnlockMutex(textureMutex);

//     return false;
// }