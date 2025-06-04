#include "SDL3/SDL_stdinc.h"
#include "G_file/G_file.h"
#include "G_buffer.h"

#ifndef G_RESOURCE_H
#define G_RESOURCE_H

#include "SDL3/SDL_begin_code.h"

struct _G_Texture_P;

struct _G_Texture_P
{
	char innerName[16];
	Uint16 draw;

	Uint32 layoutCount;
	VkImageLayout * layouts;

	struct _G_Texture_P * next;
	struct _G_Texture_P * prev;
    
	Uint32 ID;
	Uint32 source_width;
	Uint32 source_height;
	VkFormat format;

	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMem;

	VkDescriptorSet * pDescriptorSet;
	VkDescriptorSet * pShadowDescriptorSet;
	VkFramebuffer frameBuffer;

    Uint32 offsetSize;
	Uint32 refCount;
	struct _offsets
	{
		Uint32 offset;
		Uint32 count;
	} * offsets;
};
typedef struct _G_Texture_P G_Texture_P;

struct _G_Texture_Head
{
	G_Texture_P * pTexture;
	G_Texture_P * pMiddleTexture;
};
typedef struct _G_Texture_Head G_Texture_Head;

struct _G_Descriptor_Update_Texture
{
	G_Texture_P const * pParent;

	VkSampler sampler;
	VkImageLayout layout;
	char align[4];
};
typedef struct _G_Descriptor_Update_Texture G_Descriptor_Update_Texture;

struct _G_Descriptor_Update_Buffer
{
	G_Buffer ** pBuffer;
};
typedef struct _G_Descriptor_Update_Buffer G_Descriptor_Update_Buffer;

struct _G_Descriptor_Update_Buffer_View
{
	VkBufferView * pBufferView;
};
typedef struct _G_Descriptor_Update_Buffer_View G_Descriptor_Update_Buffer_View;

struct _G_DescriptorSet_Update
{
	VkDescriptorType descriptorType;
	union Buffer_Image
	{
		G_Descriptor_Update_Buffer Buffer;
		G_Descriptor_Update_Texture Texture;
		G_Descriptor_Update_Buffer_View TexelBuffer;
	} bufferImage;
	VkDescriptorSet * pSet;
	Uint32 binding;
};
typedef struct _G_DescriptorSet_Update G_DescriptorSet_Update;

extern void SDLCALL logAllTexture(void);
extern void SDLCALL initGlobalTexture(void);
extern bool SDLCALL loadTexture(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL addTexture(Uint32 width, Uint32 height, VkFormat format, VkImage image, VkDeviceMemory imageMem, VkImageView imageView, VkDescriptorSet * pDescriptorSet, Uint32 layerCount, VkImageLayout initLayout, const char * innerName);
extern G_Texture_P * SDLCALL getTexture(const char * innerName);
extern bool SDLCALL loadShadowResource(const char * innerName, Uint32 width, Uint32 height);
extern bool SDLCALL loadDepthResource(const char * innerName, bool sample);
extern bool SDLCALL loadNormalResource(const char * innerName);
extern bool SDLCALL loadImageResource(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageLayout targetLayout, const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL addDescriptorSetToTexture(const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL addShadowDescriptorSetToTexture(const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL textureOffsetsAdd(G_Texture_P * pTexture, Uint32 offset);
extern void SDLCALL setTextureImageLayout(G_Texture_P * pTexture, VkImageLayout layout, Uint32 baseArrayLayer, Uint32 layerCount);
extern void SDLCALL setTextureImageMemoryBarrier(void * imgPNext, VkAccessFlags imgSrcAccessMask, VkAccessFlags imgDstAccessMask, VkImageLayout newLayout, Uint32 imgSrcQueueFamilyIndex\
    , Uint32 imgDstQueueFamilyIndex, VkImageAspectFlags aspectMask, Uint32 baseMipLevel, Uint32 levelCount, Uint32 baseArrayLayer, Uint32 layerCount, VkImageMemoryBarrier * pImageMemoryBarrier\
    , G_Texture_P * pTexture);
// extern void SDLCALL emptyTextureRefCount(void);
extern void SDLCALL setTextureDraw(G_Texture_P * pTexture, bool draw);
extern bool SDLCALL unloadTexture(const char * innerName);
extern void SDLCALL unloadAllTexture(void);

#include "SDL3/SDL_close_code.h"

#endif // G_texture.h