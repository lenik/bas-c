#include "program.h"

#include "../base/stdio.h"
#include "../base/str.h"
#include "../log/log.h"
#include "../log/logger.h"

#include <sys/types.h>

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* .section. options */

const char *program_title = "PROGRAM";
const char *program_help_args = "ARGUMENTS";

bool parse_options(GOptionEntry *options, parse_options_ctx *ctx) {
    GError *gerr = NULL;
    GOptionContext *opts;
    int err;

    opts = g_option_context_new(program_help_args);

    GOptionGroup *main_group = g_option_group_new(
        "main", "Main options", 
        NULL, ctx, NULL);

    g_option_group_add_entries(main_group, options);

    g_option_context_set_main_group(opts, main_group);

    if (! g_option_context_parse(opts, &ctx->argc, &ctx->argv, &gerr)) {
        errorf("Couldn't parse options: %s\n", gerr->message);
        g_option_context_free(opts);
        return FALSE;
    }

    g_option_context_free(opts);
    return TRUE;
}

gboolean _g_parse_option(const char *opt, const char *val, 
                       gpointer data, GError **err) {
    parse_options_ctx *ctx = (parse_options_ctx *)data;
    if (ctx == NULL) {
        errorf("parse_options_ctx is NULL\n");
        return FALSE;
    } else {
        // errorf("ctx: %p\n", ctx);
        // errorf("ctx: argc=%d\n", ctx->argc);
        // for (int i = 0; i < ctx->argc; i++) {
        //     errorf("ctx: argv[%d]=%s\n", i, ctx->argv[i]);
        // }
    }
    return parse_option(opt, val, ctx);
}

__attribute__((weak))
gboolean parse_option(const char *opt,
                       const char *val,
                       parse_options_ctx *ctx) {
    logwarn_fmt("parse_option() is not implemented");
    return parse_option_defaults(opt, val, ctx);
}

gboolean parse_option_defaults(const char *opt,
                       const char *val,
                       parse_options_ctx *ctx) {
    assert(*opt == '-');
    opt++;

    bool shortopt = *opt != '-';
    while (*opt == '-') opt++;

    switch (*opt) {
    case 'e':
        if (streq(opt, "error-continue")) {
            opt_error_continue = true;
            return true;
        }
        break;

    case 'f':
        if (shortopt || streq(opt, "force")) {
            opt_force = true;
            return true;
        }
        break;

    case 'q':
        if (shortopt || streq(opt, "quiet")) {
            log_level--;
            for (logger_t **logger = ctx->loggers; *logger; logger++) {
                logger_less(*logger);
            }
            return true;
        }
        break;

    case 'v':
        if (shortopt || streq(opt, "verbose")) {
            log_level++;
            for (logger_t **logger = ctx->loggers; *logger; logger++) {
                logger_more(*logger);
            }
            return true;
        }
        if (streq(opt, "version")) {
            puts(program_title);
            puts("Written by Lenik, (at) 99jsj.com");
            return true;
        }
        break;
    }

    log_err("Bad option: %s %s", opt, val);
    return FALSE;
}

/* .section. logging */

bool opt_error_continue = false;
bool opt_force = false;

/* .section. utils */

bool process_files(char **paths,
                   const char *open_mode,
                   file_handler handler,
                   void *data) {
    assert(paths != NULL);
    assert(open_mode != NULL);
    assert(handler != NULL);

    bool err = false;
    const char *path;

    /* empty path list? read from stdin. */
    if (*paths == NULL)
        return handler("<stdin>", stdin, data);

    for (path = *paths; *path; paths++) {
        FILE *in = fopen(path, open_mode);
        if (in == NULL) {
            log_perr("Can't open file %s", path);
            err = true;
        } else {
            if (! handler(path, in, data))
                err = true;
            fclose(in);
        }

        if (err && !opt_error_continue)
            break;
    }

    return err;
}
