#ifndef PATH_COMPARE_H
#define PATH_COMPARE_H

typedef enum _PathType
{
    None,
    Font1,
    Font1HashTable,
    Font1Png,
    TriangleVertexShader,
    TriangleFragmentShader,
    ParticleVertexShader,
    ParticleFragmentShader,
    ParticleComputeShader,
    IconPng,
    Loading1Png,
    CirclePng,
    EmojiFont,
    EmojiHashTable,
    EmojiPng,
    BackgroundMusic1,
    PathPath,
    LogPath,
    CustomePath1,
    CustomePath2,
    CustomePath3,
    CustomePath4,
    CustomePath5,
} PathType;

PathType pathCompare(char * buffer);

#endif