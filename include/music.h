#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "std_c.h"

#ifndef MUSIC_H
#define MUSIC_H

#define MAX_SONG 128

typedef struct _MusicPack
{
    const char * realName;
    const char * name;
    Mix_Music * music;
    bool hadSong;
}MusicPack;

/**
 * \param fileName file's name
 * \param reName name used in application
 */
bool loadMusic(char * fileName, char * reName);
bool playMusic(char * name);
bool unloadMusic(char * name);
bool initMusicManagement(void);
bool deInitMusicManagement(void);

#endif