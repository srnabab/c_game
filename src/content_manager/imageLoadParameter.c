#include "content_manager/content_manager.h"

bool insertIntoImageLoadParameter(int ID)
{
    const char * insertSQL = "INSERT INTO imageLoadParameter (FileID) VALUES = (?);";

    bool finalize = false;
    int res = SQLITE_OK;
    sqlite3_stmt * stmt = NULL;

    if (sqlite3_prepare_v2(db, insertSQL_ID, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        res = false;
        goto cleanup;
    }
    finalize = true;

    res |= sqlite3_bind_int(stmt, 1, ID);
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
