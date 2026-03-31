// NOTICE: This header can be included multiple times.

#include "uselog.h"

#if !defined(LOGGER_NAME) || !defined(LOGGER)
#error "LOGGER_NAME and LOGGER must be defined"
#endif

#define COLOR_ERROR   "\033[1;31m"  // Bright red
#define COLOR_WARN    "\033[1;33m"  // Bright yellow
#define COLOR_MESG    "\033[0m"     // Default/white
#define COLOR_INFO    "\033[1;32m"  // Bright green
#define COLOR_LOG     "\033[1;37m"  // Gray
#define COLOR_DEBUG   "\033[1;90m"  // Light gray
#define COLOR_TRACE   "\033[1;90m"  // Light gray
#define COLOR_RESET   "\033[0m"     // Default/white

#ifdef __cplusplus
#define EXTERN_C extern "C" {
#define END_EXTERN_C }
#else
#define EXTERN_C
#define END_EXTERN_C
#endif

// define the logger instance
#define __STRING_EVAL(x) __STRING(x)

#define _define_logger(name) \
    logger_t __CONCAT(name, _logger) = { \
        .level = 1, \
        .level_default = 1, \
        .header = "[" __STRING_EVAL(name) "]", \
        .color_error = COLOR_ERROR, \
        .color_warn = COLOR_WARN, \
        .color_mesg = COLOR_MESG, \
        .color_info = COLOR_INFO, \
        .color_log = COLOR_LOG, \
        .color_debug = COLOR_DEBUG, \
        .color_trace = COLOR_TRACE, \
        .color_reset = COLOR_RESET, \
    }

#define define_logger() _define_logger(LOGGER_NAME)

#define DEFINE_LOGGER \
    EXTERN_C \
    _define_logger(LOGGER_NAME) \
    END_EXTERN_C
