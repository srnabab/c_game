#include "file/path_compare.h"

#include <string.h>
PathType pathCompare(char * buffer)
{
    if ((strcmp(buffer, "[TextFont]") == 0))
    {
        return TextFont;
    }
    else if ((strcmp(buffer, "[TextHashTable]") == 0))
    {
        return TextHashTable;
    }
    else if ((strcmp(buffer, "[TextPng]") == 0))
    {
        return TextPng;
    }
    else if ((strcmp(buffer, "[TriangleVertexShader]") == 0))
    {
        return TriangleVertexShader;
    }
    else if ((strcmp(buffer, "[TriangleFragmentShader]") == 0))
    {
        return TriangleFragmentShader;
    }
    else if ((strcmp(buffer, "[ParticleVertexShader]") == 0))
    {
        return ParticleVertexShader;
    }
    else if ((strcmp(buffer, "[ParticleFragmentShader]") == 0))
    {
        return ParticleFragmentShader;
    }
    else if ((strcmp(buffer, "[ParticleComputeShader]") == 0))
    {
        return ParticleComputeShader;
    }
    else if ((strcmp(buffer, "[IconPng]") == 0))
    {
        return IconPng;
    }
    else if ((strcmp(buffer, "[LoadingPng]") == 0))
    {
        return LoadingPng;
    }
    else if ((strcmp(buffer, "[CirclePng]") == 0))
    {
        return CirclePng;
    }
    else if ((strcmp(buffer, "[EmojiFont]") == 0))
    {
        return EmojiFont;
    }
    else if ((strcmp(buffer, "[EmojiHashTable]") == 0))
    {
        return EmojiHashTable;
    }
    else if ((strcmp(buffer, "[EmojiPng]") == 0))
    {
        return EmojiPng;
    }
    else if ((strcmp(buffer, "[PathPath]") == 0))
    {
        return PathPath;
    }
    else if ((strcmp(buffer, "[RootPath]") == 0))
    {
        return RootPath;
    }
    else if ((strcmp(buffer, "[LogPath]") == 0))
    {
        return LogPath;
    }

    return None;
}