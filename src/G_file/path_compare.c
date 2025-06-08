#include "G_file/path_compare.h"

#include "G_allocator.h"

static PathTypeHashTable * root = NULL;

static PathTypeHashTable * hashTable = NULL;

static bool initPathHashTable(void)
{
    const PathTypeHashTable hashTableTemp[] = {
        {Font1, "[Font1]", {0}},
        {FontHashTable1, "[FontHashTable1]", {0}},
        {FontPng1, "[FontPng1]", {0}},
        {DepthImage, "[DepthImage]", {0}},
        {CustomePath1, "[CustomePath1]", {0}},
        {CustomePath2, "[CustomePath2]", {0}},
        {CustomePath3, "[CustomePath3]", {0}},
        {CustomePath4, "[CustomePath4]", {0}},
        {LogPath, "[LogPath]", {0}},
        {PathPath, "[PathPath]", {0}},
        {MainBackgroundMusic1Wav, "[MainBackgroundMusic1Wav]", {0}},
        {TestWav, "[TestWav]", {0}},
        {EmojiHashTable, "[EmojiHashTable]", {0}},
        {MainFontHashTable, "[MainFontHashTable]", {0}},
        {EmojiFont, "[EmojiFont]", {0}},
        {MainFont, "[MainFont]", {0}},
        {BottomObj, "[BottomObj]", {0}},
        {BottomPng, "[BottomPng]", {0}},
        {BoxObj, "[BoxObj]", {0}},
        {BoxPng, "[BoxPng]", {0}},
        {VoxelMtl, "[VoxelMtl]", {0}},
        {VoxelObj, "[VoxelObj]", {0}},
        {VoxelPng, "[VoxelPng]", {0}},
        {CombineFragShader, "[CombineFragShader]", {0}},
        {CombineVertShader, "[CombineVertShader]", {0}},
        {Combine2dFragShader, "[Combine2dFragShader]", {0}},
        {EmptyFragShader, "[EmptyFragShader]", {0}},
        {Model3dFragShader, "[Model3dFragShader]", {0}},
        {Model3dVertShader, "[Model3dVertShader]", {0}},
        {ModelBottomFragShader, "[ModelBottomFragShader]", {0}},
        {ParticleCompShader, "[ParticleCompShader]", {0}},
        {ParticleFragShader, "[ParticleFragShader]", {0}},
        {ParticleVertShader, "[ParticleVertShader]", {0}},
        {ShadowVertShader, "[ShadowVertShader]", {0}},
        {ShapeFragShader, "[ShapeFragShader]", {0}},
        {ShapeVertShader, "[ShapeVertShader]", {0}},
        {Spirv_reflectExe, "[Spirv_reflectExe]", {0}},
        {SSGICompShader, "[SSGICompShader]", {0}},
        {TextTxt, "[TextTxt]", {0}},
        {TriangleFragShader, "[TriangleFragShader]", {0}},
        {TriangleVertShader, "[TriangleVertShader]", {0}},
        {CirclePng, "[CirclePng]", {0}},
        {EmojiPng, "[EmojiPng]", {0}},
        {IconPng, "[IconPng]", {0}},
        {Loading1Png, "[Loading1Png]", {0}},
        {MainBackgroundPng, "[MainBackgroundPng]", {0}},
        {MainFontPng, "[MainFontPng]", {0}},
        {Non_existPng, "[Non_existPng]", {0}},
        {ExitPng, "[ExitPng]", {0}},
        {LoadPng, "[LoadPng]", {0}},
        {SettingPng, "[SettingPng]", {0}},
        {StartPng, "[StartPng]", {0}},
        {TextRectangle1Png, "[TextRectangle1Png]", {0}},
        {TileSet1Png, "[TileSet1Png]", {0}},
        {TileMap1TsdI, "[TileMap1TsdI]", {0}},
        {TileSet1Tsd, "[TileSet1Tsd]", {0}},
    };


    int size = sizeof(hashTableTemp) / sizeof(PathTypeHashTable);
    static int i = 0;
    if (i != 0) return true;

    hashTable = G_malloc(sizeof(hashTableTemp));
    memcpy(hashTable, hashTableTemp, sizeof(hashTableTemp));
    for (i = 0;i < size;i++)
        {
            HASH_ADD_STR(root, str, hashTable + i);
        }
    return false;
}
PathType pathCompare(char * buffer)
{
    initPathHashTable();
    PathTypeHashTable * temp = NULL;
    HASH_FIND_STR(root, buffer, temp);
    if (temp) return temp->type;
    else return None;
}
void freePathHashTable(void)
{
    G_free(hashTable);
}
