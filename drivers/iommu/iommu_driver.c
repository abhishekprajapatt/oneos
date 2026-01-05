#include "iommu_driver.h"

int iommu_init(void)
{
    return 0;
}

int iommu_attach_device(iommu_device_t *dev, uint32_t domain_id)
{
    if (!dev)
    {
        return -1;
    }
    dev->domain_id = domain_id;
    return 0;
}

int iommu_map(iommu_device_t *dev, iommu_map_t *map)
{
    if (!dev || !map)
    {
        return -1;
    }
    return 0;
}

int iommu_unmap(iommu_device_t *dev, uint32_t iova, uint32_t size)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
