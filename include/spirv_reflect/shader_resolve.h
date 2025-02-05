#include "SDL3/SDL_stdinc.h"

#include "G_file/path_compare.h"

#ifndef SHADER_RESOLVE_H
#define SHADER_RESOLVE_H

#include "SDL3/SDL_begin_code.h"

extern void SDLCALL freeEntryName(Uint32 count, char ** ppEntryName);
extern void SDLCALL CreateShaderModulesAndDescriptorSets(PathType * types, VkDescriptorPool * pDescriptorPool, int8_t shaderCount, VkShaderModule ** ppShaderModule, VkPipelineShaderStageCreateInfo ** ppShaderStageCreateInfo, VkDescriptorSetLayout ** ppDescriptorSetLayout, VkPipelineLayout * pPipelineLayout, VkDescriptorSet ** ppDescriptorSets, char *** ppEntryName);
extern int SDLCALL SpirvReflectExample(const void* spirv_code, size_t spirv_nbytes);
extern void SDLCALL sprivReflect(PathType type);

#include "SDL3/SDL_close_code.h"

#endif /* SHADER_RESOLVE_H */