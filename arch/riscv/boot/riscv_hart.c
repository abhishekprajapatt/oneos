#include "riscv_hart.h"

int riscv_hart_init(void)
{
    return 0;
}

int riscv_hart_online(uint32_t hart_id)
{
    if (hart_id == 0)
    {
        return -1;
    }
    return 0;
}

int riscv_hart_offline(uint32_t hart_id)
{
    if (hart_id == 0)
    {
        return -1;
    }
    return 0;
}

int riscv_hart_get_info(uint32_t hart_id, riscv_hart_info_t *info)
{
    if (hart_id == 0 || !info)
    {
        return -1;
    }
    return 0;
}

int riscv_hart_set_frequency(uint32_t hart_id, uint64_t frequency)
{
    if (hart_id == 0 || frequency == 0)
    {
        return -1;
    }
    return 0;
}

int riscv_hart_send_ipi(uint32_t hart_id, uint32_t interrupt_type)
{
    if (hart_id == 0)
    {
        return -1;
    }
    return 0;
}

int riscv_hart_get_stats(riscv_hart_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
