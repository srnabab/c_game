#include "SDL3_mixer/SDL_mixer.h"

#ifndef MUSIC_H
#define MUSIC_H 1

#include "SDL3/SDL_begin_code.h"

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
extern bool SDLCALL loadMusic(char * fileName, char * reName);
extern bool SDLCALL playMusic(char * name);
extern bool SDLCALL unloadMusic(char * name);
extern bool SDLCALL initMusicManagement(void);
extern bool SDLCALL deInitMusicManagement(void);

#include "SDL3/SDL_close_code.h"

#endif