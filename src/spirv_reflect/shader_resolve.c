#include "vulkan/vulkan.h"

#include "spirv_reflect/shader_resolve.h"
#include "spirv_reflect/spirv_reflect.h"

#include "vk_code_h/vk_shader.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_collection.h"

#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_assert.h"

#include "G_constants.h"
#include "G_log.h"
#include "G_pop_window.h"
#include "G_file/G_file.h"

extern VK_ALL allInOne;

#include <stdio.h>

Uint32 CreateShaderModulesAndDescriptorSets(PathType * types, int32_t shaderCount, VkShaderModule ** ppShaderModule,\
VkPipelineShaderStageCreateInfo ** ppShaderStageCreateInfo, VkDescriptorSetLayout ** ppDescriptorSetLayout, VkPipelineLayout * pPipelineLayout)
{
    int32_t i, j;
    int32_t setCount = 1;
    int32_t biggestSet = 0;
    int32_t bindingsCount[10] = {};
    int32_t pushConstantCount = 0;
    Uint32 pushConstantOffset = 0;
    VkDescriptorSetLayoutBinding ppSetLayoutBinding[5][5];
    VkPushConstantRange * pPushConstantRange = (VkPushConstantRange*)SDL_malloc(shaderCount * sizeof(VkPushConstantRange));

    SDL_IOStream * shaderFile;

    SpvReflectResult spv_result;
    
    char ** entryName = (char **)SDL_malloc(shaderCount * sizeof(char*));
    for (i = 0;i < shaderCount;i++) entryName[i] = (char*)SDL_malloc(255 * sizeof(char));

    *ppShaderModule = (VkShaderModule*)SDL_malloc(shaderCount * sizeof(VkShaderModule));
    *ppShaderStageCreateInfo = (VkPipelineShaderStageCreateInfo*)SDL_malloc(shaderCount * sizeof(VkPipelineShaderStageCreateInfo));
    
    for (i = 0;i < shaderCount;i++)
    {
        if ((shaderFile = SDL_IOFromFile(getPath(types[i]), "rb+")) == NULL)
        {
            OPEN_FILE_FAILED(types[i])
        }

        SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_END);
        long fileSize = SDL_TellIO(shaderFile);
        //printf("\nfileSize: %u\n", fileSize);
        SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_SET);

        char * shaderCode = (char *)SDL_malloc(fileSize * sizeof(char));
        SDL_ReadIO(shaderFile, shaderCode, sizeof(char) * fileSize);

        createShaderModuleFromMem(fileSize, (const Uint32*)shaderCode, (*ppShaderModule) + i);
        CO_addShaderModule((*ppShaderModule)[i]);// CO

        SpvReflectShaderModule module;
        spv_result = spvReflectCreateShaderModule(fileSize, shaderCode, &module);
        SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

        (*ppShaderStageCreateInfo)[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        (*ppShaderStageCreateInfo)[i].pNext = NULL;
        (*ppShaderStageCreateInfo)[i].flags = 0;
        (*ppShaderStageCreateInfo)[i].stage = module.shader_stage;
        (*ppShaderStageCreateInfo)[i].module = (*ppShaderModule)[i];
        (*ppShaderStageCreateInfo)[i].pName = "main";
        (*ppShaderStageCreateInfo)[i].pSpecializationInfo = NULL;

        Uint32 var_count = 0;
        spv_result = spvReflectEnumerateDescriptorBindings(&module, &var_count, NULL);
        SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);
        SpvReflectDescriptorBinding** bindings = (SpvReflectDescriptorBinding**)SDL_malloc(var_count * sizeof(SpvReflectDescriptorBinding*));
        spv_result = spvReflectEnumerateDescriptorBindings(&module, &var_count, bindings);
        SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

        // if (module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) var_count--;

        for (j = 0;j < var_count;j++)
        {
            ppSetLayoutBinding[bindings[j]->set][bindingsCount[bindings[j]->set]].binding = bindings[j]->binding;
            ppSetLayoutBinding[bindings[j]->set][bindingsCount[bindings[j]->set]].descriptorType = bindings[j]->descriptor_type;
            ppSetLayoutBinding[bindings[j]->set][bindingsCount[bindings[j]->set]].descriptorCount = bindings[j]->count;
            ppSetLayoutBinding[bindings[j]->set][bindingsCount[bindings[j]->set]].stageFlags = module.shader_stage;
            ppSetLayoutBinding[bindings[j]->set][bindingsCount[bindings[j]->set]].pImmutableSamplers = NULL;
            bindingsCount[bindings[j]->set] += 1;
            biggestSet = SDL_max(biggestSet, bindings[j]->set);
        }
        
        var_count = 0;
        spv_result = spvReflectEnumeratePushConstantBlocks(&module, &var_count, NULL);
        SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);
        SpvReflectBlockVariable** pushconstants = (SpvReflectBlockVariable**)SDL_malloc(var_count * sizeof(SpvReflectBlockVariable*));
        spv_result = spvReflectEnumeratePushConstantBlocks(&module, &var_count, pushconstants);
        SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

        if (var_count != 0)
        {
            pPushConstantRange[pushConstantCount].stageFlags = module.shader_stage;
            pPushConstantRange[pushConstantCount].offset = pushConstantOffset;
            pPushConstantRange[pushConstantCount].size = pushconstants[0]->size;
            pushConstantOffset += pushconstants[0]->size;
            pushConstantCount++;
        }

        spvReflectDestroyShaderModule(&module);

        SDL_free(pushconstants);
        SDL_free(bindings);
        SDL_free(shaderCode);
        SDL_CloseIO(shaderFile);
    }
    CO_addShaderStageCreateInfo(*ppShaderStageCreateInfo);// CO

    setCount = biggestSet + 1;
    *ppDescriptorSetLayout = (VkDescriptorSetLayout*)SDL_malloc(setCount * sizeof(VkDescriptorSetLayout));

    for (i = 0;i < setCount;i++)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pNext = NULL;
        layoutInfo.flags = 0;
        layoutInfo.bindingCount = bindingsCount[i];
        layoutInfo.pBindings = ppSetLayoutBinding[i];

        vkCreateDescriptorSetLayout(allInOne.device, &layoutInfo, allInOne.pAllocationCallbacks, (*ppDescriptorSetLayout) + i);
    }
    CO_addDescriptorSetLayout(setCount, (*ppDescriptorSetLayout));// CO

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = NULL;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = setCount;
    pipelineLayoutCreateInfo.pSetLayouts = *ppDescriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantCount;
    pipelineLayoutCreateInfo.pPushConstantRanges = pPushConstantRange;

    vkCreatePipelineLayout(allInOne.device, &pipelineLayoutCreateInfo, allInOne.pAllocationCallbacks, pPipelineLayout);
    CO_addPieplineLayout(*pPipelineLayout);

    // VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * setCount * sizeof(VkDescriptorSetLayout));
    // for (i = 0;i < setCount;i++)
    // {
    //     for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
    //     {
    //         layouts[i * MAX_FRAMES_IN_FLIGHT + j] = (*ppDescriptorSetLayout)[i];
    //     }
    // }
    // *ppDescriptorSets = (VkDescriptorSet *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * setCount * sizeof(VkDescriptorSet));

    // VkDescriptorSetAllocateInfo allocInfo = {};
    // allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    // allocInfo.pNext = NULL;
    // allocInfo.descriptorSetCount = (Uint32)(MAX_FRAMES_IN_FLIGHT * setCount);
    // allocInfo.descriptorPool = *pDescriptorPool;
    // allocInfo.pSetLayouts = layouts;

    // vkAllocateDescriptorSets(allInOne.device, &allocInfo, *ppDescriptorSets);

    // SDL_free(layouts);
    SDL_free(pPushConstantRange);
    SDL_free(*ppShaderModule);

    return setCount;
}