#ifndef REMOTE_DESKTOP_H
#define REMOTE_DESKTOP_H

#include <stdint.h>

typedef struct
{
    uint32_t session_id;
    const char *remote_host;
    uint32_t remote_port;
    const char *username;
    uint32_t is_connected;
    uint32_t connection_time;
} remote_session_t;

int remote_desktop_init(void);
int remote_desktop_connect(const char *host, uint32_t port, const char *username);
int remote_desktop_disconnect(uint32_t session_id);
int remote_desktop_send_mouse_event(uint32_t x, uint32_t y);
int remote_desktop_send_keyboard_event(uint32_t key_code);
int remote_desktop_list_sessions(remote_session_t *sessions, uint32_t *count);
int remote_desktop_take_screenshot(const char *output_path);
int remote_desktop_transfer_file(const char *source, const char *destination);

#endif
