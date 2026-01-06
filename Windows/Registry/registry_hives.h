#ifndef REGISTRY_HIVES_H
#define REGISTRY_HIVES_H

#include <stdint.h>

int registry_hives_init_hklm(void);
int registry_hives_init_hkcu(void);
int registry_hives_init_hkcr(void);
int registry_hives_init_hku(void);
int registry_hives_init_hkcc(void);
int registry_hives_backup_all(const char *backup_dir);
int registry_hives_restore_all(const char *backup_dir);

#endif
