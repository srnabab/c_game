#ifndef LOG_H
#define LOG_H

void initLog(void);
void logMessage(char * format, ...);
int putMessage_file(void * arg);
int putMessage_print(void * arg);
void destroyLog(void);

#endif