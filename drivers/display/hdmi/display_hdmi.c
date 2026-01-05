#include "display_hdmi.h"

int display_hdmi_init(void)
{
    return 0;
}

int display_hdmi_probe(display_hdmi_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_hdmi_set_mode(display_hdmi_device_t *dev, display_hdmi_mode_t *mode)
{
    if (!dev || !mode)
    {
        return -1;
    }
    return 0;
}

int display_hdmi_detect_connected(display_hdmi_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_hdmi_enable_audio(display_hdmi_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_hdmi_send_infoframe(display_hdmi_device_t *dev, uint8_t *frame, uint32_t len)
{
    if (!dev || !frame)
    {
        return -1;
    }
    return 0;
}
