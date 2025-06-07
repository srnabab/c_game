#ifndef G_FILE_TYPE_H
#define G_FILE_TYPE_H 1

enum _FileType
{
    Folder = 0,
    Obj,
    Mtl,
    Png,
    Tsdi,
    Tsd,
    Ttf,
    Wav,
    Shader,
    Txt,
    HashTable,
    Unknown,
    MAX_TYPE = 0x7FFFFFFF
};
typedef enum _FileType FileType;

#include "SDL3/SDL_begin_code.h"

#include "SDL3/SDL_close_code.h"

#endif // G_file_type.h
