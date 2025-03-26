#include "G_music.h"
#include "G_stack.h"
#include "G_file.h"

static MusicPack musics[MAX_SONG];
static uint8_t songs = 0;
// static uint8_t song_selected = 0;

static G_Stack stack;

static uint8_t findSong(char * name)
{
    for (int i = 0;i < songs;i++)
    {
        if (!musics[i].hadSong)
        {
            continue;
        }

        if (SDL_strcmp(musics[i].name, name) == 0)
        {
            return i;
        }
    }

    return 128;
}

bool loadMusic(char * fileName, char * reName)
{    
    if (songs == 128)
    {
        return false;
    }

    uint8_t index;
    if (StackIsEmpty(stack))
    {
        index = songs;
        songs++;
    }
    else
    {
        stack.popFn(&stack, &index);
    }

    musics[index].music = Mix_LoadMUS(getPath(MainBackgroundMusic1Wav));
    if (musics[index].music == NULL)
    {
        return false;
    }
    musics[index].realName = fileName;
    musics[index].name = reName;
    musics[index].hadSong = true;

    return true;
}
bool playMusic(char * name)
{
    uint8_t index = findSong(name);
    
    if (index & 0x80)
    {
        return false;
    }

    Mix_HaltMusic();
    if (!Mix_PlayMusic(musics[index].music, -1))
        return false;

    return true;
}
bool unloadMusic(char * name)
{
    uint8_t index = findSong(name);

    if (index & 0x80)
    {
        return false;
    }

    Mix_FreeMusic(musics[index].music);
    musics[index].hadSong = false;
    musics[index].name = NULL;
    musics[index].realName = NULL;

    if (stack.pushFn(&stack, &index))
        return true;
    else
        return false;
}
bool initMusicManagement(void)
{
    if (!initStack(&stack, sizeof(Uint8), NULL, NULL))
        return false;

    Mix_OpenAudio(0, NULL);
    
    return true;
}
bool deInitMusicManagement(void)
{
    Mix_CloseAudio();
    for (int i = 0;i < songs;i++)
    {
        if (musics[i].hadSong)
        {
            Mix_FreeMusic(musics[i].music);
        }
    }
    deInitStack(&stack);

    return true;
}