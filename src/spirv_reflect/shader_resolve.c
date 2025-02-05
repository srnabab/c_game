#include "vulkan/vulkan.h"

#include "spirv_reflect/shader_resolve.h"
#include "spirv_reflect/spirv_reflect.h"

#include "vk_code_h/vk_shader.h"
#include "vk_code_h/vk_struct.h"

#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_assert.h"

#include "G_constants.h"
#include "G_log.h"
#include "G_pop_window.h"
#include "G_file/G_file.h"

extern VK_ALL allInOne;

#include <stdio.h>

void freeEntryName(Uint32 count, char ** ppEntryName)
{
    for (int i = 0;i < count;i++) SDL_free(ppEntryName[i]);
    free(ppEntryName);
}
void CreateShaderModulesAndDescriptorSets(\
PathType * types, VkDescriptorPool * pDescriptorPool, int8_t shaderCount, VkShaderModule ** ppShaderModule,\
 VkPipelineShaderStageCreateInfo ** ppShaderStageCreateInfo, VkDescriptorSetLayout ** ppDescriptorSetLayout,\
  VkPipelineLayout * pPipelineLayout, VkDescriptorSet ** ppDescriptorSets, char *** ppEntryName)
{
    int8_t i, j;
    int8_t setCount = 1;
    int8_t biggestSet = 0;
    int8_t bindingsCount[10] = {};
    int8_t pushConstantCount = 0;
    Uint32 pushConstantOffset = 0;
    VkDescriptorSetLayoutBinding ppSetLayoutBinding[5][5];
    VkPushConstantRange * pPushConstantRange = (VkPushConstantRange*)SDL_malloc(shaderCount * sizeof(VkPushConstantRange));

    SDL_IOStream * shaderFile;

    SpvReflectResult spv_result;
    
    char ** entryName = (char **)SDL_malloc(shaderCount * sizeof(char*));
    for (i = 0;i < shaderCount;i++) entryName[i] = (char*)SDL_malloc(255 * sizeof(char));

    *ppShaderModule = (VkShaderModule*)SDL_malloc(shaderCount * sizeof(VkShaderModule));
    *ppShaderStageCreateInfo = (VkPipelineShaderStageCreateInfo*)SDL_malloc(shaderCount * sizeof(VkPipelineShaderStageCreateInfo));
    *ppDescriptorSetLayout = (VkDescriptorSetLayout*)SDL_malloc(sizeof(VkDescriptorSetLayout));
    
    for (i = 0;i < shaderCount;i++)
    {
        if ((shaderFile = SDL_IOFromFile(getPath(types[i]), "rb+")) == NULL)
        {
            OPEN_FILE_FAILED(types[i]);
            return;
        }

        SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_END);
        long fileSize = SDL_TellIO(shaderFile);
        //printf("\nfileSize: %u\n", fileSize);
        SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_SET);

        char * shaderCode = (char *)SDL_malloc(fileSize * sizeof(char));
        SDL_ReadIO(shaderFile, shaderCode, sizeof(char) * fileSize);

       createShaderModuleFromMem(fileSize, (const Uint32*)shaderCode, (*ppShaderModule) + i);

        SpvReflectShaderModule module;
        spv_result = spvReflectCreateShaderModule(fileSize, shaderCode, &module);
        SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

        SDL_strlcpy(entryName[i], module.entry_point_name, 255);

        (*ppShaderStageCreateInfo)[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        (*ppShaderStageCreateInfo)[i].pNext = NULL;
        (*ppShaderStageCreateInfo)[i].flags = 0;
        (*ppShaderStageCreateInfo)[i].stage = module.shader_stage;
        (*ppShaderStageCreateInfo)[i].module = (*ppShaderModule)[i];
        (*ppShaderStageCreateInfo)[i].pName = entryName[i];
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

    *ppEntryName = entryName;

    setCount = biggestSet + 1;

    for (i = 0;i < setCount;i++)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pNext = NULL;
        layoutInfo.flags = 0;
        layoutInfo.bindingCount = bindingsCount[i];
        layoutInfo.pBindings = ppSetLayoutBinding[i];

        vkCreateDescriptorSetLayout(*allInOne.pDevice, &layoutInfo, allInOne.pAllocationCallbacks, (*ppDescriptorSetLayout) + i);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = NULL;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = setCount;
    pipelineLayoutCreateInfo.pSetLayouts = *ppDescriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantCount;
    pipelineLayoutCreateInfo.pPushConstantRanges = pPushConstantRange;

    vkCreatePipelineLayout(*allInOne.pDevice, &pipelineLayoutCreateInfo, allInOne.pAllocationCallbacks, pPipelineLayout);

    VkDescriptorSetLayout * layouts = (VkDescriptorSetLayout *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * setCount * sizeof(VkDescriptorSetLayout));
    for (i = 0;i < setCount;i++)
    {
        for (j = 0;j < MAX_FRAMES_IN_FLIGHT;j++)
        {
            layouts[i * MAX_FRAMES_IN_FLIGHT + j] = (*ppDescriptorSetLayout)[i];
        }
    }
    *ppDescriptorSets = (VkDescriptorSet *)SDL_malloc(MAX_FRAMES_IN_FLIGHT * setCount * sizeof(VkDescriptorSet));

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.descriptorSetCount = (Uint32)(MAX_FRAMES_IN_FLIGHT * setCount);
    allocInfo.descriptorPool = *pDescriptorPool;
    allocInfo.pSetLayouts = layouts;

    VkResult aResult = vkAllocateDescriptorSets(*allInOne.pDevice, &allocInfo, *ppDescriptorSets);
    logMessage("%d", aResult);

    SDL_free(layouts);
    SDL_free(pPushConstantRange);
}

void sprivReflect(PathType type)
{
    SDL_IOStream * shaderFile;
    if ((shaderFile = SDL_IOFromFile(getPath(type), "rb+")) == NULL)
    {
        logMessage("open file %s failed", getPath(type));
        return;
    }

    SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_END);
    long fileSize = SDL_TellIO(shaderFile);
    //printf("\nfileSize: %u\n", fileSize);
    SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_SET);

    char * shaderCode = (char *)SDL_malloc(fileSize * sizeof(char));
    SDL_ReadIO(shaderFile, shaderCode, sizeof(char) * fileSize);

    SpirvReflectExample(shaderCode, fileSize);

    SDL_CloseIO(shaderFile);
    SDL_free(shaderCode);
}

int SpirvReflectExample(const void* spirv_code, size_t spirv_nbytes)
{
    // Generate reflection data for a shader
    SpvReflectShaderModule module;
    SpvReflectResult spv_result = spvReflectCreateShaderModule(spirv_nbytes, spirv_code, &module);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

    printf("\nentry: %s, stage: %u", module.entry_point_name, module.shader_stage);

    // Enumerate and extract shader's input variables
    uint32_t var_count = 0;
    spv_result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectInterfaceVariable** vars = (SpvReflectInterfaceVariable**)SDL_malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
    spv_result = spvReflectEnumerateInputVariables(&module, &var_count, vars);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

    for (int i = 0;i < var_count;i++)
    {
        printf("\ninput variable %d: name: %s, location: %u", i, vars[i]->name, vars[i]->location);
    }
    SDL_free(vars);
    vars = NULL;

    var_count = 0;
    spv_result = spvReflectEnumerateOutputVariables(&module, &var_count, NULL);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);
    vars = (SpvReflectInterfaceVariable**)SDL_malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
    spv_result = spvReflectEnumerateOutputVariables(&module, &var_count, vars);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

    for (int i = 0;i < var_count - 1;i++)
    {
        printf("\noutput variable %d: name: %s, location: %u", i, vars[i]->name, vars[i]->location);
    }

    SDL_free(vars);

    // spv_result = spvReflectEnumerateDescriptorSets(&module, &var_count, NULL);
    // SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);
    // SpvReflectDescriptorSet** sets = (SpvReflectDescriptorSet**)SDL_malloc(var_count * sizeof(SpvReflectDescriptorSet*));
    // spv_result = spvReflectEnumerateDescriptorSets(&module, &var_count, sets);
    // SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

    // for (int i = 0;i < var_count;i++)
    // {
    //     logMessage("sets variable %d: name: %u", i, sets[i]->set, sets[i]->bindings);
    // }
    // SDL_free(sets);

    var_count = 0;
    spv_result = spvReflectEnumerateDescriptorBindings(&module, &var_count, NULL);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorBinding** bindings = (SpvReflectDescriptorBinding**)SDL_malloc(var_count * sizeof(SpvReflectDescriptorBinding*));
    spv_result = spvReflectEnumerateDescriptorBindings(&module, &var_count, bindings);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

    for (int i = 0;i < var_count;i++)
    {
        printf("\nbindings variable %d: name: %s, set: %u, binding: %u, count: %u", i, bindings[i]->name, bindings[i]->set, bindings[i]->binding, bindings[i]->count);
    }
    SDL_free(bindings);

    var_count = 0;
    spv_result = spvReflectEnumeratePushConstantBlocks(&module, &var_count, NULL);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectBlockVariable** pushconstants = (SpvReflectBlockVariable**)SDL_malloc(var_count * sizeof(SpvReflectBlockVariable*));
    spv_result = spvReflectEnumeratePushConstantBlocks(&module, &var_count, pushconstants);
    SDL_assert(spv_result == SPV_REFLECT_RESULT_SUCCESS);

    for (int i = 0;i < var_count;i++)
    {
        printf("\npushconstants variable %d: name: %s, size: %u", i, pushconstants[i]->name, pushconstants[i]->size);
    }
    SDL_free(pushconstants);
    // Output variables, descriptor bindings, descriptor sets, and push constants
    // can be enumerated and extracted using a similar mechanism.


    // Destroy the reflection data when no longer required.
    spvReflectDestroyShaderModule(&module);

    return 0;
}