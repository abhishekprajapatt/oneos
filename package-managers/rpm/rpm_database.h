#ifndef RPM_DATABASE_H
#define RPM_DATABASE_H

#include <stdint.h>

int rpm_database_init(void);
int rpm_database_open(const char *db_path);
int rpm_database_query(const char *package_name);
int rpm_database_rebuild(void);
int rpm_database_verify(void);
int rpm_database_export(const char *output_file);
int rpm_database_close(void);

#endif
