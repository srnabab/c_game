#ifndef PATH_COMPARE_H
#define PATH_COMPARE_H

typedef enum _PathType
{
    None,
    TextFont,
    TextHashTable,
    TextPng,
    TriangleVertexShader,
    TriangleFragmentShader,
    ParticleVertexShader,
    ParticleFragmentShader,
    ParticleComputeShader,
    IconPng,
    LoadingPng,
    CirclePng,
    EmojiFont,
    EmojiHashTable,
    EmojiPng,
    PathPath,
    RootPath,
    LogPath,
} PathType;

PathType pathCompare(char * buffer);

#endif