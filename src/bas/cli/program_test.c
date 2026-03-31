#include "program.h"
#include <bas/log/uselog.h>

static GOptionEntry options[] = {
    { "force",     'f', 0, G_OPTION_ARG_NONE, &opt_force,
      "Force to overwrite existing files", },

    { "quiet",     'q', G_OPTION_FLAG_NO_ARG,
      G_OPTION_ARG_CALLBACK, parse_option,
      "Show less verbose info", },

    { "verbose",   'v', G_OPTION_FLAG_NO_ARG,
      G_OPTION_ARG_CALLBACK, parse_option,
      "Show more verbose info", },

    { "version",   '\0', G_OPTION_FLAG_NO_ARG,
      G_OPTION_ARG_CALLBACK, parse_option,
      "Show version info", },

    { NULL },
};

gboolean parse_option(const char *opt, const char *val, parse_options_ctx *ctx) {
    return parse_option_defaults(opt, val, ctx);
}

int main(int argc, char **argv) {
    program_title = "Show line numbers";
    program_help_args = "FILES";

    logger_t *loggers[] = { &LOGGER, 0 };

    parse_options_ctx ctx = {
        .argc = argc,
        .argv = argv,
        .data = NULL,
        .error = NULL,
        .loggers = loggers,
    };

    if (! parse_options(options, &ctx)) {
        logerror("Illegal cmdline syntax.");
        return 1;
    }

    while (--argc > 0) {
        argv++;
        printf("arg: %s\n", *argv);
    }

    return 0;
}
