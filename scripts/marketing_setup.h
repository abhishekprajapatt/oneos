#ifndef MARKETING_SETUP_H
#define MARKETING_SETUP_H

#include <stdint.h>

int marketing_setup_init(void);
int marketing_setup_create_demo(const char *demo_name);
int marketing_setup_generate_screenshots(void);
int marketing_setup_create_presentation(const char *output_path);
int marketing_setup_feature_showcase(const char **features, uint32_t count);
int marketing_setup_user_personas(void);

#endif
