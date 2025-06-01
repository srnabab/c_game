#include "SDL3/SDL_stdinc.h"

#include "vk_code_h/vk_descriptorPool.h"

#include "G_file/G_file.h"

#ifndef G_IMAGE_H
#define G_IMAGE_H 1

#include "SDL3/SDL_begin_code.h"

extern int SDLCALL G_loadImage(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName, G_DescriptorSets * pDescriptorSets, Uint32 setIndex, void * data, bool draw);

#include "SDL3/SDL_close_code.h"

#endif // G_image.h