#include "SDL3/SDL_stdinc.h"
#include "G_file/G_file.h"
#include "vulkan/vulkan.h"

#ifndef G_RESOURCE_H
#define G_RESOURCE_H

#include "SDL3/SDL_begin_code.h"

struct _G_Texture_P
{
	char innerName[16];
    
	Uint32 ID;
	Uint32 source_width;
	Uint32 source_height;
	VkFormat format;

	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMem;

	VkDescriptorSet * pDescriptorSet;
	VkFramebuffer frameBuffer;

	struct _offsets
	{
		Uint32 offset;
		Uint32 count;
	} * offsets;
    Uint32 offsetSize;
	Uint32 refCount;
};
typedef struct _G_Texture_P G_Texture_P;

struct _G_Texture
{
	G_Texture_P const * pParent;

	VkSampler sampler;
	VkImageLayout layout;
};
typedef struct _G_Texture G_Texture;

struct _G_Buffer
{
	VkBuffer * pBuffer;
	VkDeviceSize offset;
	VkDeviceSize range;
};
typedef struct _G_Buffer G_Buffer;

struct _G_Buffer_View
{
	VkBufferView * pBufferView;
};
typedef struct _G_Buffer_View G_Buffer_View;

struct _G_DescriptorSet_Update
{
	VkDescriptorType descriptorType;
	union Buffer_Image
	{
		G_Buffer Buffer;
		G_Texture Texture;
		G_Buffer_View TexelBuffer;
	} bufferImage;
	VkDescriptorSet * pSet;
	Uint32 binding;
};
typedef struct _G_DescriptorSet_Update G_DescriptorSet_Update;

extern void SDLCALL logAllTexture(void);
extern void SDLCALL initGlobalTexture(void);
extern bool SDLCALL loadTexture(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName, VkDescriptorSet * pDescriptorSet);
extern G_Texture_P * SDLCALL getTexture(const char * innerName);
extern bool SDLCALL loadDepthResource(const char * innerName, bool sample);
extern bool SDLCALL loadNormalResource(const char * innerName);
extern bool SDLCALL loadStorageImageResource(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageLayout targetLayout, const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL addDescriptorSetToTexture(const char * innerName, VkDescriptorSet * pDescriptorSet);
extern bool SDLCALL textureOffsetsAdd(G_Texture_P * pTexture, Uint32 offset);
extern void SDLCALL emptyTextureRefCount(void);
extern bool SDLCALL unloadTexture(const char * innerName);
extern void SDLCALL unloadAllTexture(void);

#include "SDL3/SDL_close_code.h"

#endif