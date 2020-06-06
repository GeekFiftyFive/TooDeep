#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

#define LOG_LEVELS 3

bool infoEnabled = true;
bool warnEnabled = true;
bool errorEnabled = true;

void logInfo(const char *format, ...) {
    if(!infoEnabled) return;
    va_list args;

    va_start(args, format);
    fprintf(stdout, "INFO: ");
    vfprintf(stdout, format, args);
    va_end(args);
}

void logWarn(const char *format, ...) {
    if(!warnEnabled) return;
    va_list args;

    va_start(args, format);
    fprintf(stdout, "WARN: ");
    vfprintf(stdout, format, args);
    va_end(args);
}

void logError(const char *format, ...) {
    if(!errorEnabled) return;
    va_list args;

    va_start(args, format);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, format, args);
    va_end(args);
}

void setLevelEnabled(td_logLevel level, bool enabled) {
    switch(level) {
        case LOG_INFO:
            infoEnabled = enabled;
            break;
        case LOG_WARN:
            warnEnabled = enabled;
            break;
        case LOG_ERROR:
            errorEnabled = enabled;
            break;
        default:
            break;
    }
}