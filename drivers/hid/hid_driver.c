#include "hid_driver.h"

int hid_init(void)
{
    return 0;
}

int hid_register_device(hid_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int hid_send_report(hid_device_t *dev, hid_report_t *report)
{
    if (!dev || !report || !report->data)
    {
        return -1;
    }
    return 0;
}

int hid_receive_report(hid_device_t *dev, hid_report_t *report)
{
    if (!dev || !report || !report->data)
    {
        return -1;
    }
    return 0;
}
