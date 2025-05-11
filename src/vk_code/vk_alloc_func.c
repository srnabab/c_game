#include "vk_code_h/vk_alloc_func.h"
#include "G_allocator.h"

void * VK_alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)allocationScope;

    return G_aligned_alloc(alignment, size);
}

void * VK_realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    (void)allocationScope;

    return G_aligned_realloc(pOriginal, size, alignment);
}

void VK_free(void * pUserData, void * pMemory)
{
    if (pMemory) G_aligned_free(pMemory);
}