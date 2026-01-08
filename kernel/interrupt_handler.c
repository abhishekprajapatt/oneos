#include "interrupt_handler.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

InterruptController *irq_controller_init(void)
{
    InterruptController *ic = (InterruptController *)malloc(sizeof(InterruptController));
    if (!ic)
        return NULL;

    ic->interrupt_count = 0;
    ic->total_handled = 0;
    ic->last_interrupt_time = 0;

    memset(ic->descriptors, 0, sizeof(ic->descriptors));

    for (uint32_t i = 0; i < MAX_INTERRUPTS; i++)
    {
        ic->descriptors[i].irq_number = i;
        ic->descriptors[i].handlers = (void (*)(uint32_t))malloc(MAX_IRQ_HANDLERS * sizeof(void *));
        if (!ic->descriptors[i].handlers)
        {
            for (uint32_t j = 0; j < i; j++)
            {
                free(ic->descriptors[j].handlers);
            }
            free(ic);
            return NULL;
        }
        ic->descriptors[i].handler_count = 0;
        ic->descriptors[i].enabled = false;
        ic->descriptors[i].priority = 0;
    }

    return ic;
}

void irq_controller_cleanup(InterruptController *ic)
{
    if (!ic)
        return;

    for (uint32_t i = 0; i < MAX_INTERRUPTS; i++)
    {
        if (ic->descriptors[i].handlers)
        {
            free(ic->descriptors[i].handlers);
        }
    }

    free(ic);
}

int irq_register_handler(InterruptController *ic, uint32_t irq_number, IRQType type,
                         void (*handler)(uint32_t), uint32_t priority)
{
    if (!ic || irq_number >= MAX_INTERRUPTS || !handler)
        return -1;

    InterruptDescriptor *desc = &ic->descriptors[irq_number];
    if (desc->handler_count >= MAX_IRQ_HANDLERS)
        return -1;

    desc->handlers[desc->handler_count] = handler;
    desc->handler_count++;
    desc->type = type;
    desc->priority = priority;
    ic->interrupt_count++;

    return 0;
}

int irq_unregister_handler(InterruptController *ic, uint32_t irq_number)
{
    if (!ic || irq_number >= MAX_INTERRUPTS)
        return -1;

    InterruptDescriptor *desc = &ic->descriptors[irq_number];
    if (desc->handler_count > 0)
    {
        desc->handler_count--;
        ic->interrupt_count--;
        return 0;
    }

    return -1;
}

int irq_enable(InterruptController *ic, uint32_t irq_number)
{
    if (!ic || irq_number >= MAX_INTERRUPTS)
        return -1;

    ic->descriptors[irq_number].enabled = true;
    return 0;
}

int irq_disable(InterruptController *ic, uint32_t irq_number)
{
    if (!ic || irq_number >= MAX_INTERRUPTS)
        return -1;

    ic->descriptors[irq_number].enabled = false;
    return 0;
}

void irq_handle(InterruptController *ic, uint32_t irq_number)
{
    if (!ic || irq_number >= MAX_INTERRUPTS)
        return;

    InterruptDescriptor *desc = &ic->descriptors[irq_number];
    if (!desc->enabled)
        return;

    for (uint32_t i = 0; i < desc->handler_count; i++)
    {
        if (desc->handlers[i])
        {
            desc->handlers[i](irq_number);
        }
    }

    ic->total_handled++;
    ic->last_interrupt_time = time(NULL);
}

void irq_acknowledge(InterruptController *ic, uint32_t irq_number)
{
    if (!ic || irq_number >= MAX_INTERRUPTS)
        return;
}

uint32_t irq_get_handler_count(InterruptController *ic, uint32_t irq_number)
{
    if (!ic || irq_number >= MAX_INTERRUPTS)
        return 0;
    return ic->descriptors[irq_number].handler_count;
}

uint32_t irq_get_total_interrupts_handled(InterruptController *ic)
{
    if (!ic)
        return 0;
    return ic->total_handled;
}
