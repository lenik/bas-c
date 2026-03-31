#ifndef __CLI_PROGRAM_H
#define __CLI_PROGRAM_H

#include <glib.h>

#include <sys/types.h>

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if !defined(LOGGER_NAME) && !defined(LOGGER)
#error "LOGGER_NAME or LOGGER must be defined"
#endif
#include "../log/uselog.h"

/* .section. options */

#define OPTION(shortopt, longopt, description)  \
    { longopt, shortopt,                        \
            G_OPTION_FLAG_NO_ARG,               \
            G_OPTION_ARG_CALLBACK,              \
            &_g_parse_option,                   \
            description,                        \
            NULL,                               \
            }

#define OPTARG(shortopt, longopt, description, arg_description)  \
    { longopt, shortopt,                        \
            0,                                  \
            G_OPTION_ARG_CALLBACK,              \
            &_g_parse_option,                   \
            description,                        \
            arg_description,                    \
            }

#define OPTZ G_OPTION_ENTRY_NULL

#ifdef __cplusplus
extern "C" {
#endif

extern const char *program_title;
extern const char *program_help_args;

extern bool opt_error_continue;
extern bool opt_force;

typedef struct {
    int argc;
    char **argv;
    gpointer data;
    GError *error;
    logger_t **loggers;
} parse_options_ctx;

bool parse_options(GOptionEntry *options, parse_options_ctx *ctx);

gboolean _g_parse_option(const char *opt, const char *val, gpointer data, GError **err);

gboolean parse_option(const char *opt, const char *val, parse_options_ctx *ctx);

gboolean parse_option_defaults(const char *opt, const char *val, parse_options_ctx *ctx);

typedef bool (*file_handler)(const char *path, FILE *in, void *data);

bool process_files(char **paths, const char *open_mode,
                   file_handler handler, void *data);

/* .sectino. utils */

/* Evaluate and see if the return value matches the given value, if matched,
   return the returned value. Unless opt_force is set to true. */
#define F_EVAL_RET_EQ(expr, chk)                \
    do {                                        \
        int _retval = expr;                     \
        if (_retval == (chk)) {                 \
            if (! opt_force) return _retval;    \
        }                                       \
    } while (0)

/* Evaluate and see if the return value matches the given value, if unmatched,
   return the returned value. Unless opt_force is set to true. */
#define F_EVAL_RET_NEQ(expr, chk)               \
    do {                                        \
        int _retval = expr;                     \
        if (_retval != (chk)) {                 \
            if (! opt_force) return _retval;    \
        }                                       \
    } while (0)

#define F_EVAL_RETZ(expr) F_EVAL_RET_EQ(expr, 0)
#define F_EVAL_RETNZ(expr) F_EVAL_RET_NEQ(expr, 0)

#ifdef __cplusplus
}
#endif

#endif // __CLI_PROGRAM_H