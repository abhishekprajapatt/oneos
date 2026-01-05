#ifndef IOCTL_H
#define IOCTL_H

#include <stdint.h>

#define _IOC_NRBITS 8
#define _IOC_TYPEBITS 8
#define _IOC_SIZEBITS 14

#define _IOC_NRSHIFT 0
#define _IOC_TYPESHIFT (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT (_IOC_TYPESHIFT + _IOC_TYPEBITS)

#define _IOC(dir, type, nr, size)                  \
    (((dir) << (_IOC_SIZESHIFT + _IOC_SIZEBITS)) | \
     ((size) << _IOC_SIZESHIFT) |                  \
     ((type) << _IOC_TYPESHIFT) |                  \
     ((nr) << _IOC_NRSHIFT))

#define _IO(type, nr) _IOC(0, type, nr, 0)
#define _IOR(type, nr, size) _IOC(1, type, nr, sizeof(size))
#define _IOW(type, nr, size) _IOC(2, type, nr, sizeof(size))
#define _IOWR(type, nr, size) _IOC(3, type, nr, sizeof(size))

#endif
