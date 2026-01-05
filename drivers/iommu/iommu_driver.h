#ifndef IOMMU_DRIVER_H
#define IOMMU_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t domain_id;
} iommu_device_t;

typedef struct
{
    uint32_t iova;
    uint32_t paddr;
    uint32_t size;
    uint32_t flags;
} iommu_map_t;

int iommu_init(void);
int iommu_attach_device(iommu_device_t *dev, uint32_t domain_id);
int iommu_map(iommu_device_t *dev, iommu_map_t *map);
int iommu_unmap(iommu_device_t *dev, uint32_t iova, uint32_t size);

#endif
