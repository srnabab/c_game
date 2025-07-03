#include "G_file_type.h"
#include "SDL_stdinc.h"
#include "SDL_time.h"
#include "content_manager/content_manager.h"
#include "G_constants.h"

#include "G_file/G_file.h"

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_iostream.h"

#include "blake3.h"
#include "sqlite3/sqlite3.h"

extern sqlite3 * db;
extern char ContentPath[255];
extern size_t PathBeginLocation;
extern bool existInDatabase[MAX_ROW];
static FileTypeHashTable * fileTypeHash = NULL;

void initFileTypeHashTable(void)
{
    Uint32 i;

    static FileTypeHashTable allFileType[] = {
        {Folder, "", {0}},
        {Obj, ".obj", {0}},
        {Mtl, ".mtl", {0}},
        {Png, ".png", {0}},
        {Tsdi, ".tsdi", {0}},
        {Tsd, ".tsd", {0}},
        {Ttf, ".ttf", {0}},
        {Wav, ".wav", {0}},
        {Shader, ".spv", {0}},
        {Txt, ".txt", {0}},
        {HashTable, ".hash", {0}}, 
        {FileType_Unknown, ".unknow", {0}},
    };

    for (i = 0;i < FileType_Unknown;i++)
    {
        HASH_ADD_STR(fileTypeHash, suffix, allFileType + i);
    }
}
static FileType findFileType(const char * suffix)
{
    if (suffix == NULL) return Folder;
    FileTypeHashTable * temp = NULL;
    HASH_FIND_STR(fileTypeHash, suffix, temp);
    if (temp) return temp->fileType;
    return FileType_Unknown;
}
static int getIDByRelativePath(const char * name, char * UUID)
{
    if (UUID == NULL) return -2;

    static const char *findSQL = "SELECT ID FROM ContentPath WHERE RelativePath = ?;";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, findSQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        unsigned char * str = sqlite3_column_text(stmt, 0);
        memcpy(UUID, str, 38);
    }
    else if (rc == SQLITE_DONE)
    {
        rc = 0;
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        rc = -1;
    }

    sqlite3_finalize(stmt);

    return rc;
}
// get MOdifiedTime in ContentPath by ID
static Sint64 getModifyTimeRelativePath(const char * relativePath)
{
    const char * SQL = "SELECT ModifiedTime FROM ContentPath WHERE RelativePath = ?";
    sqlite3_stmt * stmt;
    int rc = 0;
    Sint64 timeStamp = 0;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, relativePath, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        timeStamp = sqlite3_column_int64(stmt, 0);
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        timeStamp = -1;
    }

    sqlite3_finalize(stmt);
    return timeStamp;
}
static Sint64 getModifyTimeFileName(const char * fname)
{
    const char * SQL = "SELECT ModifiedTime FROM ContentPath WHERE FileName = ?";
    sqlite3_stmt * stmt;
    int rc = 0;
    Sint64 timeStamp = 0;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, fname, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        timeStamp = sqlite3_column_int64(stmt, 0);
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        timeStamp = -1;
    }

    sqlite3_finalize(stmt);
    return timeStamp;
}
// get row count in ContentPath
int getRowsCount(void)
{
    const char * SQL = "SELECT COUNT(*) FROM ContentPath";
    sqlite3_stmt * stmt;
    int rc = 0;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -3;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        rc = sqlite3_column_int(stmt, 0);
    }
    else
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        rc = -1;
    }

    sqlite3_finalize(stmt);
    return rc;
}
static bool ifExistContentPath(void)
{
    const char * SQL = "SELECT TYPE FROM ContentPath WHERE ParentID is NULL;";
    sqlite3_stmt * stmt = NULL;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    int res = sqlite3_step(stmt);
    if (res == SQLITE_ROW)
    {
        res = sqlite3_column_int(stmt, 0);
        if (res == 1) res = true;
        if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));
        return res;
    }
    else
    {
        // SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        if (sqlite3_finalize(stmt)) SDL_Log("Failed to finalize statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
}
static bool insertNodeContentPath(const unsigned char * ID, const unsigned char * parentID, const char * releativePath, const char * fname, int type, Sint64 fileSize\
    , const Uint8 * contentHash, Uint64 modifiedTime, Uint64 lastSeenTime, Uint32 fileType)
{
    static const char *insertSQL_ID = "INSERT INTO ContentPath \
                                    (ID, ParentID, RelativePath, FileName, TYPE, FileSize, ContentHash, ModifiedTime, LastSeenTime, FileType) \
                                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

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

    char * slash = SDL_strrchr(releativePath, SEPRATOR_C);
    if (slash) slash++;
    else slash = releativePath;

    res |= sqlite3_bind_text(stmt, 1, ID, -1, SQLITE_STATIC);
    res |= sqlite3_bind_text(stmt, 2, parentID, -1, SQLITE_STATIC);
    res |= sqlite3_bind_text(stmt, 3, releativePath, -1, SQLITE_STATIC);
    res |= sqlite3_bind_text(stmt, 4, slash, -1, SQLITE_STATIC);
    res |= sqlite3_bind_int(stmt, 5, type);
    res |= sqlite3_bind_int64(stmt, 6, fileSize);
    res |= sqlite3_bind_blob(stmt, 7, contentHash, BLAKE3_OUT_LEN, SQLITE_STATIC);
    res |= sqlite3_bind_int64(stmt, 8, modifiedTime);
    res |= sqlite3_bind_int64(stmt, 9, lastSeenTime);
    res |= sqlite3_bind_int(stmt, 10, fileType);

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
// update LastSeemTime in ContentPath by name 
static void updateLastSeenTime(Sint64 timeStamp, const char * relativePath)
{
    const char * SQL = "UPDATE ContentPath SET LastSeenTime = ? WHERE RelativePath = ?";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int64(stmt, 1, timeStamp);
    sqlite3_bind_text(stmt, 2, relativePath, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    return;
}
// update ModifiedTime in ContentPath by ID
static void updateModifyTime(Sint64 timeStamp, const char * relativePath)
{
    const char * SQL = "UPDATE ContentPath SET ModifiedTime = ? WHERE RelativePath = ? AND ModifiedTime < ?";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int64(stmt, 1, timeStamp);
    sqlite3_bind_text(stmt, 2, relativePath, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, timeStamp);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    return;
}
// update ParentID in ContentPath by ID (parent folder changed)
static void updateParentIDByFileName(const unsigned char * parentID, const char * fname)
{
    const char * SQL = "UPDATE ContentPath SET ParentID = ? WHERE FileName = ?";
    sqlite3_stmt * stmt;

    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK) 
    {
        SDL_Log("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, parentID, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, fname, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        SDL_Log("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    return;
}
static FileType SDLCALL fileTypeJudge(const char * fname)
{
    char * temp;
    char * suffix = SDL_strrchr(fname, '.');

    temp = suffix;
    while (temp)
    {
        *temp = (char)SDL_tolower(*temp);
        temp++;
        if (*temp == '\0') break;
    }

    // SDL_Log("file: %s, suffix: %s", fname, suffix);

    if (suffix)
    {
        return findFileType(suffix);
    }

    return Folder;
}
static bool deleteUnenumeratedRow(Uint64 timestamp)
{
    const char *SQL = "DELETE FROM ContentPath WHERE LastSeenTime < ?;";
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

    res |= sqlite3_bind_int64(stmt, 1, timestamp);
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
static int updateRelativePathFileName(const char * relativePath, const char * fname)
{
    const char *SQL = "UPDATE ContentPath SET RelativePath = ? WHERE FileName = ?;";
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

    res |= sqlite3_bind_text(stmt, 1, relativePath, -1, SQLITE_STATIC);
    res |= sqlite3_bind_text(stmt, 2, fname, -1, SQLITE_STATIC);
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
static int updateContentHashRelativePath(const Uint8 * contentHash, const char * relativePath)
{
    const char *SQL = "UPDATE ContentPath SET ContentHash = ? WHERE RelativePath = ?;";
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

    res |= sqlite3_bind_blob(stmt, 1, contentHash, BLAKE3_OUT_LEN, SQLITE_STATIC);
    res |= sqlite3_bind_text(stmt, 2, relativePath, -1, SQLITE_STATIC);
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
static bool insertIntoTypeTable(FileType fileType, const char * fname, const Uint8 * hash, const unsigned char * uuid)
{
    switch (fileType) 
    {
        case Png:
        insertIntoImageLoadParameter(fname, hash, uuid);

        default:
        ;
    }

    return true;
}
// recursion to get all file and folder in Content Folder and create database
static SDL_EnumerationResult SDLCALL createFolderDatabase(void *userdata, const char *dirname, const char *fname)
{
    if (fname != NULL)
    {
        SDL_PathInfo info = {};
        FileType fileType = Folder;
        Sint64 fileSize = 0;
        Sint64 currentTime;
        char tempPath[255];
        char tempParentPath[255];
        unsigned char UUID[37];
        unsigned char parentUUID[37];
        Uint8 contentHash[BLAKE3_OUT_LEN];
        Uint8 * contentHashPtr = NULL;
        int res = 0;

        res = createNewUUID(UUID);
        if (res) return SDL_ENUM_FAILURE;

        SDL_strlcpy(tempPath, dirname, 255);

        SDL_strlcpy(tempParentPath, dirname + PathBeginLocation, 255);
        char * slash = SDL_strrchr(tempParentPath, SEPRATOR_C);
        if (slash != NULL) *(slash) = '\0';

        SDL_strlcat(tempPath, fname, 255);
        SDL_GetPathInfo(tempPath, &info);

        SDL_GetCurrentTime(&currentTime);

        getIDByRelativePath(tempParentPath, parentUUID);

        if (info.type != SDL_PATHTYPE_DIRECTORY) 
        {
            readFile(tempPath, NULL, &fileSize);
            char * buffer = G_malloc(fileSize);
            readFile(tempPath, buffer, &fileSize);

            blake3HashContent(buffer, fileSize, contentHash);
            G_free(buffer);
            contentHashPtr = contentHash;

            fileType = fileTypeJudge(fname);
        }

        insertNodeContentPath(UUID, parentUUID, tempPath + PathBeginLocation, fname, info.type, fileSize, contentHashPtr\
            , info.modify_time, currentTime, (Uint32)fileType);

        insertIntoTypeTable(fileType, fname, contentHashPtr, UUID);
        
        if (info.type == SDL_PATHTYPE_DIRECTORY)
        {
            SDL_EnumerateDirectory(tempPath, createFolderDatabase, NULL);
        }
    }
    else if (fname == NULL)
    {
        SDL_Log("End");
        return SDL_ENUM_SUCCESS;
    }

    return SDL_ENUM_CONTINUE;
}
// recursion to update all file and folder in Content Folder and create database
static SDL_EnumerationResult SDLCALL updateFolderDatabase(void *userdata, const char *dirname, const char *fname)
{
    if (fname)
    {
        SDL_PathInfo info = {0};
        char tempPath[255];
        char tempParentPath[255];
        SDL_strlcpy(tempPath, dirname, 255);

        SDL_strlcpy(tempParentPath, dirname + PathBeginLocation, 255);
        char * slash = SDL_strrchr(tempParentPath, SEPRATOR_C);
        if (slash != NULL) *(slash) = '\0';

        SDL_strlcat(tempPath, fname, 255);
        SDL_GetPathInfo(tempPath, &info);

        FileType fileType = Folder;
        Sint64 pathModifyTime, fileModifyTime;
        pathModifyTime = getModifyTimeRelativePath(tempPath + PathBeginLocation);
        fileModifyTime = getModifyTimeFileName(fname);

        if (fileModifyTime == -1)
        {
            Sint64 fileSize = 0;
            unsigned char UUID[37];
            unsigned char parentUUID[37];
            Uint8 contentHash[BLAKE3_OUT_LEN];
            Uint8 * contentHashPtr = NULL;
            int res = 0;

            res = createNewUUID(UUID);
            if (res) return SDL_ENUM_FAILURE;

            SDL_GetCurrentTime(&pathModifyTime);

            getIDByRelativePath(tempParentPath, parentUUID);

            if (info.type != SDL_PATHTYPE_DIRECTORY) 
            {
                readFile(tempPath, NULL, &fileSize);
                char * buffer = G_malloc(fileSize);
                readFile(tempPath, buffer, &fileSize);

                blake3HashContent(buffer, fileSize, contentHash);
                G_free(buffer);
                contentHashPtr = contentHash;

                fileType = fileTypeJudge(fname);
            }

            insertNodeContentPath(UUID, parentUUID, tempPath + PathBeginLocation, fname, info.type, fileSize, contentHashPtr\
                , info.modify_time, pathModifyTime, (Uint32)fileType);

            insertIntoTypeTable(fileType, fname, contentHashPtr, UUID);
        }
        else if (pathModifyTime == -1)
        {
            unsigned char parentUUID[37];
            getIDByRelativePath(tempParentPath, parentUUID);

            if (fileModifyTime != info.modify_time)
            {
                Sint64 fileSize = 0;
                Uint8 contentHash[BLAKE3_OUT_LEN];
                Uint8 * contentHashPtr = NULL;

                if (info.type != SDL_PATHTYPE_DIRECTORY) 
                {
                    readFile(tempPath, NULL, &fileSize);
                    char * buffer = G_malloc(fileSize);
                    readFile(tempPath, buffer, &fileSize);

                    blake3HashContent(buffer, fileSize, contentHash);
                    G_free(buffer);
                    contentHashPtr = contentHash;

                    updateContentHashRelativePath(contentHashPtr, tempPath + PathBeginLocation);
                }

                updateModifyTime(info.modify_time, tempPath + PathBeginLocation);
            }

            updateRelativePathFileName(tempPath + PathBeginLocation, fname);
            updateParentIDByFileName(parentUUID, fname);
        }
        else
        {
            if (info.modify_time != fileModifyTime)
            {
                Sint64 fileSize = 0;
                Uint8 contentHash[BLAKE3_OUT_LEN];
                Uint8 * contentHashPtr = NULL;

                if (info.type != SDL_PATHTYPE_DIRECTORY) 
                {
                    readFile(tempPath, NULL, &fileSize);
                    char * buffer = G_malloc(fileSize);
                    readFile(tempPath, buffer, &fileSize);

                    blake3HashContent(buffer, fileSize, contentHash);
                    G_free(buffer);
                    contentHashPtr = contentHash;

                    updateContentHashRelativePath(contentHashPtr, tempPath + PathBeginLocation);
                }

                updateModifyTime(info.modify_time, tempPath + PathBeginLocation);
            }
        }

        SDL_GetCurrentTime(&pathModifyTime);
        updateLastSeenTime(pathModifyTime, tempPath + PathBeginLocation);

        if (info.type == SDL_PATHTYPE_DIRECTORY)
        {
            SDL_EnumerateDirectory(tempPath, updateFolderDatabase, NULL);
        }
    }
    else
    {
        SDL_Log("End");
        return SDL_ENUM_SUCCESS;
    }
    return SDL_ENUM_CONTINUE;
}
bool updateDatabase(DB_Path * pPack)
{
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(ContentPath, &info);

    Sint64 currentTime; 
    SDL_GetCurrentTime(&currentTime);
    bool exist = ifExistContentPath();

    if (exist)
    {
        updateLastSeenTime(currentTime, ContentPath + PathBeginLocation);

        SDL_EnumerateDirectory(ContentPath, updateFolderDatabase, NULL);

        // insertDeletedRowIntoDeletedRow(currentTime);
        deleteUnenumeratedRow(currentTime);
    }
    else
    {
        unsigned char UUID[37];
        createNewUUID(UUID);
        insertNodeContentPath(UUID, NULL, ContentPath + PathBeginLocation, "Content", (int)info.type, 0, NULL\
        , info.modify_time, currentTime, (Uint32)info.type);

        SDL_EnumerateDirectory(ContentPath, createFolderDatabase, pPack);
    }

    return true;
}
