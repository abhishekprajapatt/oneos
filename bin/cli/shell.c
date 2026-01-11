#include "cli.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define PROMPT_SIZE 256
#define INPUT_BUFFER_SIZE 4096

/* Command declarations (from commands.c) */
extern cli_status_t cmd_pwd(int argc, char **argv);
extern cli_status_t cmd_ls(int argc, char **argv);
extern cli_status_t cmd_cd(int argc, char **argv);
extern cli_status_t cmd_cat(int argc, char **argv);
extern cli_status_t cmd_mkdir(int argc, char **argv);
extern cli_status_t cmd_rm(int argc, char **argv);
extern cli_status_t cmd_cp(int argc, char **argv);
extern cli_status_t cmd_mv(int argc, char **argv);
extern cli_status_t cmd_touch(int argc, char **argv);
extern cli_status_t cmd_find(int argc, char **argv);
extern cli_status_t cmd_whoami(int argc, char **argv);
extern cli_status_t cmd_help(int argc, char **argv);
extern cli_status_t cmd_history(int argc, char **argv);
extern cli_status_t cmd_clear(int argc, char **argv);
extern cli_status_t cmd_echo(int argc, char **argv);
extern cli_status_t cmd_version(int argc, char **argv);
extern cli_status_t cmd_exit(int argc, char **argv);
extern cli_status_t cmd_abhi_sysinfo(int argc, char **argv);
extern cli_status_t cmd_abhi_sysconfig_get(int argc, char **argv);
extern cli_status_t cmd_abhi_sysconfig_set(int argc, char **argv);
extern cli_status_t cmd_abhi_memory_status(int argc, char **argv);
extern cli_status_t cmd_abhi_process_list(int argc, char **argv);
extern cli_status_t cmd_abhi_shutdown(int argc, char **argv);
extern cli_status_t cmd_abhi_user_create(int argc, char **argv);
extern cli_status_t cmd_abhi_package_install(int argc, char **argv);
extern cli_status_t cmd_abhi_device_list(int argc, char **argv);

/* Extended command declarations (from extended_commands.c) */
extern cli_status_t cmd_abhi_interface_list(int argc, char **argv);
extern cli_status_t cmd_abhi_route_list(int argc, char **argv);
extern cli_status_t cmd_ping(int argc, char **argv);
extern cli_status_t cmd_ps(int argc, char **argv);
extern cli_status_t cmd_top(int argc, char **argv);
extern cli_status_t cmd_abhi_cpu_usage(int argc, char **argv);
extern cli_status_t cmd_abhi_disk_usage(int argc, char **argv);
extern cli_status_t cmd_abhi_time_get(int argc, char **argv);
extern cli_status_t cmd_abhi_time_set(int argc, char **argv);
extern cli_status_t cmd_abhi_cron_list(int argc, char **argv);
extern cli_status_t cmd_abhi_cron_add(int argc, char **argv);
extern cli_status_t cmd_abhi_mount(int argc, char **argv);
extern cli_status_t cmd_abhi_unmount(int argc, char **argv);
extern cli_status_t cmd_abhi_volume_list(int argc, char **argv);
extern cli_status_t cmd_abhi_user_list(int argc, char **argv);
extern cli_status_t cmd_abhi_permission_grant(int argc, char **argv);
extern cli_status_t cmd_abhi_audit_log(int argc, char **argv);
extern cli_status_t cmd_abhi_log_view(int argc, char **argv);
extern cli_status_t cmd_abhi_config_export(int argc, char **argv);
extern cli_status_t cmd_abhi_config_import(int argc, char **argv);
extern cli_status_t cmd_abhi_status_report(int argc, char **argv);
extern cli_status_t cmd_abhi_health_check(int argc, char **argv);
extern cli_status_t cmd_abhi_reboot(int argc, char **argv);
extern cli_status_t cmd_abhi_sleep(int argc, char **argv);
extern cli_status_t cmd_abhi_power_status(int argc, char **argv);

static void register_all_commands(void)
{
    cli_command_t commands[] = {
        {.name = "pwd",
         .description = "Print working directory",
         .execute = cmd_pwd,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "pwd"},
        {.name = "ls",
         .description = "List directory contents",
         .execute = cmd_ls,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 2,
         .usage = "ls [path] [-l]"},
        {.name = "cd",
         .description = "Change directory",
         .execute = cmd_cd,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 1,
         .usage = "cd <directory>"},
        {.name = "cat",
         .description = "Display file contents",
         .execute = cmd_cat,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 1,
         .usage = "cat <file>"},
        {.name = "mkdir",
         .description = "Create directory",
         .execute = cmd_mkdir,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 2,
         .usage = "mkdir <dirname> [-p]"},
        {.name = "rm",
         .description = "Remove file",
         .execute = cmd_rm,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 1,
         .usage = "rm <file>"},
        {.name = "cp",
         .description = "Copy file",
         .execute = cmd_cp,
         .required_level = PRIV_USER,
         .min_args = 2,
         .max_args = 2,
         .usage = "cp <source> <destination>"},
        {.name = "mv",
         .description = "Move/rename file",
         .execute = cmd_mv,
         .required_level = PRIV_USER,
         .min_args = 2,
         .max_args = 2,
         .usage = "mv <source> <destination>"},
        {.name = "touch",
         .description = "Create empty file",
         .execute = cmd_touch,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 1,
         .usage = "touch <filename>"},
        {.name = "find",
         .description = "Find files",
         .execute = cmd_find,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 3,
         .usage = "find <path> [-name pattern]"},
        {.name = "whoami",
         .description = "Show current user",
         .execute = cmd_whoami,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "whoami"},
        {.name = "help",
         .description = "Show help information",
         .execute = cmd_help,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 1,
         .usage = "help [command]"},
        {.name = "history",
         .description = "Show command history",
         .execute = cmd_history,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "history"},
        {.name = "clear",
         .description = "Clear screen",
         .execute = cmd_clear,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "clear"},
        {.name = "echo",
         .description = "Print text",
         .execute = cmd_echo,
         .required_level = PRIV_PUBLIC,
         .min_args = 1,
         .max_args = 32,
         .usage = "echo <text>"},
        {.name = "version",
         .description = "Show system version",
         .execute = cmd_version,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "version"},
        {.name = "exit",
         .description = "Exit shell",
         .execute = cmd_exit,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "exit"},
        {.name = "abhi-sysinfo",
         .description = "Show system information",
         .execute = cmd_abhi_sysinfo,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-sysinfo [--json]"},
        {.name = "abhi-sysconfig-get",
         .description = "Get system configuration",
         .execute = cmd_abhi_sysconfig_get,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 1,
         .usage = "abhi-sysconfig-get <key>"},
        {.name = "abhi-sysconfig-set",
         .description = "Set system configuration",
         .execute = cmd_abhi_sysconfig_set,
         .required_level = PRIV_ADMIN,
         .min_args = 2,
         .max_args = 2,
         .usage = "abhi-sysconfig-set <key> <value>"},
        {.name = "abhi-memory-status",
         .description = "Show memory usage",
         .execute = cmd_abhi_memory_status,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-memory-status [--verbose]"},
        {.name = "abhi-process-list",
         .description = "List running processes",
         .execute = cmd_abhi_process_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-process-list [--detailed]"},
        {.name = "abhi-shutdown",
         .description = "Shutdown system",
         .execute = cmd_abhi_shutdown,
         .required_level = PRIV_ADMIN,
         .min_args = 0,
         .max_args = 2,
         .usage = "abhi-shutdown [--delay seconds]"},
        {.name = "abhi-user-create",
         .description = "Create user account",
         .execute = cmd_abhi_user_create,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 4,
         .usage = "abhi-user-create <username> [--uid=] [--group=]"},
        {.name = "abhi-package-install",
         .description = "Install package",
         .execute = cmd_abhi_package_install,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 3,
         .usage = "abhi-package-install <package> [--version=]"},
        {.name = "abhi-device-list",
         .description = "List system devices",
         .execute = cmd_abhi_device_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-device-list [--type=all|block|char|net]"},
        {.name = "ps",
         .description = "List processes",
         .execute = cmd_ps,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "ps [--verbose]"},
        {.name = "top",
         .description = "Show process usage",
         .execute = cmd_top,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "top"},
        {.name = "cpu-usage",
         .description = "Show CPU usage",
         .execute = cmd_abhi_cpu_usage,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "cpu-usage"},
        {.name = "abhi-cpu-usage",
         .description = "Show CPU usage",
         .execute = cmd_abhi_cpu_usage,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-cpu-usage"},
        {.name = "disk-usage",
         .description = "Show disk usage",
         .execute = cmd_abhi_disk_usage,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "disk-usage [path]"},
        {.name = "abhi-disk-usage",
         .description = "Show disk usage",
         .execute = cmd_abhi_disk_usage,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-disk-usage [path]"},
        {.name = "time-get",
         .description = "Get system time",
         .execute = cmd_abhi_time_get,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "time-get"},
        {.name = "abhi-time-get",
         .description = "Get system time",
         .execute = cmd_abhi_time_get,
         .required_level = PRIV_PUBLIC,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-time-get"},
        {.name = "time-set",
         .description = "Set system time",
         .execute = cmd_abhi_time_set,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 1,
         .usage = "time-set <timestamp>"},
        {.name = "abhi-time-set",
         .description = "Set system time",
         .execute = cmd_abhi_time_set,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 1,
         .usage = "abhi-time-set <timestamp>"},
        {.name = "ping",
         .description = "Ping host",
         .execute = cmd_ping,
         .required_level = PRIV_USER,
         .min_args = 1,
         .max_args = 2,
         .usage = "ping <host> [-c count]"},
        {.name = "interfaces",
         .description = "List network interfaces",
         .execute = cmd_abhi_interface_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "interfaces"},
        {.name = "abhi-interface-list",
         .description = "List network interfaces",
         .execute = cmd_abhi_interface_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-interface-list"},
        {.name = "routes",
         .description = "List routing table",
         .execute = cmd_abhi_route_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "routes"},
        {.name = "abhi-route-list",
         .description = "List routing table",
         .execute = cmd_abhi_route_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-route-list"},
        {.name = "mount",
         .description = "Mount filesystem",
         .execute = cmd_abhi_mount,
         .required_level = PRIV_ADMIN,
         .min_args = 2,
         .max_args = 3,
         .usage = "mount <device> <path> [--type=]"},
        {.name = "abhi-mount",
         .description = "Mount filesystem",
         .execute = cmd_abhi_mount,
         .required_level = PRIV_ADMIN,
         .min_args = 2,
         .max_args = 3,
         .usage = "abhi-mount <device> <path> [--type=]"},
        {.name = "unmount",
         .description = "Unmount filesystem",
         .execute = cmd_abhi_unmount,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 1,
         .usage = "unmount <path>"},
        {.name = "abhi-unmount",
         .description = "Unmount filesystem",
         .execute = cmd_abhi_unmount,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 1,
         .usage = "abhi-unmount <path>"},
        {.name = "volumes",
         .description = "List volumes",
         .execute = cmd_abhi_volume_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "volumes"},
        {.name = "abhi-volume-list",
         .description = "List volumes",
         .execute = cmd_abhi_volume_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-volume-list"},
        {.name = "users",
         .description = "List users",
         .execute = cmd_abhi_user_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "users"},
        {.name = "abhi-user-list",
         .description = "List users",
         .execute = cmd_abhi_user_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-user-list"},
        {.name = "chmod",
         .description = "Grant permissions",
         .execute = cmd_abhi_permission_grant,
         .required_level = PRIV_ADMIN,
         .min_args = 2,
         .max_args = 2,
         .usage = "chmod <user> <permission>"},
        {.name = "abhi-permission-grant",
         .description = "Grant permissions",
         .execute = cmd_abhi_permission_grant,
         .required_level = PRIV_ADMIN,
         .min_args = 2,
         .max_args = 2,
         .usage = "abhi-permission-grant <user> <permission>"},
        {.name = "audit",
         .description = "View audit log",
         .execute = cmd_abhi_audit_log,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "audit"},
        {.name = "abhi-audit-log",
         .description = "View audit log",
         .execute = cmd_abhi_audit_log,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-audit-log"},
        {.name = "logs",
         .description = "View system logs",
         .execute = cmd_abhi_log_view,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "logs [--lines=N]"},
        {.name = "abhi-log-view",
         .description = "View system logs",
         .execute = cmd_abhi_log_view,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-log-view [--lines=N]"},
        {.name = "cron-list",
         .description = "List cron jobs",
         .execute = cmd_abhi_cron_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "cron-list"},
        {.name = "abhi-cron-list",
         .description = "List cron jobs",
         .execute = cmd_abhi_cron_list,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-cron-list"},
        {.name = "cron-add",
         .description = "Add cron job",
         .execute = cmd_abhi_cron_add,
         .required_level = PRIV_USER,
         .min_args = 2,
         .max_args = 2,
         .usage = "cron-add <schedule> <command>"},
        {.name = "abhi-cron-add",
         .description = "Add cron job",
         .execute = cmd_abhi_cron_add,
         .required_level = PRIV_USER,
         .min_args = 2,
         .max_args = 2,
         .usage = "abhi-cron-add <schedule> <command>"},
        {.name = "export",
         .description = "Export configuration",
         .execute = cmd_abhi_config_export,
         .required_level = PRIV_ADMIN,
         .min_args = 0,
         .max_args = 1,
         .usage = "export [--file=]"},
        {.name = "abhi-config-export",
         .description = "Export configuration",
         .execute = cmd_abhi_config_export,
         .required_level = PRIV_ADMIN,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-config-export [--file=]"},
        {.name = "import",
         .description = "Import configuration",
         .execute = cmd_abhi_config_import,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 1,
         .usage = "import <file>"},
        {.name = "abhi-config-import",
         .description = "Import configuration",
         .execute = cmd_abhi_config_import,
         .required_level = PRIV_ADMIN,
         .min_args = 1,
         .max_args = 1,
         .usage = "abhi-config-import <file>"},
        {.name = "status",
         .description = "System status report",
         .execute = cmd_abhi_status_report,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "status [--json]"},
        {.name = "abhi-status-report",
         .description = "System status report",
         .execute = cmd_abhi_status_report,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-status-report [--json]"},
        {.name = "health",
         .description = "Health check",
         .execute = cmd_abhi_health_check,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "health [--verbose]"},
        {.name = "abhi-health-check",
         .description = "Health check",
         .execute = cmd_abhi_health_check,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-health-check [--verbose]"},
        {.name = "reboot",
         .description = "Reboot system",
         .execute = cmd_abhi_reboot,
         .required_level = PRIV_ADMIN,
         .min_args = 0,
         .max_args = 1,
         .usage = "reboot [--delay=seconds]"},
        {.name = "abhi-reboot",
         .description = "Reboot system",
         .execute = cmd_abhi_reboot,
         .required_level = PRIV_ADMIN,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-reboot [--delay=seconds]"},
        {.name = "sleep",
         .description = "Sleep mode",
         .execute = cmd_abhi_sleep,
         .required_level = PRIV_ADMIN,
         .min_args = 0,
         .max_args = 1,
         .usage = "sleep [--delay=seconds]"},
        {.name = "abhi-sleep",
         .description = "Sleep mode",
         .execute = cmd_abhi_sleep,
         .required_level = PRIV_ADMIN,
         .min_args = 0,
         .max_args = 1,
         .usage = "abhi-sleep [--delay=seconds]"},
        {.name = "power",
         .description = "Show power status",
         .execute = cmd_abhi_power_status,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "power"},
        {.name = "abhi-power-status",
         .description = "Show power status",
         .execute = cmd_abhi_power_status,
         .required_level = PRIV_USER,
         .min_args = 0,
         .max_args = 0,
         .usage = "abhi-power-status"},
    };

    int cmd_count = sizeof(commands) / sizeof(cli_command_t);
    for (int i = 0; i < cmd_count; i++)
    {
        cli_register_command(commands[i]);
    }
}

static void print_welcome(void)
{
    printf("\n");
    printf("  \033[1;34m╔════════════════════════════════════════════════╗\033[0m\n");
    printf("  \033[1;34m║\033[0m     \033[1;32mOneOS Command-Line Interface v1.0\033[0m         \033[1;34m║\033[0m\n");
    printf("  \033[1;34m║\033[0m     Production-Grade CLI System                \033[1;34m║\033[0m\n");
    printf("  \033[1;34m╚════════════════════════════════════════════════╝\033[0m\n");
    printf("\n");
    printf("  Type 'help' for available commands\n");
    printf("  Type 'version' for version info\n");
    printf("  Type 'exit' to quit\n\n");
}

static void print_prompt(char *prompt, size_t size)
{
    cli_context_t *ctx = cli_get_context();

    char priv_char = (ctx->current_level >= PRIV_ADMIN) ? '#' : '$';
    snprintf(prompt, size, "\033[0;32m[%s@oneos]\033[0m \033[0;34m%s\033[0m %c ",
             ctx->current_user,
             ctx->current_dir,
             priv_char);
}

int main(int argc, char **argv)
{
    cli_init();
    register_all_commands();
    print_welcome();

    char input_buffer[INPUT_BUFFER_SIZE];
    char prompt[PROMPT_SIZE];

    cli_context_t *ctx = cli_get_context();

    while (!ctx->should_exit)
    {
        print_prompt(prompt, sizeof(prompt));
        fputs(prompt, stdout);
        fflush(stdout);

        if (!fgets(input_buffer, sizeof(input_buffer), stdin))
        {
            break;
        }

        input_buffer[strcspn(input_buffer, "\n")] = 0;

        if (strlen(input_buffer) == 0)
        {
            continue;
        }

        cli_execute_command(input_buffer);
    }

    cli_cleanup_context();
    return 0;
}
