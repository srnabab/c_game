#include "SDL3/SDL_stdinc.h"

#include "G_file/path_compare.h"

#ifndef SHADER_RESOLVE_H
#define SHADER_RESOLVE_H

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL freeEntryName(Uint32 count, char ** ppEntryName);
extern Uint32 SDLCALL CreateShaderModulesAndDescriptorSets(PathType * types, int32_t shaderCount, VkShaderModule ** ppShaderModule, VkPipelineShaderStageCreateInfo ** ppShaderStageCreateInfo, VkDescriptorSetLayout ** ppDescriptorSetLayout, VkPipelineLayout * pPipelineLayout, char *** ppEntryName);

#include "SDL3/SDL_close_code.h"

#endif /* SHADER_RESOLVE_H */