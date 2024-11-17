#ifndef PATH_COMPARE_H
#define PATH_COMPARE_H

typedef enum _PathType
{
    None,
    TextFont,
    TextHashTable,
    TextPng,
    LogPath,
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
} PathType;

PathType pathCompare(char * buffer);

#endif