#include "vk_code_h/vk_queue.h"
#include "vk_code_h/vk_judge.h"
#include "vk_code_h/vk_all_struct.h"

#include "SDL3/SDL_assert.h"

#include "G_log.h"
#include "G_allocator.h"

extern VK_ALL allInOne;

bool findQueueFamilies(void)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(allInOne.physicalDevice, &queueFamilyCount, NULL);
    print("queueFamilyCount: %u", queueFamilyCount);

    VkQueueFamilyProperties * queueFamily = (VkQueueFamilyProperties *)G_malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(allInOne.physicalDevice, &queueFamilyCount, queueFamily);

    for (uint32_t i = 0;i < queueFamilyCount;i++)
    {
        print("queueFlags: %d", queueFamily[i].queueFlags);
    }
    allInOne.queueFamilyIndices.graphicsFamily.familyIndice = -1;
    allInOne.queueFamilyIndices.computeFamily.familyIndice = -1;
    allInOne.queueFamilyIndices.transferFamily.familyIndice = -1;
    allInOne.queueFamilyIndices.presentFamily.familyIndice = -1;
    bool graphic, compute, transfer, present, sparse, encode, decode;
    for (uint32_t i = 0;i < queueFamilyCount;i++)
    {
        graphic = compute = transfer = present = sparse = encode = decode = false;
        if (queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphic = true;
        }
        if (queueFamily[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            compute = true;
        }
        if (queueFamily[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            transfer = true;
        }
        if (queueFamily[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
        {
            sparse = true;
        }
        if (queueFamily[i].queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
        {
            encode = true;
        }
        if (queueFamily[i].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
        {
            decode = true;
        }
        {
            VkBool32 presentSupport = false;
            resultVulkan(vkGetPhysicalDeviceSurfaceSupportKHR(allInOne.physicalDevice, i, allInOne.surface3D, &presentSupport), 1, queueFamily);
            if (presentSupport)
            {
                present = true;
            }
        }

        if (graphic)
        {
            if (present)
            {
                allInOne.queueFamilyIndices.graphicsFamily.familyIndice = i;
                allInOne.queueFamilyIndices.presentFamily.familyIndice = i;
                if (queueFamily[i].queueCount > GRAPHIC_QUEUE_COUNT + PRESENT_QUEUE_COUNT)
                {
                    if (queueFamily[i].queueCount >= GRAPHIC_QUEUE_COUNT + COMPUTE_QUEUE_COUNT + TRANSFER_QUEUE_COUNT + PRESENT_QUEUE_COUNT)
                    {
                        allInOne.queueFamilyIndices.graphicsFamily.queueCount = GRAPHIC_QUEUE_COUNT + COMPUTE_QUEUE_COUNT + TRANSFER_QUEUE_COUNT;
                    }
                    else if (queueFamily[i].queueCount >= GRAPHIC_QUEUE_COUNT + PRESENT_QUEUE_COUNT + COMPUTE_QUEUE_COUNT)
                    {
                        allInOne.queueFamilyIndices.graphicsFamily.queueCount = queueFamily[i].queueCount - 1;
                    }
                    else if (queueFamily[i].queueCount >= GRAPHIC_QUEUE_COUNT + PRESENT_QUEUE_COUNT + TRANSFER_QUEUE_COUNT)
                    {
                        allInOne.queueFamilyIndices.graphicsFamily.queueCount = queueFamily[i].queueCount - 1;
                    }
                    allInOne.queueFamilyIndices.presentFamily.queueCount = PRESENT_QUEUE_COUNT;
                }
                else if (queueFamily[i].queueCount == GRAPHIC_QUEUE_COUNT + PRESENT_QUEUE_COUNT)
                {
                    allInOne.queueFamilyIndices.graphicsFamily.queueCount = GRAPHIC_QUEUE_COUNT;
                    allInOne.queueFamilyIndices.presentFamily.queueCount = PRESENT_QUEUE_COUNT;
                }
                else if (queueFamily[i].queueCount == 2)
                {
                    allInOne.queueFamilyIndices.presentFamily.queueCount = 1;
                    allInOne.queueFamilyIndices.graphicsFamily.queueCount = 1;
                }
                else
                {
                    allInOne.queueFamilyIndices.presentFamily.familyIndice = -1; 
                    allInOne.queueFamilyIndices.graphicsFamily.queueCount = 1;
                }
            }
        }

        if (compute)
        {
            if (!graphic)
            {
                allInOne.queueFamilyIndices.computeFamily.familyIndice = i;
                if (queueFamily[i].queueCount >= COMPUTE_QUEUE_COUNT)
                {
                    allInOne.queueFamilyIndices.computeFamily.queueCount = COMPUTE_QUEUE_COUNT;
                }
                else
                {
                    allInOne.queueFamilyIndices.computeFamily.queueCount = 1;
                }
            }
        }

        if (transfer)
        {
            if (!graphic && !compute && !decode && !encode)
            {
                allInOne.queueFamilyIndices.transferFamily.familyIndice = i;
                allInOne.queueFamilyIndices.transferFamily.queueCount = TRANSFER_QUEUE_COUNT;
            }
        }
    }

    G_free(queueFamily);

    if (allInOne.queueFamilyIndices.graphicsFamily.familyIndice == -1)
    {
        resultVulkan(VK_ERROR_UNKNOWN, 0);
    }

    return true;
}
static void getGVkQueue( VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, G_VkQueue * pQueue)
{
    vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &pQueue->queue);
    pQueue->mutex = SDL_CreateMutex();
}
void createQueue(void)
{
    Uint32 i, graphicQueueCount, graphicQueueIndex;
    graphicQueueCount = allInOne.queueFamilyIndices.graphicsFamily.queueCount;
    graphicQueueIndex = 0;

    if (allInOne.queueFamilyIndices.graphicsFamily.familyIndice != -1)
    {
        if (allInOne.queueFamilyIndices.graphicsFamily.queueCount < 2)
        {
            getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, 0, &allInOne.pGraphicQueue[0]);

            allInOne.pGraphicQueue[1] = allInOne.pGraphicQueue[0];

            graphicQueueIndex++;
            graphicQueueCount = 0;
        }
        else if (graphicQueueCount >= GRAPHIC_QUEUE_COUNT)
        {
            for (i = 0;i < GRAPHIC_QUEUE_COUNT;i++)
            {
                getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, graphicQueueIndex, &allInOne.pGraphicQueue[i]);

                graphicQueueIndex++;
            }
            graphicQueueCount -= GRAPHIC_QUEUE_COUNT;
        }
    }

    if (allInOne.queueFamilyIndices.transferFamily.familyIndice != -1)
    {
        for (i = 0;i < allInOne.queueFamilyIndices.transferFamily.queueCount;i++)
        {
            getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.transferFamily.familyIndice, i, &allInOne.pTransferQueue[i]);
        }
    }
    else
    {
        if (graphicQueueCount == 0)
        {
            allInOne.pTransferQueue[0] = allInOne.pGraphicQueue[0];
        }
        else if (graphicQueueCount >= TRANSFER_QUEUE_COUNT)
        {
            for (i = 0;i < TRANSFER_QUEUE_COUNT;i++)
            {
                getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, graphicQueueIndex, &allInOne.pTransferQueue[0]);

                graphicQueueIndex++;
            }
            graphicQueueCount -= TRANSFER_QUEUE_COUNT;
        }
        allInOne.queueFamilyIndices.transferFamily.familyIndice = allInOne.queueFamilyIndices.graphicsFamily.familyIndice;
    }

    if (allInOne.queueFamilyIndices.computeFamily.familyIndice != -1)
    {
        for (i = 0;i < allInOne.queueFamilyIndices.computeFamily.queueCount;i++)
        {
            getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.computeFamily.familyIndice, i, allInOne.pComputeQueue + i);
        }

        if (allInOne.queueFamilyIndices.computeFamily.queueCount != 2)
        {
            allInOne.pComputeQueue[1] = allInOne.pComputeQueue[0];
        }
    }
    else
    {
        if (graphicQueueCount == 0)
        {
            allInOne.pComputeQueue[0] = allInOne.pComputeQueue[1] = allInOne.pGraphicQueue[0];
        }
        else if (graphicQueueCount >= COMPUTE_QUEUE_COUNT)
        {
            for (i = 0;i < COMPUTE_QUEUE_COUNT;i++)
            {
                getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, graphicQueueIndex, allInOne.pComputeQueue + i);
                graphicQueueIndex++;
            }
            graphicQueueCount -= COMPUTE_QUEUE_COUNT;
            SDL_assert(graphicQueueCount == 0);
        }
        else if (graphicQueueCount >= 1)
        {
            getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, graphicQueueIndex, allInOne.pComputeQueue + 0);
            for (i = 1;i < COMPUTE_QUEUE_COUNT;i++)
            {
                allInOne.pComputeQueue[i] = allInOne.pComputeQueue[0];
            }
            graphicQueueIndex++;
            graphicQueueCount--;
            SDL_assert(graphicQueueCount == 0);
        }
        allInOne.queueFamilyIndices.computeFamily.familyIndice = allInOne.queueFamilyIndices.graphicsFamily.familyIndice;
    }


    if (allInOne.queueFamilyIndices.presentFamily.familyIndice != -1)
    {
        for (i = 0;i < allInOne.queueFamilyIndices.presentFamily.queueCount;i++)
        {
            getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.presentFamily.familyIndice, graphicQueueIndex, allInOne.pPresentQueue + i);
            graphicQueueIndex++;
        }
    }
    else
    {
        if (graphicQueueCount == 0)
        {
            allInOne.pPresentQueue[0] = allInOne.pGraphicQueue[0];
        }
        else if (graphicQueueCount >= PRESENT_QUEUE_COUNT)
        {
            for (i = 0;i < PRESENT_QUEUE_COUNT;i++)
            {
                getGVkQueue(allInOne.device, allInOne.queueFamilyIndices.graphicsFamily.familyIndice, graphicQueueIndex, allInOne.pPresentQueue + i);
                graphicQueueIndex++;
            }
            graphicQueueCount -= PRESENT_QUEUE_COUNT;
        }
        allInOne.queueFamilyIndices.presentFamily.familyIndice = allInOne.queueFamilyIndices.graphicsFamily.familyIndice;
    }
}
G_VkQueue * getFirstQueueByIndex(Uint32 index)
{
    if (index == allInOne.queueFamilyIndices.graphicsFamily.familyIndice)
    {
        return allInOne.pGraphicQueue;
    }
    else if (index == allInOne.queueFamilyIndices.presentFamily.familyIndice)
    {
        return allInOne.pPresentQueue;
    }
    else if (index == allInOne.queueFamilyIndices.computeFamily.familyIndice)
    {
        return allInOne.pComputeQueue;
    }
    else if (index == allInOne.queueFamilyIndices.transferFamily.familyIndice)
    {
        return allInOne.pTransferQueue;
    }

    return allInOne.pGraphicQueue;
}
VkResult G_vkQueueSubmit(G_VkQueue * queue, Uint32 submitCount, const VkSubmitInfo * pSubmits, VkFence fence)
{
    SDL_LockMutex(queue->mutex);
    VkResult result = vkQueueSubmit(queue->queue, submitCount, pSubmits, fence);
    SDL_UnlockMutex(queue->mutex);

    return result;
}