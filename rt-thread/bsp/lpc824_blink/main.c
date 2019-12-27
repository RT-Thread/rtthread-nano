/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-22     Tanek        the first version
 */
#include <stdio.h>
#include <rthw.h>
#include <rtthread.h>

#include "board_lpc.h"

int main(void)
{
    /* Initialize GPIO */
    Chip_GPIO_Init(LPC_GPIO_PORT);
    Chip_GPIO_PinSetDIR(LPC_GPIO_PORT, 0, 7, 1);
    Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 7, true);
    
    while (1)
    {
        Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 7, true);
        rt_thread_delay(RT_TICK_PER_SECOND / 2);
        
        Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 7, false);
        rt_thread_delay(RT_TICK_PER_SECOND / 2);
    }
}

