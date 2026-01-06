#ifndef ISO_BUILDER_H
#define ISO_BUILDER_H

#include <stdint.h>

int iso_builder_init(void);
int iso_builder_add_file(const char *file_path, const char *iso_path);
int iso_builder_add_directory(const char *dir_path, const char *iso_path);
int iso_builder_create(const char *iso_output_path);
int iso_builder_set_boot_image(const char *boot_image_path);
int iso_builder_verify_image(const char *iso_path);

#endif
