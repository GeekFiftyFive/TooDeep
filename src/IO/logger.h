#ifndef LOGGER
#define LOGGER

typedef struct td_logger *td_logger;

td_logger createLogger(const char* sourceFile);

void logInfo(td_logger logger, const char *format, ...);

void logWarn(td_logger logger, const char *format, ...);

void logError(td_logger logger, const char *format, ...);

void destroyLogger(td_logger logger);

#endif