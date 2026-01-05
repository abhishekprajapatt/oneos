#ifndef SOUND_USB_H
#define SOUND_USB_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t product_id;
    uint32_t flags;
    uint8_t usb_address;
} sound_usb_device_t;

typedef struct
{
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
    uint16_t packet_size;
} sound_usb_format_t;

int sound_usb_init(void);
int sound_usb_probe(sound_usb_device_t *dev);
int sound_usb_set_format(sound_usb_device_t *dev, sound_usb_format_t *fmt);
int sound_usb_start_stream(sound_usb_device_t *dev);
int sound_usb_stop_stream(sound_usb_device_t *dev);
int sound_usb_submit_urb(sound_usb_device_t *dev, uint8_t *data, uint32_t len);

#endif
