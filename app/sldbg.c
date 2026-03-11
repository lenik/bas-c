#include <base/stdio.h>
#include <base/str.h>
#include <cli/program.h>
#include <log/log.h>
#include <log/uselog.h>
#include <proc/dbgthread.h>
#include <proc/stackdump.h>

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

static GOptionEntry options[] = {
    OPTION('q', "quiet", "Show less verbose info"),
    OPTION('v', "verbose", "Show more verbose info"),
    OPTION(0, "version", "Show version info"),
    { NULL },
};

gboolean parse_option(const char *opt, const char *val, parse_options_ctx *ctx) {
    return parse_option_defaults(opt, val, ctx);
}

int main(int argc, char **argv) {
    program_title = "sleep sample with debug thread";
    program_help_args = "[TIMEOUT]";
    
    stackdump_install_crash_handler(&stackdump_color_schema_default);
    stackdump_set_interactive(1);

    extern logger_t bas_logger;
    logger_t *loggers[] = { &bas_logger, &LOGGER, 0 };

    parse_options_ctx ctx = {
        .argc = argc,
        .argv = argv,
        .data = NULL,
        .error = NULL,
        .loggers = loggers,
    };

    if (! parse_options(options, &ctx)) {
        errorf("Failed to parse options\n");
        return 1;
    }

    int timeout = 1;
    if (argc > 1)
        timeout = strtol(argv[1], NULL, 10);
    loginfo_fmt("Sleeping for %d seconds", timeout);
    
    loginfo("Starting debug thread");
    void *context = start_dbg_thread();
    if (!context) {
        logerror("Failed to start debug thread");
        return 1;
    }
    
    while (timeout > 0) {
        sleep(1);
        timeout--;
    }

    loginfo("Stopping debug thread");
    stop_dbg_thread(context);
    return 0;
}
