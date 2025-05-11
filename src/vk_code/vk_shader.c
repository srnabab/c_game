#include "vk_code_h/vk_shader.h"
#include "vk_code_h/vk_all_struct.h"
#include "vk_code_h/vk_judge.h"

#include "SDL3/SDL_iostream.h"

#include "G_file/G_file.h"
#include "G_allocator.h"
#include "G_stack.h"
#include "G_log.h"

extern VK_ALL allInOne;

void createShaderModuleFromFile(PathType type, VkShaderModule * pShaderModule)
{
    SDL_IOStream * shaderFile;
    if ((shaderFile = SDL_IOFromFile(getPath(type), "rb+")) == NULL)
    {
        print("open file %s failed", getPath(type));
        resultVulkan(VK_ERROR_INITIALIZATION_FAILED, 0);
    }

    SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_END);
    long fileSize = SDL_TellIO(shaderFile);
    //printf("fileSize: %u\n", fileSize);
    SDL_SeekIO(shaderFile, 0, SDL_IO_SEEK_SET);

    char * shaderCode = (char *)G_malloc(fileSize * sizeof(char));
    SDL_ReadIO(shaderFile, shaderCode, sizeof(char) * fileSize);
    
    SDL_CloseIO(shaderFile);

    resultVulkan(createShaderModuleFromMem(fileSize, (const Uint32 *)shaderCode, pShaderModule), 1, shaderCode);

    G_free(shaderCode);

    //printf("shaderModule created\n");
}
VkResult createShaderModuleFromMem(size_t codeSize, const Uint32 * pCode, VkShaderModule * pShaderModule)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.codeSize = codeSize;
    createInfo.pCode = pCode;

    return vkCreateShaderModule(allInOne.device, &createInfo, allInOne.pAllocationCallbacks, pShaderModule);
}
void addShaderStageCreateInfo(VkShaderModule * pShaderModule, VkShaderStageFlags stage, uint32_t * pShaderCount, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo)
{
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.pNext = NULL;
    shaderStageCreateInfo.flags = 0;
    shaderStageCreateInfo.stage = stage;
    shaderStageCreateInfo.module = *pShaderModule;
    shaderStageCreateInfo.pName = "main";
    shaderStageCreateInfo.pSpecializationInfo = NULL;

    (*pShaderCount)++;
    //printf("shader count: %u\n", *pShaderCount);

    *pPipelineShaderStageCreateInfo = (VkPipelineShaderStageCreateInfo *)G_realloc(*pPipelineShaderStageCreateInfo, *pShaderCount * sizeof(VkPipelineShaderStageCreateInfo));
    (*pPipelineShaderStageCreateInfo)[*pShaderCount - 1] = shaderStageCreateInfo;
}
/*
void configureShaderStageCreateInfo(VkShaderModule * vertShaderModule, VkShaderModule * fragShaderModule, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo)
{
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
    vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageCreateInfo.pNext = NULL;
    vertShaderStageCreateInfo.flags = 0;
    vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageCreateInfo.module = *vertShaderModule;
    vertShaderStageCreateInfo.pName = "main";
    vertShaderStageCreateInfo.pSpecializationInfo = NULL;

    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
    fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageCreateInfo.pNext = NULL;
    fragShaderStageCreateInfo.flags = 0;
    fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageCreateInfo.module = *fragShaderModule;
    fragShaderStageCreateInfo.pName = "main";
    fragShaderStageCreateInfo.pSpecializationInfo = NULL;

    *pPipelineShaderStageCreateInfo = (VkPipelineShaderStageCreateInfo *)malloc(2 * sizeof(VkPipelineShaderStageCreateInfo));
    (*pPipelineShaderStageCreateInfo)[0] = vertShaderStageCreateInfo;
    (*pPipelineShaderStageCreateInfo)[1] = fragShaderStageCreateInfo;
}*/