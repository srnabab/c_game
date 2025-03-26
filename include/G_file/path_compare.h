#include "SDL3/SDL_stdinc.h"

#ifndef G_PATH_COMPARE_H
#define G_PATH_COMPARE_H 1

typedef enum _PathType
{
    None,
    Font1,
    FontHashTable1,
    FontPng1,
    DepthImage,
    CustomePath1,
    CustomePath2,
    CustomePath3,
    CustomePath4,
    LogPath,
    PathPath,
    Model3dVertShader,
    Viking_room_CopyPng,
    Cornell_boxObj,
    Viking_roomObj,
    BoxObj,
    BoxPng,
    SSGICompShader,
    BottomObj,
    BottomPng,
    Model3dFragShader,
    Viking_roomPng,
    MainBackgroundMusic1Wav,
    TestWav,
    EmojiHashTable,
    MainFontHashTable,
    EmojiFont,
    MainFont,
    ParticleCompShader,
    ParticleFragShader,
    ParticleVertShader,
    Spirv_reflectExe,
    StartFragShader,
    StartVertShader,
    TriangleFragShader,
    TriangleVertShader,
    TextTxt,
    CirclePng,
    EmojiPng,
    H1w1Png,
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
    TileSet1Png,
    TileMap1TsdI,
    TileSet1Tsd,
    MAX_PATH_TYPE = 0x7FFFFFFF
} PathType;

extern PathType SDLCALL pathCompare(char * buffer);

#endif