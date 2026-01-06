#include "libudev.h"

int libudev_init(void)
{
    return 0;
}

int libudev_new(udev **udev)
{
    if (!udev)
    {
        return -1;
    }
    return 0;
}

int libudev_unref(udev *udev)
{
    if (!udev)
    {
        return -1;
    }
    return 0;
}

int libudev_enumerate_new(udev *udev, udev_enumerate **enumerate)
{
    if (!udev || !enumerate)
    {
        return -1;
    }
    return 0;
}

int libudev_enumerate_add_match_subsystem(udev_enumerate *enumerate, const char *subsystem)
{
    if (!enumerate || !subsystem)
    {
        return -1;
    }
    return 0;
}

int libudev_enumerate_scan_devices(udev_enumerate *enumerate)
{
    if (!enumerate)
    {
        return -1;
    }
    return 0;
}
