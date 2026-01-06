#ifndef LIBUDEV_H
#define LIBUDEV_H

#include <stdint.h>

typedef void udev;
typedef void udev_enumerate;

int libudev_init(void);
int libudev_new(udev **udev);
int libudev_unref(udev *udev);
int libudev_enumerate_new(udev *udev, udev_enumerate **enumerate);
int libudev_enumerate_add_match_subsystem(udev_enumerate *enumerate, const char *subsystem);
int libudev_enumerate_scan_devices(udev_enumerate *enumerate);

#endif
