#include "vk_code_h/vk_vertex.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

void createVertexBuffer(VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, void ** ppVertexBufferMemMapped, Uint32 bufferSize)
{
    resultVulkan(createBuffer(pVertexBuffer, pVertexBufferMemory, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, NULL, 0, 0), 0);
    
    vkMapMemory(allInOne.device, *pVertexBufferMemory, 0, bufferSize, 0, ppVertexBufferMemMapped);
    memset(*ppVertexBufferMemMapped, 0, (size_t)bufferSize);
}