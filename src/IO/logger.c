#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

struct td_logger {
    const char *sourceFile;
    FILE *out;
    FILE *err;
};

/**
 * TODO: Use some kind of logging configuration
*/
td_logger createLogger(const char* sourceFile) {
    td_logger logger = malloc(sizeof(struct td_logger));

    logger -> sourceFile = sourceFile;
    logger -> out = stdout;
    logger -> err = stderr;

    return logger;
}

void logInfo(td_logger logger, const char *format, ...) {
    va_list args;

    va_start(args, format);
    fprintf(logger -> out, "INFO: ");
    vfprintf(logger -> out, format, args);
    va_end(args);
}

void logWarn(td_logger logger, const char *format, ...) {
    va_list args;

    va_start(args, format);
    fprintf(logger -> out, "WARN: ");
    vfprintf(logger -> out, format, args);
    va_end(args);
}

void logError(td_logger logger, const char *format, ...) {
    va_list args;

    va_start(args, format);
    fprintf(logger -> err, "ERROR: ");
    vfprintf(logger -> err, format, args);
    va_end(args);
}

void destroyLogger(td_logger logger) {
    free(logger);
}