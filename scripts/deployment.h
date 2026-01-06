#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include <stdint.h>

int deployment_init(void);
int deployment_package_system(const char *source_dir, const char *output_path);
int deployment_sign_package(const char *package_path, const char *key);
int deployment_distribute(const char *package_path, const char **targets, uint32_t target_count);
int deployment_verify_distribution(const char *package_path);
int deployment_rollback(const char *backup_path);

#endif
