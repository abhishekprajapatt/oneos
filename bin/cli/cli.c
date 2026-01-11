#include "cli.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <unistd.h>

#define MAX_COMMANDS 256
#define MAX_HISTORY 1000
#define MAX_ARG_COUNT 32
#define CMD_BUFFER_SIZE 4096

static cli_context_t g_context = {0};
static cli_command_t g_commands[MAX_COMMANDS] = {0};
static int g_command_count = 0;

/* ============================================
   CONTEXT MANAGEMENT
   ============================================ */

void cli_init_context(void)
{
    memset(&g_context, 0, sizeof(cli_context_t));
    g_context.current_level = PRIV_USER;
    strcpy(g_context.current_user, "oneos");
    strcpy(g_context.current_dir, "/home");
    g_context.history_max = MAX_HISTORY;
    g_context.history = malloc(sizeof(char *) * MAX_HISTORY);
    g_context.history_count = 0;
    g_context.should_exit = false;

    // Set privilege level (simplified for cross-platform)
    g_context.current_level = PRIV_USER;
    #ifdef __linux__
    if (getuid() == 0) {
        g_context.current_level = PRIV_ROOT;
        strcpy(g_context.current_user, "root");
    }
    #endif
}

void cli_cleanup_context(void)
{
    if (g_context.history)
    {
        for (int i = 0; i < g_context.history_count; i++)
        {
            if (g_context.history[i])
            {
                free(g_context.history[i]);
            }
        }
        free(g_context.history);
    }
}

cli_context_t *cli_get_context(void)
{
    return &g_context;
}

/* ============================================
   OUTPUT FUNCTIONS
   ============================================ */

void cli_print_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("\033[0;31m[ERROR]\033[0m ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

void cli_print_success(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("\033[0;32m[OK]\033[0m ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

void cli_print_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("\033[0;34m[INFO]\033[0m ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

void cli_print_warning(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("\033[1;33m[WARN]\033[0m ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/* ============================================
   PERMISSION MANAGEMENT
   ============================================ */

int cli_check_permission(priv_level_t required)
{
    return g_context.current_level >= required;
}

priv_level_t cli_get_user_level(const char *username)
{
    if (strcmp(username, "root") == 0)
        return PRIV_ROOT;
    if (strcmp(username, "admin") == 0)
        return PRIV_ADMIN;
    return PRIV_USER;
}

void cli_set_user_level(priv_level_t level)
{
    g_context.current_level = level;
}

/* ============================================
   COMMAND REGISTRATION & DISPATCH
   ============================================ */

void cli_register_command(cli_command_t cmd)
{
    if (g_command_count >= MAX_COMMANDS)
    {
        cli_print_error("Command registry full");
        return;
    }
    g_commands[g_command_count++] = cmd;
}

int cli_command_exists(const char *name)
{
    for (int i = 0; i < g_command_count; i++)
    {
        if (strcmp(g_commands[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

cli_status_t cli_dispatch_command(const char *command, int argc, char **argv)
{
    int cmd_index = cli_command_exists(command);

    if (cmd_index < 0)
    {
        cli_print_error("Command not found: %s", command);
        return CLI_ERROR_NOT_FOUND;
    }

    cli_command_t *cmd = &g_commands[cmd_index];

    if (!cli_check_permission(cmd->required_level))
    {
        cli_print_error("Permission denied");
        return CLI_ERROR_PERMISSION;
    }

    if (argc < cmd->min_args || (cmd->max_args > 0 && argc > cmd->max_args))
    {
        cli_print_error("Invalid argument count. Usage: %s", cmd->usage);
        return CLI_ERROR_INVALID_ARG;
    }

    return cmd->execute(argc, argv);
}

/* ============================================
   PARSING
   ============================================ */

cli_parse_result_t *cli_parse_input(const char *input)
{
    if (!input || strlen(input) == 0)
    {
        return NULL;
    }

    cli_parse_result_t *result = malloc(sizeof(cli_parse_result_t));
    memset(result, 0, sizeof(cli_parse_result_t));

    result->args = malloc(sizeof(char *) * MAX_ARG_COUNT);

    char *input_copy = strdup(input);
    char *token = strtok(input_copy, " \t\n");

    if (!token)
    {
        free(input_copy);
        free(result->args);
        free(result);
        return NULL;
    }

    strcpy(result->command, token);
    result->argc = 0;

    while ((token = strtok(NULL, " \t\n")) && result->argc < MAX_ARG_COUNT - 1)
    {
        result->args[result->argc] = strdup(token);
        result->argc++;
    }

    free(input_copy);
    return result;
}

void cli_free_parse_result(cli_parse_result_t *result)
{
    if (!result)
        return;
    for (int i = 0; i < result->argc; i++)
    {
        free(result->args[i]);
    }
    free(result->args);
    free(result);
}

/* ============================================
   HISTORY MANAGEMENT
   ============================================ */

void cli_add_history(const char *command)
{
    if (g_context.history_count >= MAX_HISTORY)
    {
        free(g_context.history[0]);
        for (int i = 0; i < MAX_HISTORY - 1; i++)
        {
            g_context.history[i] = g_context.history[i + 1];
        }
        g_context.history_count--;
    }

    g_context.history[g_context.history_count] = strdup(command);
    g_context.history_count++;
}

char *cli_get_history(int index)
{
    if (index < 0 || index >= g_context.history_count)
    {
        return NULL;
    }
    return g_context.history[index];
}

int cli_history_count(void)
{
    return g_context.history_count;
}

/* ============================================
   HELP SYSTEM
   ============================================ */

void cli_show_help(const char *command)
{
    int cmd_index = cli_command_exists(command);

    if (cmd_index < 0)
    {
        cli_print_error("Command not found: %s", command);
        return;
    }

    cli_command_t *cmd = &g_commands[cmd_index];
    printf("\n\033[1;34m%s\033[0m\n", cmd->name);
    printf("  %s\n\n", cmd->description);
    printf("  Usage: %s\n\n", cmd->usage);
}

void cli_show_all_commands(void)
{
    printf("\n\033[1;34mAvailable Commands:\033[0m\n\n");

    for (int i = 0; i < g_command_count; i++)
    {
        printf("  \033[0;32m%-20s\033[0m %s\n",
               g_commands[i].name,
               g_commands[i].description);
    }
    printf("\n");
}

/* ============================================
   UTILITY FUNCTIONS
   ============================================ */

char *cli_get_current_dir(void)
{
    return g_context.current_dir;
}

char *cli_get_current_user(void)
{
    return g_context.current_user;
}

/* ============================================
   CORE CLI FUNCTIONS
   ============================================ */

int cli_init(void)
{
    cli_init_context();
    g_command_count = 0;
    return CLI_SUCCESS;
}

int cli_parse_arguments(int argc, const char **argv)
{
    if (argc < 1 || !argv)
    {
        return CLI_ERROR_INVALID_ARG;
    }
    return CLI_SUCCESS;
}

int cli_execute_command(const char *command)
{
    if (!command)
    {
        return CLI_ERROR_INVALID_ARG;
    }

    cli_parse_result_t *result = cli_parse_input(command);
    if (!result)
    {
        return CLI_ERROR_INVALID_ARG;
    }

    char **argv = result->args;
    int argc = result->argc;

    cli_add_history(command);
    cli_status_t status = cli_dispatch_command(result->command, argc, argv);

    cli_free_parse_result(result);
    return status;
}

int cli_display_help(void)
{
    cli_show_all_commands();
    return CLI_SUCCESS;
}

int cli_display_version(void)
{
    printf("OneOS CLI v1.0\n");
    printf("Command-Line Interface System\n");
    printf("(c) 2026 OneOS Development\n");
    return CLI_SUCCESS;
}

int cli_process_input(const char *input_buffer)
{
    if (!input_buffer)
    {
        return CLI_ERROR_INVALID_ARG;
    }
    return cli_execute_command(input_buffer);
}
