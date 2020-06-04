#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

#define LOG_LEVELS 3

struct td_logger {
    const char *sourceFile;
    FILE *out;
    FILE *err;
    bool levelsEnabled[LOG_LEVELS];
};

/**
 * TODO: Use some kind of logging configuration
*/
td_logger createLogger(const char* sourceFile) {
    td_logger logger = malloc(sizeof(struct td_logger));

    logger -> sourceFile = sourceFile;
    logger -> out = stdout;
    logger -> err = stderr;
    
    for(int i = 0; i < LOG_LEVELS; i++) {
        logger -> levelsEnabled[i] = true;
    }

    return logger;
}

void logInfo(td_logger logger, const char *format, ...) {
    if(!logger -> levelsEnabled[INFO]) return;
    va_list args;

    va_start(args, format);
    fprintf(logger -> out, "INFO: ");
    vfprintf(logger -> out, format, args);
    va_end(args);
}

void logWarn(td_logger logger, const char *format, ...) {
    if(!logger -> levelsEnabled[WARN]) return;
    va_list args;

    va_start(args, format);
    fprintf(logger -> out, "WARN: ");
    vfprintf(logger -> out, format, args);
    va_end(args);
}

void logError(td_logger logger, const char *format, ...) {
    if(!logger -> levelsEnabled[ERR]) return;
    va_list args;

    va_start(args, format);
    fprintf(logger -> err, "ERROR: ");
    vfprintf(logger -> err, format, args);
    va_end(args);
}

void setLevelEnabled(td_logger logger, td_logLevel level, bool enabled) {
    logger -> levelsEnabled[level] = enabled;
}

void destroyLogger(td_logger logger) {
    free(logger);
}