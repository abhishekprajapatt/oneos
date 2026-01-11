#ifndef CLI_H
#define CLI_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

/* Status Codes */
typedef enum
{
    CLI_SUCCESS = 0,
    CLI_ERROR = 1,
    CLI_ERROR_INVALID_ARG = 2,
    CLI_ERROR_NOT_FOUND = 127,
    CLI_ERROR_PERMISSION = 126,
} cli_status_t;

/* Privilege Levels */
typedef enum
{
    PRIV_PUBLIC = 0,
    PRIV_USER = 1,
    PRIV_ADMIN = 2,
    PRIV_SYSTEM = 3,
    PRIV_ROOT = 4,
} priv_level_t;

/* Command Structure */
typedef struct
{
    char name[64];
    char description[256];
    cli_status_t (*execute)(int argc, char **argv);
    priv_level_t required_level;
    int min_args;
    int max_args;
    char usage[256];
} cli_command_t;

/* Parse Result */
typedef struct
{
    char command[64];
    char **args;
    int argc;
} cli_parse_result_t;

/* CLI Context */
typedef struct
{
    priv_level_t current_level;
    char current_user[32];
    char current_dir[512];
    char **history;
    int history_count;
    int history_max;
    bool should_exit;
} cli_context_t;

/* Core Functions */
int cli_init(void);
int cli_parse_arguments(int argc, const char **argv);
int cli_execute_command(const char *command);
int cli_display_help(void);
int cli_display_version(void);
int cli_process_input(const char *input_buffer);

/* Command Management */
void cli_register_command(cli_command_t cmd);
cli_status_t cli_dispatch_command(const char *command, int argc, char **argv);
int cli_command_exists(const char *name);

/* Parsing */
cli_parse_result_t *cli_parse_input(const char *input);
void cli_free_parse_result(cli_parse_result_t *result);

/* Permissions */
int cli_check_permission(priv_level_t required);
priv_level_t cli_get_user_level(const char *username);
void cli_set_user_level(priv_level_t level);

/* Output */
void cli_print_error(const char *format, ...);
void cli_print_success(const char *format, ...);
void cli_print_info(const char *format, ...);
void cli_print_warning(const char *format, ...);

/* Context */
void cli_init_context(void);
void cli_cleanup_context(void);
cli_context_t *cli_get_context(void);

/* History */
void cli_add_history(const char *command);
char *cli_get_history(int index);
int cli_history_count(void);

/* Help */
void cli_show_help(const char *command);
void cli_show_all_commands(void);

/* Utility */
char *cli_get_current_dir(void);
char *cli_get_current_user(void);

#endif
