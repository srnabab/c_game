#include "G_begin_code.h"

#ifndef G_PATH_COMPARE_H
#define G_PATH_COMPARE_H 1

typedef enum _PathType
{
    None,
    Font1,
    FontHashTable1,
    FontPng1,
    LogPath,
    PathPath,
    MainBackgroundMusic1Wav,
    TestWav,
    EmojiHashTable,
    MainFontHashTable,
    EmojiFont,
    MainFont,
    ParticleCompShader,
    ParticleFragShader,
    ParticleVertShader,
    TriangleFragShader,
    TriangleVertShader,
    TextTxt,
    CirclePng,
    EmojiPng,
    IconPng,
    LinePng,
    Loading1Png,
    MainFontPng,
    Non_existPng,
    PixelsPng,
    ExitPng,
    LoadPng,
    SettingPng,
    StartPng,
    TextRectangle1Png,
} PathType;

extern PathType G_CALL pathCompare(char * buffer);

#endif