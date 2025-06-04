#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_thread.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_time.h"
#include "SDL3/SDL_iostream.h"
#include "SDL_complement.h"

#include "G_file/G_file.h"
#include "G_log.h"
#include "G_struct.h"

#include <time.h>

#define MAX_MESSAGE_STORAGE 10000
#define MAX_MESSAGE_IN_FILE 100000
#define MAX_MESSAGE_SIZE 255

static char message[MAX_MESSAGE_STORAGE][MAX_MESSAGE_SIZE];
static uint32_t messageCount = -1;
static uint32_t messagePrintCount = 0;

extern bool game_is_running;

static SDL_Thread * log_thread = NULL;

static SDL_IOStream * log_file = NULL;

static bool disabled = false;

extern G_SYNC allSync;

static char * getCurrentTime(char * buffer, SDL_DateTime dateTime)
{
    memset(buffer, 0, 255);
    SDL_snprintf(buffer, 255, "--%04d-%02d-%02d %02d:%02d:%02d\n", dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, dateTime.second);
    return buffer;
}
static int putMessage_file(void * arg)
{
    char timeBuffer[255];
    SDL_Time ticks;
    SDL_DateTime dateTime = {0};
    while(1)
    {
        if ((messagePrintCount % MAX_MESSAGE_STORAGE == 0) && messagePrintCount != 0)
        {
            uint32_t count = 0;
            while (count != MAX_MESSAGE_STORAGE)
            {
                SDL_WriteIO(log_file, message[count], strlen(message[count]));
                count++;
            }
        }
        if ((messagePrintCount % MAX_MESSAGE_IN_FILE == 0) && messagePrintCount != 0)
        {
            SDL_CloseIO(log_file);
            log_file = SDL_IOFromFile(getPath(PathPath), "r+");
            int fileOpenCount = 0;
            while ((log_file == NULL))
            {
                log_file = SDL_IOFromFile(getPath(PathPath), "r+");
                fileOpenCount++;
                if (fileOpenCount == 10)
                    return -1;
            }

            char buffer[255];
            static int static_index = 0;
            int index;
            while ((SDL_IOgets(buffer, 255, log_file) != NULL))
            {
                buffer[SDL_strcspn(buffer, "\n")] = '\0';
                if (SDL_strcmp(buffer, "[LogPath]") == 0)
                {
                    SDL_IOgets(buffer, 255, log_file);
                    int length = SDL_strlen(buffer);
                    char digits[4];
                    for (int i = 0;i < 4;i++)
                    {
                        digits[i] = 0;
                    }
                    for (int i = 0;i < (int)SDL_strcspn(buffer, ".") - 7;i++)
                    {
                        digits[i] = buffer[7 + i];
                    }
                    index = SDL_atoi(digits) + 1;
                    index = (index > static_index) ? index : static_index++;
                    buffer[7] = '\0';
                    SDL_snprintf(digits, 4, "%d", index);
                    SDL_strlcat(buffer, digits, 4);
                    SDL_strlcat(buffer, ".txt\0\0\0\0\0\0\0\0", 255);
                    SDL_SeekIO(log_file, -length, SDL_IO_SEEK_CUR);
                    SDL_WriteIO(log_file, buffer, SDL_strlen(buffer));
                    break;
                }
            }
            static_index = index;

            SDL_CloseIO(log_file);
            char tempbuffer[255];
            SDL_strlcpy(tempbuffer, getRootPath(), 255);
            SDL_strlcat(tempbuffer, buffer, 255);
            log_file = SDL_IOFromFile(tempbuffer, "w");
            fileOpenCount = 0;
            while ((log_file == NULL))
            {
                log_file = SDL_IOFromFile(tempbuffer, "w");
                fileOpenCount++;
                if (fileOpenCount == 10)
                    return -1;
            }
        }

        SDL_WaitSemaphore(allSync.logSemaphore);
        
        if (!game_is_running && (messageCount + 1 == messagePrintCount))
            break;


        SDL_GetCurrentTime(&ticks);
        SDL_TimeToDateTime(ticks, &dateTime, 1);
        getCurrentTime(timeBuffer, dateTime);
        
        SDL_LockMutex(allSync.printMutex);

        SDL_strlcat(getCurrentTime(timeBuffer, dateTime), "\n", 3);
        SDL_strlcat(message[messagePrintCount % MAX_MESSAGE_STORAGE], timeBuffer, 255);
        messagePrintCount++;

        SDL_UnlockMutex(allSync.printMutex);
    }
    uint32_t count = 0;
    uint32_t maxCount = messagePrintCount % MAX_MESSAGE_STORAGE;
    while (count != maxCount)
    {
        SDL_WriteIO(log_file, message[count], strlen(message[count]));
        count++;
    }
    SDL_CloseIO(log_file);

    return 0;
}
static int putMessage_print(void * arg)
{
    while(1)
    {
        SDL_WaitSemaphore(allSync.logSemaphore);
        
        if (!game_is_running && (messageCount + 1 == messagePrintCount))
            break;

        SDL_LockMutex(allSync.printMutex);

        SDL_Log(message[messagePrintCount % MAX_MESSAGE_STORAGE]);
        messagePrintCount++;

        SDL_UnlockMutex(allSync.printMutex);
    }
    return 0;
}
bool initLog(Uint8 log)
{
    if (!(log & LOG_ENABLED))
        return true;

    if ((log_file = SDL_IOFromFile(getPath(LogPath), "a")) == NULL)
    {
        SDL_Log("open log file failed: %s\n", getPath(LogPath));
        return false;
    }

    if (log & LOG_TXT)
    {
        log_thread = SDL_CreateThread(&putMessage_file, "log_txt", NULL);

        if (log_thread == NULL)
        {
            SDL_CloseIO(log_file);
            return false;
        }
    }
    else
    {
        SDL_CloseIO(log_file);
        log_thread = SDL_CreateThread(&putMessage_print, "log_print", NULL);

        if (log_thread == NULL)
        {
            return false;
        }
    }

    return true;
}
void logMessage(char * format, ...)
{
    if (disabled) return;
    
    va_list arg;

    va_start(arg, format);
    SDL_LockMutex(allSync.logMutex);
    messageCount++;
    SDL_vsnprintf(message[messageCount % MAX_MESSAGE_STORAGE], MAX_MESSAGE_SIZE, format, arg);
    SDL_UnlockMutex(allSync.logMutex);
    SDL_SignalSemaphore(allSync.logSemaphore);

    va_end(arg);
}
void destroyLog(void)
{
    if (allSync.logSemaphore) SDL_SignalSemaphore(allSync.logSemaphore);
    SDL_WaitThread(log_thread, NULL);
    SDL_Log("log end\n");
}