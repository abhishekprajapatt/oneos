#include "sound_pci.h"

int sound_pci_init(void)
{
    return 0;
}

int sound_pci_probe(sound_pci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_pci_set_format(sound_pci_device_t *dev, sound_pci_format_t *fmt)
{
    if (!dev || !fmt)
    {
        return -1;
    }
    return 0;
}

int sound_pci_start_playback(sound_pci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_pci_stop_playback(sound_pci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_pci_transfer_data(sound_pci_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}
