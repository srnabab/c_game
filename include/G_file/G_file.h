#include "G_file/path_compare.h"

#ifndef G_FILE_H
#define G_FILE_H 1

#include "SDL3/SDL_begin_code.h"

#define MAX_PATHLEN 255
#define MAX_FILES 65535

extern int SDLCALL initFileSystem(int argc, char * argv[]);
extern const char* SDLCALL getRootPath(void);
extern const char* SDLCALL getPath(PathType type);

/**
 * first time call get file size, second time get file content 
 * this function will not allocate memory from heap
 * \param file
 * \param data
 * \param fileSize 
 * 
 */
extern bool SDLCALL readFile(const char * file, char * data, Sint64 * fileSize);

#include "SDL3/SDL_close_code.h"

#endif
