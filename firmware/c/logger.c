#include <stdarg.h>
#include <stdio.h>

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
} log_level;

static void logger_log(log_level level, const char* fmt, va_list args) {
    char* levelPrefix;
    switch (level) {
        case LOG_INFO:
            levelPrefix = "[INFO]::";
            break;
        case LOG_WARN:
            levelPrefix = "[WARN]::";
            break;
        case LOG_ERROR:
            levelPrefix = "[ERROR]::";
            break;
        default:
            levelPrefix = "[UNKNOWN]::";
    }
    printf("%s", levelPrefix);
    vprintf(fmt, args);
    printf("\n");
}

void logger_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log(LOG_INFO, fmt, args);
    va_end(args);
}

void logger_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log(LOG_WARN, fmt, args);
    va_end(args);
}

void logger_err(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logger_log(LOG_ERROR, fmt, args);
    va_end(args);
}
