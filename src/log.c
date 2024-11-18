#include "log.h"
#include "SDL3/SDL.h"
#include "std_c.h"
#include "file.h"
#include <time.h>

#define MAX_MESSAGE_STORAGE 10000
#define MAX_MESSAGE_IN_FILE 100000
#define MAX_MESSAGE_SIZE 256

static char ** message;
static uint32_t messageCount = -1;
static uint32_t messagePrintCount = 0;

static SDL_Mutex * log_mutex = NULL;
static SDL_Mutex * print_mutex = NULL;

static SDL_Semaphore * log_semaphore = NULL;

extern bool game_is_running;

static SDL_Thread * log_thread = NULL;

static FILE * log_file = NULL;

static time_t curtime;
static char * timeNow = NULL;
static char * timeNowPlus2 = NULL;
static size_t timeStr_length = 0;
static size_t timeStr_copy_length = 0;

bool logOutEnabled = 0;
bool logEnabled = 0;

void initLog(void)
{
    if (!logEnabled)
        return;

    if ((log_file = fopen(getPath(LogPath), "a")) == NULL)
    {
        printf("open log file failed: %s\n", getPath(LogPath));
        exit(EXIT_FAILURE);
    }

    time(&curtime);
    char * temp_str = ctime(&curtime);
    timeStr_length = strlen(temp_str) + 2;
    timeStr_copy_length = timeStr_length - 2;
    timeNow = (char*)malloc(timeStr_length * sizeof(char));
    timeNow[0] = ' ';
    timeNow[1] = '-';

    log_mutex = SDL_CreateMutex();
    print_mutex = SDL_CreateMutex();
    log_semaphore = SDL_CreateSemaphore(0);
    //logOutEnabled = false;
    log_thread = SDL_CreateThread(&putMessage, "log", &logOutEnabled);
    message = (char **)malloc(MAX_MESSAGE_STORAGE * sizeof(char *));
    for (uint32_t i = 0;i < MAX_MESSAGE_STORAGE;i++)
    {
        message[i] = (char *)calloc(MAX_MESSAGE_SIZE, sizeof(char));
    }
}
void logMessage(char * format, ...)
{
    if (!logEnabled)
        return;
    
    va_list arg;

    va_start(arg, format);
    SDL_LockMutex(log_mutex);
    messageCount++;
    vsnprintf(message[messageCount % MAX_MESSAGE_STORAGE], MAX_MESSAGE_SIZE * sizeof(char), format, arg);
    SDL_UnlockMutex(log_mutex);
    SDL_SignalSemaphore(log_semaphore);

    va_end(arg);
}
int putMessage(void * arg)
{
    if (*((bool*)arg))
    {
        while(1)
        {
            if ((messagePrintCount % MAX_MESSAGE_IN_FILE == 0) && messagePrintCount != 0)
            {
                fclose(log_file);
                log_file = fopen(getPath(PathPath), "r+");
                int fileOpenCount = 0;
                while ((log_file == NULL))
                {
                    log_file = fopen(getPath(PathPath), "r+");
                    fileOpenCount++;
                    if (fileOpenCount == 10)
                        return -1;
                }

                char buffer[255];
                static int static_index = 0;
                int index;
                while ((fgets(buffer, 255, log_file) != NULL))
                {
                    buffer[strcspn(buffer, "\n")] = '\0';
                    if (strcmp(buffer, "[LogPath]") == 0)
                    {
                        fgets(buffer, 255, log_file);
                        int length = strlen(buffer);
                        char digits[4];
                        for (int i = 0;i < 4;i++)
                        {
                            digits[i] = 0;
                        }
                        for (int i = 0;i < strcspn(buffer, ".") - 7;i++)
                        {
                            digits[i] = buffer[7 + i];
                        }
                        index = atoi(digits) + 1;
                        index = (index > static_index) ? index : static_index++;
                        buffer[7] = '\0';
                        sprintf(digits, "%d", index);
                        strcat(buffer, digits);
                        strcat(buffer, ".txt\0\0\0\0\0\0\0\0");
                        fseek(log_file, -length, SEEK_CUR);
                        fwrite(buffer, strlen(buffer), 1, log_file);
                        break;
                    }
                }
                static_index = index;

                fclose(log_file);
                char tempbuffer[255];
                strcpy(tempbuffer, getPath(RootPath));
                log_file = fopen(strcat(tempbuffer, buffer), "w");
                fileOpenCount = 0;
                while ((log_file == NULL))
                {
                    log_file = fopen(tempbuffer, "w");
                    fileOpenCount++;
                    if (fileOpenCount == 10)
                        return -1;
                }
            }

            SDL_WaitSemaphore(log_semaphore);
            
            if (!game_is_running && (messageCount + 1 == messagePrintCount))
                break;

            SDL_LockMutex(print_mutex);

            time(&curtime);
            timeNowPlus2 = ctime(&curtime);
            strncpy(timeNow + 2, timeNowPlus2, timeStr_copy_length);
            fwrite(message[messagePrintCount % MAX_MESSAGE_STORAGE], 1, strlen(message[messagePrintCount % MAX_MESSAGE_STORAGE]), log_file);
            fwrite(timeNow, 1, timeStr_length, log_file);
            messagePrintCount++;

            SDL_UnlockMutex(print_mutex);
        }
    }
    else
    {
        while(1)
        {
            SDL_WaitSemaphore(log_semaphore);
            
            if (!game_is_running && (messageCount + 1 == messagePrintCount))
                break;

            SDL_LockMutex(print_mutex);

            time(&curtime);
            timeNowPlus2 = ctime(&curtime);
            strncpy(timeNow + 2, timeNowPlus2, timeStr_copy_length);
            puts(message[messagePrintCount % MAX_MESSAGE_STORAGE]);
            messagePrintCount++;

            SDL_UnlockMutex(print_mutex);
        }
    }
    return 0;
}
void destroyLog(void)
{
    SDL_SignalSemaphore(log_semaphore);
    SDL_WaitThread(log_thread, NULL);
    printf("log end\n");
    SDL_DestroySemaphore(log_semaphore);
    fclose(log_file);
}