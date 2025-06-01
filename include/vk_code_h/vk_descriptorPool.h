#include "SDL3/SDL_stdinc.h"
#include "vulkan/vulkan.h"
#include "G_buffer.h"
#include "G_texture.h"

#ifndef VK_DESCRIPTORPOOL_H
#define VK_DESCRIPTORPOOL_H 1

typedef void (*G_ImageDescriptorUpdateFunc)(G_Texture_P * pTexture, void * data);

struct _G_DescriptorSets
{
    VkDescriptorSet * pSets;
    bool * used;

    G_ImageDescriptorUpdateFunc updateFunc;

    // set count in shader
    Uint32 shaderSetCount;
    // total set count
    Uint32 totalSetCount;
};
typedef struct _G_DescriptorSets G_DescriptorSets;

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL createDescriptorPool(VkDevice * pDevice, Uint32 poolSizeCount, VkDescriptorPoolSize * pPoolSizes, Uint32 maxSets, VkDescriptorPool * pDescriptorPool);
extern void SDLCALL createDescriptorSets(VkDescriptorPool * pDescriptorPool, VkDescriptorSetLayout * pDescriptorSetLayout, Uint32 shaderSetCount, Uint32 SetsCount, VkDescriptorSet ** ppDescriptorSets);
extern bool SDLCALL G_createDescriptorSets(VkDescriptorPool * pDescriptorPool, VkDescriptorSetLayout * pDescriptorSetLayout, Uint32 shaderSetCount, Uint32 SetsCount, G_ImageDescriptorUpdateFunc func, G_DescriptorSets * pDescriptorSets);
extern VkDescriptorSet * SDLCALL G_getFreeDescriptorSet(G_DescriptorSets * pDescriptorSets, Uint32 shaderSetIndex);
extern void SDLCALL G_destroyDescriptorSets(G_DescriptorSets * pDescriptorSets);
extern void SDLCALL addDescriptorUpdate_Buffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, G_Buffer ** pBuffers);
extern void SDLCALL addDescriptorUpdate_Texture(VkDescriptorType descriptorType, Uint32 binding, G_Texture_P * pTexture, VkDescriptorSet * pSet, VkSampler sampler, VkImageLayout layout);
extern void SDLCALL addDescriptorUpdate_TexelBuffer(VkDescriptorType descriptorType, Uint32 binding, VkDescriptorSet * pSet, VkBufferView * pBufferView);
extern void SDLCALL executeUpdateDescriptorSets(void);
extern void SDLCALL image2dDescriptorSetUpdate(G_Texture_P * pTexture, void * data);
extern void SDLCALL image3dDescriptorSetUpdate(G_Texture_P * pTexture, void * data);

#include "SDL3/SDL_close_code.h"

#endif