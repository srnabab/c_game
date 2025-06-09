#include "content_manager/content_manager.h"
#include "G_constants.h"

#include "G_file_type.h"

#include "G_allocator.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_iostream.h"

#include "SDL_stdinc.h"
#include "SDL_timer.h"
#include "sqlite3/sqlite3.h"
#include "sqlite3/sqlite3_alloc_func.h"
#include "uthash/uthash.h"

extern sqlite3 * db;

// judge if table AliasNamePair exist
bool tableExistJudge(void)
{
    static const char * SQL = "SELECT name \
                            FROM sqlite_master \
                            WHERE type='table' \
                            AND name='AliasNamePair';";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);

    return false;
}
// insert to AliasNamePair
bool insertNode_2(const char * alias, const char * name)
{
    static const char * insertSQL = "INSERT OR IGNORE INTO AliasNamePair (Alias, Name) VALUES (?, ?);";
    sqlite3_stmt *stmt = NULL;
    int res = SQLITE_OK;
    bool finalize = false;

    res = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, NULL);
    if (res) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }
    finalize = true;

    res = sqlite3_bind_text(stmt, 1, alias, -1, SQLITE_STATIC);
    if (res)
    {
        SDL_Log("Failed to bind text: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }
    res = sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    if (res)
    {
        SDL_Log("Failed to bind text: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    res = sqlite3_step(stmt);
    if (res != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    cleanup:
    if (finalize) 
    {
        res = sqlite3_finalize(stmt);
        if (res)
        {
            SDL_Log("Failed to finalize statement: %s(%d)\n", sqlite3_errmsg(db), res);
            return false;
        }

        return true;
    }

    return false;
}
