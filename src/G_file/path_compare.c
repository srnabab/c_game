#include "G_file/path_compare.h"

#include "SDL3/SDL_stdinc.h"
PathType pathCompare(char * buffer)
{
    if ((SDL_strcmp(buffer, "[Font1]") == 0))
    {
        return Font1;
    }
    else if ((SDL_strcmp(buffer, "[FontHashTable1]") == 0))
    {
        return FontHashTable1;
    }
    else if ((SDL_strcmp(buffer, "[FontPng1]") == 0))
    {
        return FontPng1;
    }
    else if ((SDL_strcmp(buffer, "[LogPath]") == 0))
    {
        return LogPath;
    }
    else if ((SDL_strcmp(buffer, "[PathPath]") == 0))
    {
        return PathPath;
    }
    else if ((SDL_strcmp(buffer, "[MainBackgroundMusic1Wav]") == 0))
    {
        return MainBackgroundMusic1Wav;
    }
    else if ((SDL_strcmp(buffer, "[TestWav]") == 0))
    {
        return TestWav;
    }
    else if ((SDL_strcmp(buffer, "[EmojiHashTable]") == 0))
    {
        return EmojiHashTable;
    }
    else if ((SDL_strcmp(buffer, "[MainFontHashTable]") == 0))
    {
        return MainFontHashTable;
    }
    else if ((SDL_strcmp(buffer, "[EmojiFont]") == 0))
    {
        return EmojiFont;
    }
    else if ((SDL_strcmp(buffer, "[MainFont]") == 0))
    {
        return MainFont;
    }
    else if ((SDL_strcmp(buffer, "[ParticleCompShader]") == 0))
    {
        return ParticleCompShader;
    }
    else if ((SDL_strcmp(buffer, "[ParticleFragShader]") == 0))
    {
        return ParticleFragShader;
    }
    else if ((SDL_strcmp(buffer, "[ParticleVertShader]") == 0))
    {
        return ParticleVertShader;
    }
    else if ((SDL_strcmp(buffer, "[TriangleFragShader]") == 0))
    {
        return TriangleFragShader;
    }
    else if ((SDL_strcmp(buffer, "[TriangleVertShader]") == 0))
    {
        return TriangleVertShader;
    }
    else if ((SDL_strcmp(buffer, "[TextTxt]") == 0))
    {
        return TextTxt;
    }
    else if ((SDL_strcmp(buffer, "[CirclePng]") == 0))
    {
        return CirclePng;
    }
    else if ((SDL_strcmp(buffer, "[EmojiPng]") == 0))
    {
        return EmojiPng;
    }
    else if ((SDL_strcmp(buffer, "[IconPng]") == 0))
    {
        return IconPng;
    }
    else if ((SDL_strcmp(buffer, "[LinePng]") == 0))
    {
        return LinePng;
    }
    else if ((SDL_strcmp(buffer, "[Loading1Png]") == 0))
    {
        return Loading1Png;
    }
    else if ((SDL_strcmp(buffer, "[MainFontPng]") == 0))
    {
        return MainFontPng;
    }
    else if ((SDL_strcmp(buffer, "[Non_existPng]") == 0))
    {
        return Non_existPng;
    }
    else if ((SDL_strcmp(buffer, "[PixelsPng]") == 0))
    {
        return PixelsPng;
    }
    else if ((SDL_strcmp(buffer, "[ExitPng]") == 0))
    {
        return ExitPng;
    }
    else if ((SDL_strcmp(buffer, "[LoadPng]") == 0))
    {
        return LoadPng;
    }
    else if ((SDL_strcmp(buffer, "[SettingPng]") == 0))
    {
        return SettingPng;
    }
    else if ((SDL_strcmp(buffer, "[StartPng]") == 0))
    {
        return StartPng;
    }
    else if ((SDL_strcmp(buffer, "[TextRectangle1Png]") == 0))
    {
        return TextRectangle1Png;
    }

    return None;
}