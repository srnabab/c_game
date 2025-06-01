#include "G_image.h"
#include "G_texture.h"

int G_loadImage(PathType path, VkFormat format, VkImageAspectFlags flags, const char * innerName, G_DescriptorSets * pDescriptorSets, Uint32 setIndex, void * data, bool draw)
{
    bool result = false;
    result = loadTexture(path, format, flags, innerName, G_getFreeDescriptorSet(pDescriptorSets, setIndex));

    if (result)
    {
        G_Texture_P * pTexture = getTexture(innerName);
        pTexture->draw = draw;

        if (pDescriptorSets->updateFunc != NULL) pDescriptorSets->updateFunc(pTexture, data);
    }

    return result;
}