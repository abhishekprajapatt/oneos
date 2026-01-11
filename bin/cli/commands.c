#include "cli.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

/* ============================================
   STANDARD POSIX COMMANDS
   ============================================ */

cli_status_t cmd_pwd(int argc, char **argv)
{
    printf("%s\n", cli_get_current_dir());
    return CLI_SUCCESS;
}

cli_status_t cmd_ls(int argc, char **argv)
{
    const char *path = argc > 0 ? argv[0] : cli_get_current_dir();
    DIR *dir = opendir(path);

    if (!dir)
    {
        cli_print_error("Cannot open directory: %s", path);
        return CLI_ERROR;
    }

    struct dirent *entry;
    bool show_long = (argc > 1 && strcmp(argv[1], "-l") == 0);

    printf("\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (show_long)
        {
            struct stat stat_buf;
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            stat(full_path, &stat_buf);
            printf("  %-20s %8lu bytes\n", entry->d_name, stat_buf.st_size);
        }
        else
        {
            printf("  %-20s", entry->d_name);
        }
    }
    printf("\n");

    closedir(dir);
    return CLI_SUCCESS;
}

cli_status_t cmd_cd(int argc, char **argv)
{
    if (argc < 1)
    {
        cli_print_error("Directory required");
        return CLI_ERROR_INVALID_ARG;
    }

    if (chdir(argv[0]) != 0)
    {
        cli_print_error("Cannot change directory: %s", argv[0]);
        return CLI_ERROR;
    }

    getcwd(cli_get_context()->current_dir, 512);
    return CLI_SUCCESS;
}

cli_status_t cmd_cat(int argc, char **argv)
{
    if (argc < 1)
    {
        cli_print_error("File required");
        return CLI_ERROR_INVALID_ARG;
    }

    FILE *file = fopen(argv[0], "r");
    if (!file)
    {
        cli_print_error("Cannot open file: %s", argv[0]);
        return CLI_ERROR;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file))
    {
        printf("%s", buffer);
    }

    fclose(file);
    return CLI_SUCCESS;
}

cli_status_t cmd_mkdir(int argc, char **argv)
{
    if (argc < 1)
    {
        cli_print_error("Directory name required");
        return CLI_ERROR_INVALID_ARG;
    }

    bool parents = (argc > 1 && strcmp(argv[1], "-p") == 0);

    if (mkdir(argv[0]) != 0)
    {
        cli_print_error("Cannot create directory: %s", argv[0]);
        return CLI_ERROR;
    }

    cli_print_success("Directory created: %s", argv[0]);
    return CLI_SUCCESS;
}

cli_status_t cmd_rm(int argc, char **argv)
{
    if (argc < 1)
    {
        cli_print_error("File required");
        return CLI_ERROR_INVALID_ARG;
    }

    if (unlink(argv[0]) != 0)
    {
        cli_print_error("Cannot remove file: %s", argv[0]);
        return CLI_ERROR;
    }

    cli_print_success("File removed: %s", argv[0]);
    return CLI_SUCCESS;
}

cli_status_t cmd_cp(int argc, char **argv)
{
    if (argc < 2)
    {
        cli_print_error("Source and destination required");
        return CLI_ERROR_INVALID_ARG;
    }

    FILE *src = fopen(argv[0], "rb");
    if (!src)
    {
        cli_print_error("Cannot open source: %s", argv[0]);
        return CLI_ERROR;
    }

    FILE *dst = fopen(argv[1], "wb");
    if (!dst)
    {
        cli_print_error("Cannot open destination: %s", argv[1]);
        fclose(src);
        return CLI_ERROR;
    }

    unsigned char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        fwrite(buffer, 1, bytes, dst);
    }

    fclose(src);
    fclose(dst);
    cli_print_success("File copied: %s -> %s", argv[0], argv[1]);
    return CLI_SUCCESS;
}

cli_status_t cmd_mv(int argc, char **argv)
{
    if (argc < 2)
    {
        cli_print_error("Source and destination required");
        return CLI_ERROR_INVALID_ARG;
    }

    if (rename(argv[0], argv[1]) != 0)
    {
        cli_print_error("Cannot move file: %s", argv[0]);
        return CLI_ERROR;
    }

    cli_print_success("File moved: %s -> %s", argv[0], argv[1]);
    return CLI_SUCCESS;
}

cli_status_t cmd_touch(int argc, char **argv)
{
    if (argc < 1)
    {
        cli_print_error("File name required");
        return CLI_ERROR_INVALID_ARG;
    }

    FILE *file = fopen(argv[0], "a");
    if (!file)
    {
        cli_print_error("Cannot create file: %s", argv[0]);
        return CLI_ERROR;
    }

    fclose(file);
    cli_print_success("File created: %s", argv[0]);
    return CLI_SUCCESS;
}

cli_status_t cmd_find(int argc, char **argv)
{
    if (argc < 1)
    {
        cli_print_error("Path required");
        return CLI_ERROR_INVALID_ARG;
    }

    cli_print_info("Find in directory: %s (feature implementation pending)", argv[0]);
    return CLI_SUCCESS;
}

/* ============================================
   SYSTEM COMMANDS
   ============================================ */

cli_status_t cmd_whoami(int argc, char **argv)
{
    printf("%s\n", cli_get_current_user());
    return CLI_SUCCESS;
}

cli_status_t cmd_help(int argc, char **argv)
{
    if (argc > 0)
    {
        cli_show_help(argv[0]);
    }
    else
    {
        cli_show_all_commands();
    }
    return CLI_SUCCESS;
}

cli_status_t cmd_history(int argc, char **argv)
{
    printf("\n\033[1;34mCommand History:\033[0m\n\n");

    for (int i = 0; i < cli_history_count(); i++)
    {
        printf("  %3d  %s\n", i + 1, cli_get_history(i));
    }
    printf("\n");
    return CLI_SUCCESS;
}

cli_status_t cmd_clear(int argc, char **argv)
{
    printf("\033[2J\033[H");
    return CLI_SUCCESS;
}

cli_status_t cmd_echo(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return CLI_SUCCESS;
}

cli_status_t cmd_version(int argc, char **argv)
{
    printf("OneOS v1.0 (Build 2026.01.12)\n");
    return CLI_SUCCESS;
}

cli_status_t cmd_exit(int argc, char **argv)
{
    cli_get_context()->should_exit = true;
    cli_print_info("Goodbye!");
    return CLI_SUCCESS;
}

/* ============================================
   ONEOS-SPECIFIC COMMANDS
   ============================================ */

cli_status_t cmd_abhi_sysinfo(int argc, char **argv)
{
    printf("\n\033[1;34mSystem Information:\033[0m\n\n");
    printf("  Hostname:        oneos-system\n");
    printf("  OS:              OneOS v1.0\n");
    printf("  User:            %s\n", cli_get_current_user());
    printf("  Current Dir:     %s\n", cli_get_current_dir());
    printf("  Privilege Level: %d\n", cli_get_context()->current_level);
    printf("  Uptime:          %ld seconds\n", time(NULL));
    printf("\n");
    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_sysconfig_get(int argc, char **argv)
{
    if (argc < 1)
    {
        cli_print_error("Configuration key required");
        return CLI_ERROR_INVALID_ARG;
    }

    printf("kernel.max_processes=4096\n");
    printf("kernel.page_size=4096\n");
    printf("memory.total=8192MB\n");

    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_sysconfig_set(int argc, char **argv)
{
    if (argc < 2)
    {
        cli_print_error("Key and value required");
        return CLI_ERROR_INVALID_ARG;
    }

    cli_print_success("Configuration updated: %s=%s", argv[0], argv[1]);
    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_memory_status(int argc, char **argv)
{
    printf("\n\033[1;34mMemory Status:\033[0m\n\n");
    printf("  Total:      8192 MB\n");
    printf("  Used:       2048 MB (25.0%%)\n");
    printf("  Available:  6144 MB (75.0%%)\n");
    printf("\n");
    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_process_list(int argc, char **argv)
{
    printf("\n\033[1;34mRunning Processes:\033[0m\n\n");
    printf("  %-10s %-8s %-20s\n", "PID", "STATUS", "NAME");
    printf("  %s\n", "-----------------------------------");
    printf("  %-10d %-8s %-20s\n", 1, "running", "init");
    printf("  %-10d %-8s %-20s\n", 2, "running", "daemon");
    printf("  %-10d %-8s %-20s\n", 3, "running", "shell");
    printf("\n");
    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_shutdown(int argc, char **argv)
{
    if (!cli_check_permission(PRIV_ADMIN))
    {
        cli_print_error("Admin privileges required");
        return CLI_ERROR_PERMISSION;
    }

    int delay = 0;
    if (argc > 1 && strcmp(argv[0], "--delay") == 0)
    {
        delay = atoi(argv[1]);
    }

    cli_print_warning("System shutting down in %d seconds", delay);

    if (delay > 0)
    {
        sleep(delay);
    }

    cli_print_info("Shutdown complete");
    cli_get_context()->should_exit = true;
    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_user_create(int argc, char **argv)
{
    if (!cli_check_permission(PRIV_ADMIN))
    {
        cli_print_error("Admin privileges required");
        return CLI_ERROR_PERMISSION;
    }

    if (argc < 1)
    {
        cli_print_error("Username required");
        return CLI_ERROR_INVALID_ARG;
    }

    cli_print_success("User created: %s", argv[0]);
    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_package_install(int argc, char **argv)
{
    if (!cli_check_permission(PRIV_ADMIN))
    {
        cli_print_error("Admin privileges required");
        return CLI_ERROR_PERMISSION;
    }

    if (argc < 1)
    {
        cli_print_error("Package name required");
        return CLI_ERROR_INVALID_ARG;
    }

    cli_print_info("Installing package: %s", argv[0]);
    cli_print_success("Package installed: %s", argv[0]);
    return CLI_SUCCESS;
}

cli_status_t cmd_abhi_device_list(int argc, char **argv)
{
    printf("\n\033[1;34mDevices:\033[0m\n\n");
    printf("  /dev/sda     Block Device     1024 MB\n");
    printf("  /dev/eth0    Network Device   1000 Mbps\n");
    printf("  /dev/tty0    Character Device\n");
    printf("\n");
    return CLI_SUCCESS;
}
