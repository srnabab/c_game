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
#include <stdbool.h>
#include <string.h>

extern sqlite3 * db;

int findSameDeletedRow(const char * fileName, int type, Uint32 fileType, char * innerName)
{
    static const char * findSameFileName = "SELECT ID, InnerName FROM DeletedRow WHERE FileName = ? AND TYPE = ? AND FileType = ?;";

    bool finalize = false;
    int res = false;
    int id = -1;
    sqlite3_stmt * stmt = NULL;

    if (sqlite3_prepare_v2(db, findSameFileName, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }
    finalize = true;

    char * slash = SDL_strrchr(fileName, SEPRATOR_C);
    if (slash) slash++;
    else slash = fileName;

    res |= sqlite3_bind_text(stmt, 1, slash, -1, SQLITE_STATIC);
    res |= sqlite3_bind_int(stmt, 2, type);
    res |= sqlite3_bind_int(stmt, 3, fileType);

    if (res)
    {
        SDL_Log("Failed to bind: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        id = sqlite3_column_int(stmt, 0);
        const char * nameGeted = sqlite3_column_text(stmt, 1);
        if (nameGeted) (void)SDL_strlcpy(innerName, nameGeted, SDL_strlen(nameGeted) + 1);
    }
    else if (rc == SQLITE_DONE)
    {
        id = -1;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    cleanup:
    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return id;
}
bool insertDeletedRowIntoDeletedRow(void)
{
    const char * SQL = "INSERT INTO DeletedRow (FileName, InnerName, FileType) \
                        SELECT FileName, InnerName, FileType FROM ContentPath \ 
                        WHERE MARK = 1 AND TYPE = 1;";

    if (sqlite3_exec(db, SQL, NULL, NULL, NULL) != SQLITE_OK)
    {
        SDL_Log("Insert into deleted row failed: %s\n", sqlite3_errmsg(db));
        return false;
    }

    return true;
}
bool insertNode3(int mainTableRowID)
{
    // static const char * insertDeletedRowFromContentPath = "INSERT INTO DeletedRow (FileName, InnerName, TYPE, FileType) 
    //                                             SELECT '%s', InnerName, TYPE, FileType FROM ContentPath 
    //                                             WHERE id = ?;";
    
    char buffer[512];
    sqlite3_stmt * stmt = NULL;
    int res = SQLITE_OK;
    bool finalize = false;

    char * name = getName(mainTableRowID);
    if (name == NULL)
    {
        res = -1;
        goto cleanup;
    }
    char * slash = SDL_strrchr(name, SEPRATOR_C);
    if (slash) slash++;
    else slash = name;

    res = SDL_snprintf(buffer, 511, "INSERT INTO DeletedRow (FileName, InnerName, TYPE, FileType) \
                                                SELECT '%s', InnerName, TYPE, FileType FROM ContentPath \
                                                WHERE id = ?;", slash);
    if (res < 0)
    {
        SDL_Log("Failed to joint SQL statement: %s\n", sqlite3_errmsg(db));
        res = -2;
        goto cleanup;
    }
    G_free(name);
    res = 0;

    if (sqlite3_prepare_v2(db, buffer, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        res = -3;
        goto cleanup;
    }
    finalize = true;

    res |= sqlite3_bind_int(stmt, 1, mainTableRowID);
    if (res)
    {
        SDL_Log("Failed to bind: %s\n", sqlite3_errmsg(db));
        res = -4;
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        res = -5;
        goto cleanup;
    }

    cleanup:

    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return res;
}
bool deleteRow3(int ID)
{
    const char * SQL = "DELETE FROM DeletedRow WHERE ID = ?";
    sqlite3_stmt * stmt = NULL;
    int res = 0;
    bool finalize = false;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        res = -1;
        goto cleanup;
    }
    finalize = true;

    res |= sqlite3_bind_int(stmt, 1, ID);
    if (res)
    {
        SDL_Log("Failed to bind: %s\n", sqlite3_errmsg(db));
        res = -4;
        goto cleanup;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        res = -5;
        goto cleanup;
    }

    cleanup:

    if (finalize) if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));

    return res < 0 ? false : true;
}
