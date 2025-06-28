#include "SDL3/SDL_stdinc.h"

#include "uthash/uthash.h"

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
    MainBackgroundMusic1Wav,
    TestWav,
    EmojiHashTable,
    MainFontHashTable,
    EmojiFont,
    MainFont,
    BottomObj,
    BottomPng,
    BoxObj,
    BoxPng,
    VoxelMtl,
    VoxelObj,
    VoxelPng,
    CombineFragShader,
    CombineVertShader,
    Combine2dFragShader,
    EmptyFragShader,
    Model3dFragShader,
    Model3dVertShader,
    ModelBottomFragShader,
    ParticleCompShader,
    ParticleFragShader,
    ParticleVertShader,
    ShadowVertShader,
    ShapeFragShader,
    ShapeVertShader,
    Spirv_reflectExe,
    SSGICompShader,
    TriangleFragShader,
    TriangleVertShader,
    TextTxt,
    CirclePng,
    EmojiPng,
    IconPng,
    Loading1Png,
    MainBackgroundPng,
    MainFontPng,
    Non_existPng,
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

 struct _PathTypeHashTable
{    PathType type;
    char str[256];
    UT_hash_handle hh;
};
typedef struct _PathTypeHashTable PathTypeHashTable;

extern PathType SDLCALL pathCompare(char * buffer);
extern void SDLCALL freePathHashTable(void);

#endif // path_compare.h
