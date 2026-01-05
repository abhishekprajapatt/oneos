#ifndef SOUND_PCI_H
#define SOUND_PCI_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t pci_device_id;
    uint32_t flags;
    uint32_t base_address;
} sound_pci_device_t;

typedef struct
{
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
    uint32_t format;
} sound_pci_format_t;

int sound_pci_init(void);
int sound_pci_probe(sound_pci_device_t *dev);
int sound_pci_set_format(sound_pci_device_t *dev, sound_pci_format_t *fmt);
int sound_pci_start_playback(sound_pci_device_t *dev);
int sound_pci_stop_playback(sound_pci_device_t *dev);
int sound_pci_transfer_data(sound_pci_device_t *dev, uint8_t *data, uint32_t len);

#endif
