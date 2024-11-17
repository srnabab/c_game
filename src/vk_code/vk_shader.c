#include "vk_shader.h"

void createShaderModule(VkDevice * pDevice, PathType type, VkShaderModule * pShaderModule)
{
    FuncCode code = createShaderModuleF;

    FILE * shaderFile;
    if ((shaderFile = fopen(getPath(type), "rb+")) == NULL)
    {
        fprintf(stderr, "open file %s failed\n", getPath(type));
        cleanup(createShaderModuleF);
        exit(code + 1000);
    }

    fseek(shaderFile, 0, SEEK_END);
    long fileSize = ftell(shaderFile);
    //printf("fileSize: %u\n", fileSize);
    rewind(shaderFile);

    char * shaderCode = (char *)malloc(fileSize * sizeof(char));
    fread(shaderCode, sizeof(char), fileSize, shaderFile);

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = VK_NULL_HANDLE;
    createInfo.flags = 0;
    createInfo.codeSize = fileSize;
    createInfo.pCode = (const uint32_t *)shaderCode;

    fclose(shaderFile);

    resultVulkan(vkCreateShaderModule(*pDevice, &createInfo, VK_NULL_HANDLE, pShaderModule), code, 1, shaderCode);

    free(shaderCode);

    //printf("shaderModule created\n");
}
void addShaderStageCreateInfo(VkShaderModule * pShaderModule, VkPipelineStageFlags stage, uint32_t * pShaderCount, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo)
{
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.pNext = VK_NULL_HANDLE;
    shaderStageCreateInfo.flags = 0;
    shaderStageCreateInfo.stage = stage;
    shaderStageCreateInfo.module = *pShaderModule;
    shaderStageCreateInfo.pName = "main";
    shaderStageCreateInfo.pSpecializationInfo = VK_NULL_HANDLE;

    (*pShaderCount)++;
    //printf("shader count: %u\n", *pShaderCount);

    *pPipelineShaderStageCreateInfo = (VkPipelineShaderStageCreateInfo *)realloc(*pPipelineShaderStageCreateInfo, *pShaderCount * sizeof(VkPipelineShaderStageCreateInfo));
    (*pPipelineShaderStageCreateInfo)[*pShaderCount - 1] = shaderStageCreateInfo;
}
/*
void configureShaderStageCreateInfo(VkShaderModule * vertShaderModule, VkShaderModule * fragShaderModule, VkPipelineShaderStageCreateInfo ** pPipelineShaderStageCreateInfo)
{
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
    vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageCreateInfo.pNext = VK_NULL_HANDLE;
    vertShaderStageCreateInfo.flags = 0;
    vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageCreateInfo.module = *vertShaderModule;
    vertShaderStageCreateInfo.pName = "main";
    vertShaderStageCreateInfo.pSpecializationInfo = VK_NULL_HANDLE;

    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
    fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageCreateInfo.pNext = VK_NULL_HANDLE;
    fragShaderStageCreateInfo.flags = 0;
    fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageCreateInfo.module = *fragShaderModule;
    fragShaderStageCreateInfo.pName = "main";
    fragShaderStageCreateInfo.pSpecializationInfo = VK_NULL_HANDLE;

    *pPipelineShaderStageCreateInfo = (VkPipelineShaderStageCreateInfo *)malloc(2 * sizeof(VkPipelineShaderStageCreateInfo));
    (*pPipelineShaderStageCreateInfo)[0] = vertShaderStageCreateInfo;
    (*pPipelineShaderStageCreateInfo)[1] = fragShaderStageCreateInfo;
}*/