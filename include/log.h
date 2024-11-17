#ifndef LOG_H
#define LOG_H

void initLog(void);
void logMessage(char * format, ...);
int putMessage(void * arg);
void destroyLog(void);

#endif