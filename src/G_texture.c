#include "G_constants.h"
#include "G_texture.h"
#include "G_struct.h"
#include "G_allocator.h"
#include "G_log.h"

#include "vk_code_h/vk_texture.h"
#include "vk_code_h/vk_depth.h"
#include "vk_code_h/vk_normal.h"
#include "vk_code_h/vk_image.h"
#include "vk_code_h/vk_all_struct.h"

#define UINT32_MAX_PRIME 4294967291

static G_Texture_Head * globalTexture;
// static Uint32 tableCount = 2;

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
    pTexture->layouts = NULL;
    pTexture->layoutCount = 0;
    pTexture->pDescriptorSet = VK_NULL_HANDLE;
    pTexture->frameBuffer = VK_NULL_HANDLE;
    pTexture->offsets = NULL;
    pTexture->offsetSize = 0;
    pTexture->refCount = 0;
}
void logAllTexture(void)
{
    G_Texture_P * pTexture = globalTexture->pTexture;
    while (pTexture != NULL)
    {
        print("address: %p", pTexture);
        print("innerName: %s", pTexture->innerName);
        print("ID: %u", pTexture->ID);
        print("offsets: %p", pTexture->offsets);
        print("refcount: %u", pTexture->refCount);
    }
}
void initGlobalTexture(void)
{
    globalTexture = (G_Texture_Head*)G_calloc(1, sizeof(G_Texture_Head));
    if (globalTexture == NULL) return;

    globalTexture->pTexture = NULL;
    globalTexture->pMiddleTexture = NULL;

    allInOne.pGlobalTexture = globalTexture;
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
static G_Texture_P * toTail(G_Texture_P * pTexture)
{
    if (pTexture == NULL) return NULL;

    if (pTexture->next == NULL) return pTexture;
    else return toTail(pTexture->next);
}
static G_Texture_P * getEmptyTexture(void)
{
    G_Texture_P * pTexture = toTail(globalTexture->pTexture);
    if (pTexture == NULL)
    {
        globalTexture->pTexture = (G_Texture_P*)G_calloc(1, sizeof(G_Texture_P));
        if (globalTexture->pTexture == NULL) return NULL;
        return globalTexture->pTexture;
    }
    else
    {
        pTexture->next = (G_Texture_P*)G_calloc(1, sizeof(G_Texture_P));
        if (pTexture->next == NULL) return NULL;
        pTexture->next->prev = pTexture;
        return pTexture->next;
    }
}
static void deleteTexture(G_Texture_P * pTexture)
{
    if (pTexture == NULL) return;
    if (pTexture->offsets != NULL) G_free(pTexture->offsets);
    if (pTexture->layouts != NULL) G_free(pTexture->layouts);
    if (pTexture->imageMem != NULL) vkFreeMemory(allInOne.device, pTexture->imageMem, allInOne.pAllocationCallbacks);
    if (pTexture->imageView != NULL) vkDestroyImageView(allInOne.device, pTexture->imageView, allInOne.pAllocationCallbacks);
    if (pTexture->image != NULL) vkDestroyImage(allInOne.device, pTexture->image, allInOne.pAllocationCallbacks);

    if (pTexture->next != NULL && pTexture->prev != NULL)
    {
        pTexture->prev->next = pTexture->next;
        pTexture->next->prev = pTexture->prev;
    }
    else if (pTexture->next == NULL && pTexture->prev != NULL)
    {
        pTexture->prev->next = NULL;
    }
    else if (pTexture->next != NULL && pTexture->prev == NULL)
    {
        globalTexture->pTexture = pTexture->next;
        globalTexture->pTexture->prev = NULL;
    }
    else if (pTexture->next == NULL && pTexture->prev == NULL)
    {
        globalTexture->pTexture = NULL;
    }

    G_free(pTexture);
}
bool loadTexture(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet)
{
    SDL_LockMutex(allSync.textureMutex);

    G_Texture_P * pTexture = getEmptyTexture();
    
    SDL_UnlockMutex(allSync.textureMutex);
   
    Uint32 ID = HashID(innerName);

    Uint8 channel;
    void * pixels = (void*)readPNG(path, &pTexture->source_width, &pTexture->source_height, &channel);
    if (pixels == NULL) return false;
    VkDeviceSize imageSize = pTexture->source_width * pTexture->source_height * channel;
    pTexture->format = format;

    createTextureImageFromMem(pixels, pTexture->source_width, pTexture->source_height, imageSize, format, &pTexture->image, &pTexture->imageMem);
    createTextureImageView(&pTexture->image, format, flags, &pTexture->imageView);
    
    G_free(pixels);

    pTexture->offsets = G_calloc(1, sizeof(G_Texture_P) - offsetof(G_Texture_P, offsets));
    if (pTexture->offsets == NULL)
    {
        vkFreeMemory(allInOne.device, pTexture->imageMem, allInOne.pAllocationCallbacks);
        vkDestroyImageView(allInOne.device, pTexture->imageView, allInOne.pAllocationCallbacks);
        vkDestroyImage(allInOne.device, pTexture->image, allInOne.pAllocationCallbacks);
        deleteTexture(pTexture);

        return false;
    }

    pTexture->layoutCount = 1;
    pTexture->layouts = G_calloc(pTexture->layoutCount, sizeof(VkImageLayout));
    if (pTexture->layouts == NULL)
    {
        G_free(pTexture->offsets);
        vkFreeMemory(allInOne.device, pTexture->imageMem, allInOne.pAllocationCallbacks);
        vkDestroyImageView(allInOne.device, pTexture->imageView, allInOne.pAllocationCallbacks);
        vkDestroyImage(allInOne.device, pTexture->image, allInOne.pAllocationCallbacks);
        deleteTexture(pTexture);

        return false;
    }

    pTexture->layouts[0] = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    pTexture->offsetSize = 1;

    pTexture->pDescriptorSet = pDescriptorSet;

    pTexture->ID = ID;

    pTexture->refCount = 0;

    SDL_LockMutex(allSync.textureMutex);

    SDL_strlcpy(pTexture->innerName, innerName, 16);
    print(pTexture->innerName);

    SDL_UnlockMutex(allSync.textureMutex);

    return true;
}
bool addTexture(Uint32 width, Uint32 height, VkFormat format, VkImage image, VkDeviceMemory imageMem, VkImageView imageView, VkDescriptorSet * pDescriptorSet, Uint32 layerCount, VkImageLayout initLayout, const char * innerName)
{
    SDL_LockMutex(allSync.textureMutex);

    G_Texture_P * pTexture = getEmptyTexture();

    SDL_UnlockMutex(allSync.textureMutex);

    pTexture->layouts = G_calloc(layerCount, sizeof(VkImageLayout));
    if (pTexture->layouts == NULL)
    {
        deleteTexture(pTexture);

        return false;
    }
    pTexture->layoutCount = layerCount;
    for (Uint32 i = 0;i < layerCount;i++)
    {
        pTexture->layouts[i] = initLayout;
    }
   
    Uint32 ID = HashID(innerName);

    pTexture->image = image;
    pTexture->imageMem = imageMem;
    pTexture->imageView = imageView;

    pTexture->source_width = width;
    pTexture->source_height = height;
    pTexture->format = format;

    pTexture->pDescriptorSet = pDescriptorSet;

    pTexture->ID = ID;

    SDL_LockMutex(allSync.textureMutex);

    SDL_strlcpy(pTexture->innerName, innerName, 16);

    SDL_UnlockMutex(allSync.textureMutex);

    return true;
}
bool loadShadowResource(const char * innerName, Uint32 width, Uint32 height)
{
    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VkFormat depthFormat = 0;;
    VkImage image = NULL;
    VkDeviceMemory imageMem = NULL;
    VkImageView imageView = NULL;
    findDepthFormat(VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, &depthFormat);
    createImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image, &imageMem);
    createImageView(image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &imageView);

    addTexture(width, height, depthFormat, image, imageMem, imageView, NULL, 1, VK_IMAGE_LAYOUT_UNDEFINED, innerName);

    return true;
}
bool loadDepthResource(const char * innerName, bool sample)
{
    VkImage image = NULL;
    VkDeviceMemory imageMem = NULL;
    VkImageView imageView = NULL;
    VkFormat depthFormat = createDepthResoures(&image, &imageMem, &imageView, sample);

    addTexture(allInOne.extent2D.width, allInOne.extent2D.height, depthFormat, image, imageMem, imageView, NULL, 1, VK_IMAGE_LAYOUT_UNDEFINED, innerName);

    return true;
}
bool loadNormalResource(const char * innerName)
{
    VkImage image = NULL;
    VkDeviceMemory imageMem = NULL;
    VkImageView imageView = NULL;
    VkFormat normalFormat = createNormalResoures(&image, &imageMem, &imageView);
    addTexture(allInOne.extent2D.width, allInOne.extent2D.height, normalFormat, image, imageMem, imageView, NULL, 1, VK_IMAGE_LAYOUT_UNDEFINED, innerName);

    return true;
}
bool loadImageResource(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties\
, VkImageLayout targetLayout, const char * innerName, VkDescriptorSet * pDescriptorSet)
{
    SDL_LockMutex(allSync.textureMutex);

    G_Texture_P * pTexture = getEmptyTexture();
   
    SDL_UnlockMutex(allSync.textureMutex);

    Uint32 ID = HashID(innerName);

    pTexture->layouts = G_calloc(1, sizeof(VkImageLayout));
    if (pTexture->layouts == NULL)
    {
        deleteTexture(pTexture);

        return false;
    }
    pTexture->layoutCount = 1;
    pTexture->layouts[0] = targetLayout;

    createImage(allInOne.extent2D.width, allInOne.extent2D.height, format, tiling, usage, properties, &pTexture->image, &pTexture->imageMem);

    createImageView(pTexture->image, format, VK_IMAGE_ASPECT_COLOR_BIT, &pTexture->imageView);

    if (targetLayout != VK_IMAGE_LAYOUT_UNDEFINED) _transitionImageLayout(NULL, pTexture->image, format, VK_IMAGE_LAYOUT_UNDEFINED, targetLayout, 0, 1);

    pTexture->source_width = allInOne.extent2D.width;
    pTexture->source_height = allInOne.extent2D.height;
    pTexture->format = format;

    if (pDescriptorSet != NULL) pTexture->pDescriptorSet = pDescriptorSet;

    pTexture->ID = ID;

    SDL_LockMutex(allSync.textureMutex);

    SDL_strlcpy(pTexture->innerName, innerName, 16);

    SDL_UnlockMutex(allSync.textureMutex);

    return true;
}
static G_Texture_P * getTextureFromID(Uint32 ID)
{
    G_Texture_P * pTexture = globalTexture->pTexture;
    while (pTexture != NULL)
    {
        if (pTexture->ID == ID) return pTexture;
        pTexture = pTexture->next;
    }

    return NULL;
}
bool addDescriptorSetToTexture(const char * innerName, VkDescriptorSet * pDescriptorSet)
{
    Uint32 ID = HashID(innerName);

    SDL_LockMutex(allSync.textureMutex);

    G_Texture_P * pTexture = getTextureFromID(ID);
    if (pTexture == NULL)
    {
        SDL_UnlockMutex(allSync.textureMutex);
        return false;
    }

    pTexture->pDescriptorSet = pDescriptorSet;
    SDL_UnlockMutex(allSync.textureMutex);

    return false;
}
bool addShadowDescriptorSetToTexture(const char * innerName, VkDescriptorSet * pDescriptorSet)
{
    Uint32 ID = HashID(innerName);

    SDL_LockMutex(allSync.textureMutex);

    G_Texture_P * pTexture = getTextureFromID(ID);
    if (pTexture == NULL)
    {
        SDL_UnlockMutex(allSync.textureMutex);
        return false;
    }

    pTexture->pShadowDescriptorSet = pDescriptorSet;
    SDL_UnlockMutex(allSync.textureMutex);

    return false;
}
G_Texture_P * getTexture(const char * innerName)
{
    SDL_LockMutex(allSync.textureMutex);

    Uint32 ID = HashID(innerName);
    G_Texture_P * pTexture = getTextureFromID(ID);

    SDL_UnlockMutex(allSync.textureMutex);

    return pTexture;
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
    
            if (pTexture->offsetSize > 127) tempPtr = G_realloc(pTexture->offsets, offsetof(G_Texture_P, offsets) * (pTexture->offsetSize + 128));
            else tempPtr = G_realloc(pTexture->offsets, offsetof(G_Texture_P, offsets) * pTexture->offsetSize * 2);
    
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
void setTextureImageLayout(G_Texture_P * pTexture, VkImageLayout layout, Uint32 baseArrayLayer, Uint32 layerCount)
{
    if (pTexture == NULL) return;

    SDL_LockMutex(allSync.textureMutex);

    if (pTexture->layouts == NULL) return;
    for (Uint32 i = baseArrayLayer;i < layerCount + baseArrayLayer;i++)
    {
        pTexture->layouts[i] = layout;
    }

    SDL_UnlockMutex(allSync.textureMutex);
}
void setTextureImageMemoryBarrier(void * imgPNext, VkAccessFlags imgSrcAccessMask, VkAccessFlags imgDstAccessMask, VkImageLayout newLayout, Uint32 imgSrcQueueFamilyIndex\
    , Uint32 imgDstQueueFamilyIndex, VkImageAspectFlags aspectMask, Uint32 baseMipLevel, Uint32 levelCount, Uint32 baseArrayLayer, Uint32 layerCount, VkImageMemoryBarrier * pImageMemoryBarrier\
    , G_Texture_P * pTexture)
{
    pImageMemoryBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    pImageMemoryBarrier->pNext = imgPNext;
    pImageMemoryBarrier->srcAccessMask = imgSrcAccessMask;
    pImageMemoryBarrier->dstAccessMask = imgDstAccessMask;
    pImageMemoryBarrier->oldLayout = pTexture->layouts[baseArrayLayer];

    if (newLayout == VK_IMAGE_LAYOUT_UNDEFINED) pImageMemoryBarrier->newLayout = pImageMemoryBarrier->oldLayout;
    else pImageMemoryBarrier->newLayout = newLayout;

    pImageMemoryBarrier->srcQueueFamilyIndex = imgSrcQueueFamilyIndex;
    pImageMemoryBarrier->dstQueueFamilyIndex = imgDstQueueFamilyIndex;
    pImageMemoryBarrier->image = pTexture->image;
    pImageMemoryBarrier->subresourceRange.aspectMask = aspectMask;
    pImageMemoryBarrier->subresourceRange.baseMipLevel = baseMipLevel;
    pImageMemoryBarrier->subresourceRange.levelCount = levelCount;
    pImageMemoryBarrier->subresourceRange.baseArrayLayer = baseArrayLayer;
    pImageMemoryBarrier->subresourceRange.layerCount = layerCount;

    setTextureImageLayout(pTexture, pImageMemoryBarrier->newLayout, baseArrayLayer, layerCount);
}
bool unloadTexture(const char * innerName)
{
    SDL_LockMutex(allSync.textureMutex);

    G_Texture_P * pTexture = getTextureFromID(HashID(innerName));
    deleteTexture(pTexture);

    SDL_UnlockMutex(allSync.textureMutex);

    return false;
}
void unloadAllTexture(void)
{
    G_Texture_P * pTexture = globalTexture->pTexture;
    while (pTexture != NULL)
    {
        G_Texture_P * pTextureNext = pTexture->next;
        deleteTexture(pTexture);
        pTexture = pTextureNext;
    }
    G_free(globalTexture);
}