#include "vk_code_h/vk_vertex.h"
#include "vk_code_h/vk_buffer.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

extern VK_ALL allInOne;

void createVertexBuffer(VkBuffer * pVertexBuffer, VkDeviceMemory * pVertexBufferMemory, void ** ppVertexBufferMemMapped, void * data, Uint32 bufferSize, bool staging)
{
    if (staging)
    {
        resultVulkan(createBuffer(pVertexBuffer, pVertexBufferMemory, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, NULL, 0, 0), 0);
        
        vkMapMemory(allInOne.device, *pVertexBufferMemory, 0, bufferSize, 0, ppVertexBufferMemMapped);
        memset(*ppVertexBufferMemMapped, 0, (size_t)bufferSize);
    }
    else
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        resultVulkan(createBuffer(&stagingBuffer, &stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, NULL, 0, 0), 0);

        void * tempData;
        vkMapMemory(allInOne.device, stagingBufferMemory, 0, bufferSize, 0, &tempData);
        memcpy(tempData, data, bufferSize);
        vkUnmapMemory(allInOne.device, stagingBufferMemory);

        resultVulkan(createBuffer(pVertexBuffer, pVertexBufferMemory, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, NULL, 0, 0), 0);

        copyBuffer(NULL, allInOne.graphicCommandPool, &stagingBuffer, pVertexBuffer, bufferSize);
        
        vkDestroyBuffer(allInOne.device, stagingBuffer, allInOne.pAllocationCallbacks);
        vkFreeMemory(allInOne.device, stagingBufferMemory, allInOne.pAllocationCallbacks);

    }
}
void initVertices2(Uint32 width, Uint32 height, Uint32 row, Uint32 column, float depth, Vertex2 * pVertices)
{
    float tileWidth = (width / column) / (float)(height / 2);
    float tileHeight = (height / row) / (float)(height / 2);
    Uint32 i, j;
    float x, y;

    for (i = 0;i < row;i++)
    {
        x = -1.0f;
        y = i * tileHeight - 1.0f;
        for (j = 0;j < column;j++)
        {
            Uint32 index = (i * column + j) * 4;

            pVertices[index].pos[0] = x;
            pVertices[index].pos[1] = y;
            pVertices[index].pos[2] = depth;
            pVertices[index].color[0] = pVertices[index].color[1] = pVertices[index].color[2] = 1.0f;

            index++;
            pVertices[index].pos[0] = x + tileWidth;
            pVertices[index].pos[1] = y;
            pVertices[index].pos[2] = depth;
            pVertices[index].color[0] = pVertices[index].color[1] = pVertices[index].color[2] = 1.0f;

            index++;
            pVertices[index].pos[0] = x + tileWidth;
            pVertices[index].pos[1] = y + tileHeight;
            pVertices[index].pos[2] = depth;
            pVertices[index].color[0] = pVertices[index].color[1] = pVertices[index].color[2] = 1.0f;

            index++;
            pVertices[index].pos[0] = x;
            pVertices[index].pos[1] = y + tileHeight;
            pVertices[index].pos[2] = depth;
            pVertices[index].color[0] = pVertices[index].color[1] = pVertices[index].color[2] = 1.0f;


            x += tileWidth;
        }
    }
}