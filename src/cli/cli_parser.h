#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include "prefix_map.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
struct cli_parser;
struct cli_redirection;

/**
 * Redirection information
 */
typedef struct cli_redirection {
    char* output_file;      // Output file for > or >>
    char* input_file;       // Input file for <
    char* pipe_command;     // Command for | (pipe)
    int append_mode;        // 1 for >>, 0 for >
} cli_redirection_t;

/**
 * Command handler interface
 */
typedef struct cli_handler {
    /**
     * Check if this handler can handle the given command
     * @param handler Handler instance
     * @param cmd_name Command name (first argument)
     * @param argc Number of arguments
     * @param argv Argument array (argv[0] is command name)
     * @return true if this handler can handle the command
     */
    bool (*can_handle)(struct cli_handler* handler, const char* cmd_name, int argc, char** argv);
    
    /**
     * Handle the command
     * @param handler Handler instance
     * @param cmd_name Command name (first argument)
     * @param argc Number of arguments
     * @param argv Argument array (argv[0] is command name)
     * @param redir Redirection information (may be NULL)
     * @return 0 on success, non-zero on error
     */
    int (*handle)(struct cli_handler* handler, const char* cmd_name, int argc, char** argv, cli_redirection_t* redir);
    
    /**
     * Handler-specific context data
     */
    void* context;
} cli_handler_t;

/**
 * Alias resolver function type
 * @param alias_name The alias name to resolve
 * @return The expanded alias value, or NULL if not an alias
 */
typedef const char* (*cli_alias_resolver)(const char* alias_name);

/**
 * Argument parser function type
 * @param enum_func Directory enumeration function for wildcard expansion (may be NULL)
 * @param enum_context Context data for enumeration function
 * @param item_context Context data to pass to enumeration callback
 * @param line The command line string to parse
 * @param argc Output parameter for the number of arguments
 * @param argv Output parameter for the array of argument strings
 * @return 0 on success, -1 on error
 */
typedef int (*cli_args_parser)(void* enum_func, void* enum_context, void* item_context, const char* line, int* argc, char*** argv);

/**
 * CLI parser instance
 */
typedef struct cli_parser {
    // Handler list
    cli_handler_t* handlers;
    int num_handlers;
    int handlers_capacity;
    
    // Alias resolver
    cli_alias_resolver alias_resolver;
    
    // Alias map (internal, using prefix_map as trie-like structure)
    prefix_map* alias_map;
    
    // Argument parser
    cli_args_parser args_parser;
    void* enum_func;
    void* enum_context;
    void* item_context;
    
    // Error callback
    void (*error_callback)(const char* message, void* context);
    void* error_context;
} cli_parser_t;

/**
 * Create a new CLI parser instance
 * @param alias_resolver Function to resolve aliases (may be NULL)
 * @param args_parser Function to parse arguments (may be NULL, will use default)
 * @return New parser instance, or NULL on error
 */
cli_parser_t* cli_parser_create(cli_alias_resolver alias_resolver, cli_args_parser args_parser);

/**
 * Free a CLI parser instance
 * @param parser Parser instance to free
 */
void cli_parser_free(cli_parser_t* parser);

/**
 * Register a command handler
 * @param parser Parser instance
 * @param handler Handler to register
 * @return 0 on success, -1 on error
 */
int cli_parser_register_handler(cli_parser_t* parser, cli_handler_t* handler);

/**
 * Set error callback
 * @param parser Parser instance
 * @param callback Error callback function
 * @param context Context data for callback
 */
void cli_parser_set_error_callback(cli_parser_t* parser, void (*callback)(const char* message, void* context), void* context);

/**
 * Set argument parser context
 * @param parser Parser instance
 * @param enum_func Enumeration function for wildcard expansion
 * @param enum_context Context for enumeration function
 * @param item_context Context for enumeration callback
 */
void cli_parser_set_args_context(cli_parser_t* parser, void* enum_func, void* enum_context, void* item_context);

/**
 * Parse and execute a single line
 * @param parser Parser instance
 * @param line Input line (may contain multiple statements separated by ;)
 * @return 0 on success, -1 on error
 */
int cli_parser_parse_line(cli_parser_t* parser, const char* line);

/**
 * Parse redirections from a command string
 * Modifies the input string by inserting null terminators
 * @param cmd_trim Command string (will be modified)
 * @param redir Output redirection structure (must be initialized to zeros)
 * @return 0 on success, -1 on error
 */
int cli_parser_parse_redirections(char* cmd_trim, cli_redirection_t* redir);

/**
 * Free redirection structure
 * @param redir Redirection structure to free
 */
void cli_parser_free_redirection(cli_redirection_t* redir);

/**
 * Alias management functions
 */

/**
 * Set an alias
 * @param parser Parser instance
 * @param name Alias name
 * @param value Alias value
 * @return 0 on success, -1 on error
 */
int cli_parser_set_alias(cli_parser_t* parser, const char* name, const char* value);

/**
 * Get an alias value
 * @param parser Parser instance
 * @param name Alias name
 * @return Alias value, or NULL if not found
 */
const char* cli_parser_get_alias(cli_parser_t* parser, const char* name);

/**
 * Remove an alias
 * @param parser Parser instance
 * @param name Alias name
 * @return 0 on success, -1 if alias not found
 */
int cli_parser_remove_alias(cli_parser_t* parser, const char* name);

/**
 * Enumerate all aliases
 * @param parser Parser instance
 * @param callback Callback function called for each alias (name, value, user_data)
 * @param user_data User data passed to callback
 */
void cli_parser_enumerate_aliases(cli_parser_t* parser, 
                                   void (*callback)(const char* name, const char* value, void* user_data),
                                   void* user_data);

/**
 * Clear all aliases
 * @param parser Parser instance
 */
void cli_parser_clear_aliases(cli_parser_t* parser);

#ifdef __cplusplus
}
#endif

#endif /* CLI_PARSER_H */

