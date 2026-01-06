#ifndef LIBGCCRT_H
#define LIBGCCRT_H

#include <stdint.h>

int libgccrt_init(void);
int libgccrt_register_exception_handler(void *handler);
int libgccrt_unwind_frame(void *context);
int libgccrt_get_personality_routine(void **routine);
int libgccrt_dwarf_cfi_info(void *fde);

#endif
