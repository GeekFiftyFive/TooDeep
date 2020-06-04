#ifndef LOGGER
#define LOGGER

#include <stdbool.h>

typedef enum {INFO, WARN, ERR} td_logLevel;

typedef struct td_logger *td_logger;

td_logger createLogger(const char*);

void logInfo(td_logger, const char *, ...);

void logWarn(td_logger, const char *, ...);

void logError(td_logger, const char *, ...);

void setLevelEnabled(td_logger, td_logLevel, bool);

void destroyLogger(td_logger);

#endif