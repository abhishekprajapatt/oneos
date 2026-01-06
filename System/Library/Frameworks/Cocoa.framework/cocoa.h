#ifndef COCOA_H
#define COCOA_H

#include <stdint.h>

int cocoa_init(void);
int cocoa_run_application(void);
int cocoa_process_events(void);
int cocoa_get_main_thread(uint64_t *thread_id);
int cocoa_dispatch_on_main(void (*callback)(void));
int cocoa_shutdown(void);

#endif
