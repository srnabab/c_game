#include "file/path_compare.h"

#include "SDL3/SDL_stdinc.h"
PathType pathCompare(char * buffer)
{
    if ((SDL_strcmp(buffer, "[TextFont]") == 0))
    {
        return TextFont;
    }
    else if ((SDL_strcmp(buffer, "[TextHashTable]") == 0))
    {
        return TextHashTable;
    }
    else if ((SDL_strcmp(buffer, "[TextPng]") == 0))
    {
        return TextPng;
    }
    else if ((SDL_strcmp(buffer, "[TriangleVertexShader]") == 0))
    {
        return TriangleVertexShader;
    }
    else if ((SDL_strcmp(buffer, "[TriangleFragmentShader]") == 0))
    {
        return TriangleFragmentShader;
    }
    else if ((SDL_strcmp(buffer, "[ParticleVertexShader]") == 0))
    {
        return ParticleVertexShader;
    }
    else if ((SDL_strcmp(buffer, "[ParticleFragmentShader]") == 0))
    {
        return ParticleFragmentShader;
    }
    else if ((SDL_strcmp(buffer, "[ParticleComputeShader]") == 0))
    {
        return ParticleComputeShader;
    }
    else if ((SDL_strcmp(buffer, "[IconPng]") == 0))
    {
        return IconPng;
    }
    else if ((SDL_strcmp(buffer, "[LoadingPng]") == 0))
    {
        return LoadingPng;
    }
    else if ((SDL_strcmp(buffer, "[CirclePng]") == 0))
    {
        return CirclePng;
    }
    else if ((SDL_strcmp(buffer, "[EmojiFont]") == 0))
    {
        return EmojiFont;
    }
    else if ((SDL_strcmp(buffer, "[EmojiHashTable]") == 0))
    {
        return EmojiHashTable;
    }
    else if ((SDL_strcmp(buffer, "[EmojiPng]") == 0))
    {
        return EmojiPng;
    }
    else if ((SDL_strcmp(buffer, "[BackgroundMusic1]") == 0))
    {
        return BackgroundMusic1;
    }
    else if ((SDL_strcmp(buffer, "[PathPath]") == 0))
    {
        return PathPath;
    }
    else if ((SDL_strcmp(buffer, "[RootPath]") == 0))
    {
        return RootPath;
    }
    else if ((SDL_strcmp(buffer, "[LogPath]") == 0))
    {
        return LogPath;
    }

    return None;
}