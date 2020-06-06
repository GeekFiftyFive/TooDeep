#ifndef LOGGER
#define LOGGER

#include <stdbool.h>

typedef enum {LOG_INFO, LOG_WARN, LOG_ERROR} td_logLevel;

void logInfo(const char *, ...);

void logWarn(const char *, ...);

void logError(const char *, ...);

void setLevelEnabled(td_logLevel, bool);

#endif