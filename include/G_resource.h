#include "SDL3/SDL_stdinc.h"
#include "G_file/G_file.h"
#include "vulkan/vulkan.h"

#ifndef G_RESOURCE_H
#define G_RESOURCE_H

#include "SDL3/SDL_begin_code.h"

struct _G_Texture_P
{
	char innerName[16];
    
	PathType pathType;
	Uint32 source_width;
	Uint32 source_height;
	VkFormat format;

	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMem;
	VkFramebuffer frameBuffer;

	Uint16 refCount;
};
typedef struct _G_Texture_P G_Texture_P;

struct _G_Texture
{
	G_Texture_P * pParent;
	Uint32 width;
	Uint32 height;

	VkDescriptorSet descriptorSet;
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
		G_Buffer * pBuffer;
		G_Texture * pTexture;
		G_Buffer_View * pTexelBuffer;
	} bufferImage;
	VkDescriptorSet * pSet;
	Uint32 binding;
};
typedef struct _G_DescriptorSet_Update G_DescriptorSet_Update;

extern void SDLCALL initGlobalTexture(void);
extern bool SDLCALL loadTexture(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName);
extern G_Texture_P const * SDLCALL getTextureByName(const char * innerName);
extern void SDLCALL deRefTexture(G_Texture_P * pTexture_P);
extern bool SDLCALL unloadTexture(const char * innerName);

#include "SDL3/SDL_close_code.h"

#endif