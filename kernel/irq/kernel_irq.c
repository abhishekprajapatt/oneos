#include "kernel_irq.h"

int kernel_irq_init(void)
{
    return 0;
}

int kernel_irq_register_handler(kernel_irq_handler_t *handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int kernel_irq_unregister_handler(uint32_t irq_num, void *dev_id)
{
    return 0;
}

int kernel_irq_enable(uint32_t irq_num)
{
    return 0;
}

int kernel_irq_disable(uint32_t irq_num)
{
    return 0;
}

int kernel_irq_trigger(uint32_t irq_num)
{
    return 0;
}
