#include "content_manager/content_manager.h"
#include "SDL3/SDL_log.h"
#include "blake3.h"

extern sqlite3 * db;

bool insertIntoImageLoadParameter(const char * fname, const Uint8 * hash, const unsigned char * uuid)
{
    const char * insertSQL = "INSERT INTO ImageLoadParameter (FileName, ContentHash, FileID) VALUES (?, ?, ?);";

    bool finalize = false;
    int res = SQLITE_OK;
    sqlite3_stmt * stmt = NULL;

    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }
    finalize = true;

    res |= sqlite3_bind_text(stmt, 1, fname, -1, SQLITE_STATIC);
    res |= sqlite3_bind_blob(stmt, 2, hash, BLAKE3_OUT_LEN, SQLITE_STATIC);
    res |= sqlite3_bind_text(stmt, 3, uuid, -1, SQLITE_STATIC);
    if (res)
    {
        SDL_Log("Failed to bind: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }

    res = true;

    cleanup:
    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return res;

}
