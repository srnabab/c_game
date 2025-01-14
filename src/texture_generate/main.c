#include "textureG/textureG.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_log.h"

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        SDL_Log(".exe [-F] [FontPath] [-C] [Channels] [-S] [fontSize] (optional) [-H] [HashTablePath] [-P] [PngPath]\n");
        return -1;
    }

    char fontPath[255] = {0};
    char pngPath[255] = {0};
    char hashPath[255] = {0};
    int fontSize = 60;
    int channel = 4;
    for (int i = 1;i < argc;i++)
    {
        if (SDL_strcmp(argv[i], "-F") == 0)
        {
            strcpy(fontPath, argv[++i]);
        }
        else if (SDL_strcmp(argv[i], "-P") == 0)
        {
            strcpy(pngPath, argv[++i]);
        }
        else if (SDL_strcmp(argv[i], "-H") == 0)
        {
            strcpy(hashPath, argv[++i]);
        }
        else if (SDL_strcmp(argv[i], "-S") == 0)
        {
            fontSize = atoi(argv[++i]);
        }
        else if (SDL_strcmp(argv[i], "-C") == 0)
        {
            channel = SDL_atoi(argv[++i]);
        }
    }

    int failed = 0;
    return textureGenerate(fontPath, hashPath, pngPath, channel, fontSize, &failed);
}