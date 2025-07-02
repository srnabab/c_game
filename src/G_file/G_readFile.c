#include "G_file/G_file.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"

bool readFile(const char * file, char * data, Sint64 * fileSize)
{
    Sint64 result = 0;
    SDL_IOStream * io = SDL_IOFromFile(file, "rb");
    bool success = false;

    if (io == NULL) 
    {
        SDL_Log("Error: Failed to open file %s: %s", file, SDL_GetError());
        goto cleanup;
    }

    if (data == NULL)
    {
        result = SDL_SeekIO(io, 0, SDL_IO_SEEK_END);
        if (result == -1)
        {
            SDL_Log("Error: Failed to seek to end of file %s: %s", file, SDL_GetError());
            goto cleanup;
        }

        result = SDL_TellIO(io);
        if (result == -1)
        {
            SDL_Log("Error: Failed to get file size for %s: %s", file, SDL_GetError());
            goto cleanup;
        }

        *fileSize = result;
        if (result == 0)
        {
            SDL_Log("Warning: File %s is empty.", file);
            success = true; // 视为空文件为成功读取
            goto cleanup;
        }
    }
    else
    {
        result = SDL_SeekIO(io, 0, SDL_IO_SEEK_SET);
        if (result == -1)
        {
            SDL_Log("Error: Failed to seek to beginning of file %s: %s", file, SDL_GetError());
            goto cleanup;
        }

        result = SDL_ReadIO(io, data, *fileSize);
        if (SDL_GetIOStatus(io) == SDL_IO_STATUS_ERROR)
        {
            SDL_Log("Error: Failed to read from file %s: %s", file, SDL_GetError());
            goto cleanup;
        }
    }

    success = true;

    cleanup:
    if (io) if (SDL_CloseIO(io) == false) SDL_Log("Warning: Failed to close file %s: %s", file, SDL_GetError());
    
    return success;
}
