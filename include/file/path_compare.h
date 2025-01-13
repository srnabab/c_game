#ifndef PATH_COMPARE_H
#define PATH_COMPARE_H

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

PathType pathCompare(char * buffer);

#endif