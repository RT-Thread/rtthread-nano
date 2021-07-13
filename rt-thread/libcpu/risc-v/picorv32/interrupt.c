/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/01     Bernard      The first version
 * 2020-06-06     YuZhaorong   add PicoRV32    
 */

#include <rthw.h>
#define IRQN_MAX        32
#define MAX_HANDLERS    IRQN_MAX

static struct rt_irq_desc irq_desc[MAX_HANDLERS]={0};
static rt_base_t irq_mask = 0 ; 
static rt_isr_handler_t rt_hw_interrupt_handle(rt_uint32_t vector, void *param)
{
    rt_kprintf("UN-handled interrupt %d occurred!!!\n", vector);
    return RT_NULL;
}

/**
 * This function will initialize hardware interrupt
 */
void rt_hw_interrupt_init(void)
{
    int idx;    
    irq_mask =  rt_hw_interrupt_disable();
    irq_mask = 0xFFFFFFFF;
    for (idx = 0; idx < MAX_HANDLERS; idx++)
    {
        irq_desc[idx].handler = (rt_isr_handler_t)rt_hw_interrupt_handle;
        irq_desc[idx].param = RT_NULL;
#ifdef RT_USING_INTERRUPT_INFO
        rt_snprintf(irq_desc[idx].name, RT_NAME_MAX - 1, "default");
        irq_desc[idx].counter = 0;
#endif
    }

     rt_hw_interrupt_enable(irq_mask); 
       /* Enable machine external interrupts. */
}


/**
 * This function will mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_mask(int vector)
{
    rt_base_t irq_level;
    
    if(vector < MAX_HANDLERS)
    {
        irq_level =  rt_hw_interrupt_disable();
        irq_level |= 0x01<< vector; 
        rt_hw_interrupt_enable(irq_level); 
    }

}

/**
 * This function will un-mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_umask(int vector)
{
    rt_base_t irq_level;
    
    if(vector < MAX_HANDLERS)
    {
        irq_level =  rt_hw_interrupt_disable();
        irq_level &= ~( 0x01l << vector); 
        rt_hw_interrupt_enable( irq_level ); 
    }
}

/**
 * This function will install a interrupt service routine to a interrupt.
 * @param vector the interrupt number
 * @param new_handler the interrupt service routine to be installed
 * @param old_handler the old interrupt service routine
 */
rt_isr_handler_t rt_hw_interrupt_install(int vector, rt_isr_handler_t handler,
        void *param, const char *name)
{
    rt_isr_handler_t old_handler = RT_NULL;

    if(vector < MAX_HANDLERS)
    {
        old_handler = irq_desc[vector].handler;
        if (handler != RT_NULL)
        {
            irq_desc[vector].handler = (rt_isr_handler_t)handler;
            irq_desc[vector].param = param;
#ifdef RT_USING_INTERRUPT_INFO
            rt_snprintf(irq_desc[vector].name, RT_NAME_MAX - 1, "%s", name);
            irq_desc[vector].counter = 0;
#endif
        }
    }

    return old_handler;
}



unsigned int *irq(unsigned int *regs, unsigned int irqs)
{
    int int_num;
    for(int_num = 0; int_num < MAX_HANDLERS; int_num++)
    {
        if( (0x1 & (irqs>>int_num))== 1 )
        {
            if (irq_desc[int_num].handler)
            {
                irq_desc[int_num].handler(int_num, irq_desc[int_num].param);
            }
        }
            
    }
	return regs;
}



