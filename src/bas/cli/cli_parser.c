#include "cli_parser.h"

#include "args.h"

#include "../util/prefix_map.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_HANDLERS_CAPACITY 16

// Alias entry structure
typedef struct {
    char* value;  // Alias value (owned by this structure)
} alias_entry_t;

// Free alias entry
static void free_alias_entry(void* value) {
    alias_entry_t* entry = (alias_entry_t*)value;
    if (entry) {
        free(entry->value);
        free(entry);
    }
}

// Default argument parser using args module
static int default_args_parser(void* enum_func, void* enum_context, void* item_context, 
                                const char* line, int* argc, char*** argv) {
    typedef int (*enum_fn)(void*, const char*, const char*, enum_callback, void*);
    return args_parse((enum_fn)enum_func, enum_context, item_context, line, argc, argv);
}

cli_parser_t* cli_parser_create(cli_alias_resolver alias_resolver, cli_args_parser args_parser) {
    cli_parser_t* parser = calloc(1, sizeof(cli_parser_t));
    if (!parser) {
        return NULL;
    }
    
    parser->alias_resolver = alias_resolver;
    parser->args_parser = args_parser ? args_parser : default_args_parser;
    parser->handlers_capacity = INITIAL_HANDLERS_CAPACITY;
    parser->handlers = calloc(parser->handlers_capacity, sizeof(cli_handler_t));
    if (!parser->handlers) {
        free(parser);
        return NULL;
    }
    
    // Create alias map (using prefix_map as trie-like structure)
    // If no external alias resolver is provided, use internal one
    if (!alias_resolver) {
        parser->alias_map = prefix_map_create_ex(free_alias_entry);
        if (!parser->alias_map) {
            free(parser->handlers);
            free(parser);
            return NULL;
        }
        // Don't set alias_resolver - we'll check alias_map directly in execute_statement
    } else {
        parser->alias_map = NULL;
    }
    
    return parser;
}

void cli_parser_free(cli_parser_t* parser) {
    if (!parser) {
        return;
    }
    
    free(parser->handlers);
    if (parser->alias_map) {
        prefix_map_free(parser->alias_map);
    }
    free(parser);
}

int cli_parser_register_handler(cli_parser_t* parser, cli_handler_t* handler) {
    if (!parser || !handler) {
        return -1;
    }
    
    // Grow handlers array if needed
    if (parser->num_handlers >= parser->handlers_capacity) {
        int new_capacity = parser->handlers_capacity * 2;
        cli_handler_t* new_handlers = realloc(parser->handlers, new_capacity * sizeof(cli_handler_t));
        if (!new_handlers) {
            return -1;
        }
        parser->handlers = new_handlers;
        parser->handlers_capacity = new_capacity;
    }
    
    parser->handlers[parser->num_handlers++] = *handler;
    return 0;
}

void cli_parser_set_error_callback(cli_parser_t* parser, void (*callback)(const char* message, void* context), void* context) {
    if (!parser) {
        return;
    }
    
    parser->error_callback = callback;
    parser->error_context = context;
}

void cli_parser_set_args_context(cli_parser_t* parser, void* enum_func, void* enum_context, void* item_context) {
    if (!parser) {
        return;
    }
    
    parser->enum_func = enum_func;
    parser->enum_context = enum_context;
    parser->item_context = item_context;
}

static void emit_error(cli_parser_t* parser, const char* message) {
    if (parser->error_callback) {
        parser->error_callback(message, parser->error_context);
    } else {
        fprintf(stderr, "%s\n", message);
    }
}

// Trim leading and trailing whitespace
static char* trim_whitespace(char* str) {
    if (!str) {
        return NULL;
    }
    
    // Trim leading whitespace
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    
    // Trim trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    
    return str;
}

// Skip comments (lines starting with #)
static const char* skip_comments(const char* line) {
    if (!line) {
        return NULL;
    }
    
    // Skip leading whitespace
    while (*line && isspace((unsigned char)*line)) {
        line++;
    }
    
    // Check if line starts with #
    if (*line == '#') {
        return NULL;  // Entire line is a comment
    }
    
    return line;
}

int cli_parser_parse_redirections(char* cmd_trim, cli_redirection_t* redir) {
    if (!cmd_trim || !redir) {
        return -1;
    }
    
    // Initialize redirection structure
    memset(redir, 0, sizeof(cli_redirection_t));
    
    char* gt_pos = NULL;
    
    // Look for >> first (append mode)
    // Try " >> " (with spaces)
    gt_pos = strstr(cmd_trim, " >> ");
    if (gt_pos) {
        redir->append_mode = 1;
        *gt_pos = '\0';
        redir->output_file = trim_whitespace(gt_pos + 4);
    } else {
        // Try ">> " (space after)
        gt_pos = strstr(cmd_trim, ">> ");
        if (gt_pos) {
            redir->append_mode = 1;
            *gt_pos = '\0';
            redir->output_file = trim_whitespace(gt_pos + 3);
        } else {
            // Try " >>" (space before)
            gt_pos = strstr(cmd_trim, " >>");
            if (gt_pos) {
                redir->append_mode = 1;
                *gt_pos = '\0';
                redir->output_file = trim_whitespace(gt_pos + 3);
            } else {
                // Try ">>" (no spaces)
                gt_pos = strstr(cmd_trim, ">>");
                if (gt_pos) {
                    redir->append_mode = 1;
                    *gt_pos = '\0';
                    redir->output_file = trim_whitespace(gt_pos + 2);
                }
            }
        }
    }
    
    // If not found, try single >
    if (!gt_pos) {
        // Try " > " (with spaces)
        gt_pos = strstr(cmd_trim, " > ");
        if (gt_pos) {
            redir->append_mode = 0;
            *gt_pos = '\0';
            redir->output_file = trim_whitespace(gt_pos + 3);
        } else {
            // Try "> " (space after)
            gt_pos = strstr(cmd_trim, "> ");
            if (gt_pos) {
                redir->append_mode = 0;
                *gt_pos = '\0';
                redir->output_file = trim_whitespace(gt_pos + 2);
            } else {
                // Try " >" (space before)
                gt_pos = strstr(cmd_trim, " >");
                if (gt_pos) {
                    redir->append_mode = 0;
                    *gt_pos = '\0';
                    redir->output_file = trim_whitespace(gt_pos + 2);
                } else {
                    // Try ">" (no spaces) - search from end to find last occurrence
                    char* last_gt = strrchr(cmd_trim, '>');
                    if (last_gt) {
                        // Check if it's not part of a >> sequence
                        if (last_gt == cmd_trim || *(last_gt - 1) != '>') {
                            redir->append_mode = 0;
                            *last_gt = '\0';
                            redir->output_file = trim_whitespace(last_gt + 1);
                        }
                    }
                }
            }
        }
    }
    
    // Look for < (input redirection)
    char* lt_pos = NULL;
    // Try " < " (with spaces)
    lt_pos = strstr(cmd_trim, " < ");
    if (lt_pos) {
        *lt_pos = '\0';
        redir->input_file = trim_whitespace(lt_pos + 3);
    } else {
        // Try "< " (space after)
        lt_pos = strstr(cmd_trim, "< ");
        if (lt_pos) {
            *lt_pos = '\0';
            redir->input_file = trim_whitespace(lt_pos + 2);
        } else {
            // Try " <" (space before)
            lt_pos = strstr(cmd_trim, " <");
            if (lt_pos) {
                *lt_pos = '\0';
                redir->input_file = trim_whitespace(lt_pos + 2);
            } else {
                // Try "<" (no spaces)
                char* last_lt = strrchr(cmd_trim, '<');
                if (last_lt) {
                    *last_lt = '\0';
                    redir->input_file = trim_whitespace(last_lt + 1);
                }
            }
        }
    }
    
    // Look for | (pipe)
    char* pipe_pos = NULL;
    // Try " | " (with spaces)
    pipe_pos = strstr(cmd_trim, " | ");
    if (pipe_pos) {
        *pipe_pos = '\0';
        redir->pipe_command = trim_whitespace(pipe_pos + 3);
    } else {
        // Try "| " (space after)
        pipe_pos = strstr(cmd_trim, "| ");
        if (pipe_pos) {
            *pipe_pos = '\0';
            redir->pipe_command = trim_whitespace(pipe_pos + 2);
        } else {
            // Try " |" (space before)
            pipe_pos = strstr(cmd_trim, " |");
            if (pipe_pos) {
                *pipe_pos = '\0';
                redir->pipe_command = trim_whitespace(pipe_pos + 2);
            } else {
                // Try "|" (no spaces)
                char* last_pipe = strrchr(cmd_trim, '|');
                if (last_pipe) {
                    *last_pipe = '\0';
                    redir->pipe_command = trim_whitespace(last_pipe + 1);
                }
            }
        }
    }
    
    // Trim trailing spaces from command after redirection parsing
    char* cmd_end = cmd_trim + strlen(cmd_trim) - 1;
    while (cmd_end >= cmd_trim && isspace((unsigned char)*cmd_end)) {
        *cmd_end = '\0';
        cmd_end--;
    }
    
    return 0;
}

void cli_parser_free_redirection(cli_redirection_t* redir) {
    if (!redir) {
        return;
    }
    
    // Note: The strings in redir point into the original command string,
    // so we don't free them here. They will be freed when the command string is freed.
    memset(redir, 0, sizeof(cli_redirection_t));
}

static int execute_statement(cli_parser_t* parser, char* statement) {
    if (!parser || !statement) {
        return -1;
    }
    
    // Trim whitespace
    statement = trim_whitespace(statement);
    if (!statement || strlen(statement) == 0) {
        return 0;  // Empty statement, skip
    }
    
    // Skip comments
    const char* non_comment = skip_comments(statement);
    if (!non_comment) {
        return 0;  // Entire statement is a comment
    }
    
    // Make a copy for modification (redirection parsing modifies the string)
    char cmd_copy[1024];
    strncpy(cmd_copy, non_comment, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';
    char* cmd_trim = cmd_copy;
    
    // Parse redirections
    cli_redirection_t redir = {0};
    if (cli_parser_parse_redirections(cmd_trim, &redir) != 0) {
        emit_error(parser, "Failed to parse redirections");
        return -1;
    }
    
    // Extract command name (first word)
    char cmd_name[256] = {0};
    const char* cmd_start = cmd_trim;
    while (*cmd_start && isspace((unsigned char)*cmd_start)) {
        cmd_start++;
    }
    
    const char* cmd_end = cmd_start;
    while (*cmd_end && !isspace((unsigned char)*cmd_end)) {
        cmd_end++;
    }
    
    int cmd_name_len = cmd_end - cmd_start;
    if (cmd_name_len >= sizeof(cmd_name)) {
        cmd_name_len = sizeof(cmd_name) - 1;
    }
    strncpy(cmd_name, cmd_start, cmd_name_len);
    cmd_name[cmd_name_len] = '\0';
    
    if (strlen(cmd_name) == 0) {
        return 0;  // No command name
    }
    
    // Extract arguments
    const char* args_str = NULL;
    if (*cmd_end) {
        args_str = cmd_end + 1;
        while (*args_str && isspace((unsigned char)*args_str)) {
            args_str++;
        }
        if (*args_str == '\0') {
            args_str = NULL;
        }
    }
    
    // Parse arguments using args parser
    int parsed_argc = 0;
    char** parsed_argv = NULL;
    
    if (args_str && strlen(args_str) > 0) {
        if (parser->args_parser(parser->enum_func, parser->enum_context, parser->item_context,
                                args_str, &parsed_argc, &parsed_argv) != 0) {
            emit_error(parser, "Failed to parse command arguments");
            return -1;
        }
    }
    
    // Build argv array with command name prepended (argv[0] = command name)
    int cmd_argc = parsed_argc + 1;
    char* cmd_argv[65];  // Max 64 args + command name
    cmd_argv[0] = cmd_name;  // argv[0] is command name
    
    for (int i = 0; i < parsed_argc; i++) {
        cmd_argv[i + 1] = parsed_argv[i];
    }
    cmd_argv[cmd_argc] = NULL;
    
    // Expand aliases
    int max_alias_expansions = 10;  // Prevent infinite loops
    int expansion_count = 0;
    
    while (expansion_count < max_alias_expansions) {
        const char* alias_value = NULL;
        
        // Check for alias using resolver or internal map
        if (parser->alias_resolver && parser->alias_map == NULL) {
            // External resolver
            alias_value = parser->alias_resolver(cmd_name);
        } else if (parser->alias_map) {
            // Internal alias map
            alias_entry_t* entry = (alias_entry_t*)prefix_map_get(parser->alias_map, cmd_name);
            if (entry) {
                alias_value = entry->value;
            }
        }
        
        if (!alias_value) {
            break;
        }
        
        // Build expanded command
        char expanded_cmd[1024];
        if (parsed_argc > 0) {
            // Reconstruct full command line with expanded alias
            snprintf(expanded_cmd, sizeof(expanded_cmd), "%s", alias_value);
            for (int i = 0; i < parsed_argc; i++) {
                strncat(expanded_cmd, " ", sizeof(expanded_cmd) - strlen(expanded_cmd) - 1);
                strncat(expanded_cmd, parsed_argv[i], sizeof(expanded_cmd) - strlen(expanded_cmd) - 1);
            }
        } else {
            strncpy(expanded_cmd, alias_value, sizeof(expanded_cmd) - 1);
            expanded_cmd[sizeof(expanded_cmd) - 1] = '\0';
        }
        
        // Free old parsed arguments
        if (parsed_argv) {
            args_free(parsed_argv, parsed_argc);
            parsed_argv = NULL;
            parsed_argc = 0;
        }
        
        // Re-parse expanded command
        if (parser->args_parser(parser->enum_func, parser->enum_context, parser->item_context,
                                expanded_cmd, &parsed_argc, &parsed_argv) != 0) {
            emit_error(parser, "Failed to parse expanded alias");
            return -1;
        }
        
        // Update command name
        if (parsed_argc > 0) {
            strncpy(cmd_name, parsed_argv[0], sizeof(cmd_name) - 1);
            cmd_name[sizeof(cmd_name) - 1] = '\0';
            
            // Rebuild argv array
            cmd_argc = parsed_argc;
            for (int i = 0; i < parsed_argc; i++) {
                cmd_argv[i] = parsed_argv[i];
            }
            cmd_argv[cmd_argc] = NULL;
        } else {
            emit_error(parser, "Expanded alias resulted in empty command");
            return -1;
        }
        
        expansion_count++;
    }
    
    if (expansion_count >= max_alias_expansions) {
        emit_error(parser, "Too many alias expansions (possible circular alias)");
        if (parsed_argv) {
            args_free(parsed_argv, parsed_argc);
        }
        return -1;
    }
    
    // Try handlers in order
    bool handled = false;
    for (int i = 0; i < parser->num_handlers; i++) {
        cli_handler_t* handler = &parser->handlers[i];
        if (handler->can_handle && handler->can_handle(handler, cmd_name, cmd_argc, cmd_argv)) {
            int result = handler->handle(handler, cmd_name, cmd_argc, cmd_argv, &redir);
            handled = true;
            
            // Free parsed arguments
            if (parsed_argv) {
                args_free(parsed_argv, parsed_argc);
            }
            
            return result;
        }
    }
    
    // Free parsed arguments
    if (parsed_argv) {
        args_free(parsed_argv, parsed_argc);
    }
    
    // No handler found
    if (!handled) {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "Invalid command: %s", cmd_name);
        emit_error(parser, error_msg);
        return -1;
    }
    
    return 0;
}

int cli_parser_parse_line(cli_parser_t* parser, const char* line) {
    if (!parser || !line) {
        return -1;
    }
    
    // Skip comments
    const char* non_comment = skip_comments(line);
    if (!non_comment) {
        return 0;  // Entire line is a comment
    }
    
    // Make a copy for modification (we'll split by semicolons)
    char line_copy[2048];
    strncpy(line_copy, non_comment, sizeof(line_copy) - 1);
    line_copy[sizeof(line_copy) - 1] = '\0';
    
    // Split by semicolons and execute each statement
    char* statement_start = line_copy;
    char* statement_end;
    
    do {
        // Find next semicolon or end of string
        statement_end = strchr(statement_start, ';');
        char single_statement[1024];
        
        if (statement_end) {
            // Found semicolon - extract statement up to semicolon
            int stmt_len = statement_end - statement_start;
            if (stmt_len >= sizeof(single_statement)) {
                stmt_len = sizeof(single_statement) - 1;
            }
            strncpy(single_statement, statement_start, stmt_len);
            single_statement[stmt_len] = '\0';
            statement_start = statement_end + 1;
        } else {
            // No more semicolons - use rest of line
            strncpy(single_statement, statement_start, sizeof(single_statement) - 1);
            single_statement[sizeof(single_statement) - 1] = '\0';
            statement_start = NULL;
        }
        
        // Execute the statement
        int result = execute_statement(parser, single_statement);
        if (result != 0) {
            return result;
        }
    } while (statement_start != NULL);
    
    return 0;
}


// Alias management functions

int cli_parser_set_alias(cli_parser_t* parser, const char* name, const char* value) {
    if (!parser || !name || !value) {
        return -1;
    }
    
    // If using external alias resolver, can't set aliases
    if (!parser->alias_map) {
        return -1;
    }
    
    // Create alias entry
    alias_entry_t* entry = malloc(sizeof(alias_entry_t));
    if (!entry) {
        return -1;
    }
    
    entry->value = strdup(value);
    if (!entry->value) {
        free(entry);
        return -1;
    }
    
    // Add to map (will replace existing if present)
    prefix_map_add(parser->alias_map, name, entry);
    
    return 0;
}

const char* cli_parser_get_alias(cli_parser_t* parser, const char* name) {
    if (!parser || !name) {
        return NULL;
    }
    
    // If using external alias resolver, use it
    if (parser->alias_resolver && parser->alias_map == NULL) {
        return parser->alias_resolver(name);
    }
    
    // Use internal alias map
    if (!parser->alias_map) {
        return NULL;
    }
    
    alias_entry_t* entry = (alias_entry_t*)prefix_map_get(parser->alias_map, name);
    if (entry) {
        return entry->value;
    }
    
    return NULL;
}

int cli_parser_remove_alias(cli_parser_t* parser, const char* name) {
    if (!parser || !name) {
        return -1;
    }
    
    // If using external alias resolver, can't remove aliases
    if (!parser->alias_map) {
        return -1;
    }
    
    if (!prefix_map_contains(parser->alias_map, name)) {
        return -1;
    }
    
    prefix_map_remove(parser->alias_map, name);
    return 0;
}

// Helper for alias enumeration
typedef struct {
    void (*callback)(const char* name, const char* value, void* user_data);
    void* user_data;
} alias_enum_data_t;

static int alias_enum_helper(const char* name, void* value, void* user_data) {
    alias_enum_data_t* data = (alias_enum_data_t*)user_data;
    alias_entry_t* entry = (alias_entry_t*)value;
    if (data && data->callback && entry) {
        data->callback(name, entry->value, data->user_data);
    }
    return 0;
}

void cli_parser_enumerate_aliases(cli_parser_t* parser, 
                                   void (*callback)(const char* name, const char* value, void* user_data),
                                   void* user_data) {
    if (!parser || !callback) {
        return;
    }
    
    // If using external alias resolver, can't enumerate
    if (!parser->alias_map) {
        return;
    }
    
    alias_enum_data_t enum_data = {callback, user_data};
    prefix_map_foreach(parser->alias_map, alias_enum_helper, &enum_data);
}

void cli_parser_clear_aliases(cli_parser_t* parser) {
    if (!parser || !parser->alias_map) {
        return;
    }
    
    prefix_map_clear(parser->alias_map);
}
