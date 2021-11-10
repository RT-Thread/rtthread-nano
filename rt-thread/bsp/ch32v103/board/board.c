/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-10-26     iysheng           first version
 */

#include <rtthread.h>
#include <rthw.h>
#include "board.h"
#include "ch32_pinmux.h"
#include "ch32v10x_rcc.h"
#include "drv_uart.h"
#include "riscv-ops.h"

rt_uint32_t ch32_get_sysclock_frequency(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    return RCC_Clocks.SYSCLK_Frequency;
}

static int systick_init(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFFFF)
    {
        return 1;
    }
    ticks -= 1;

    SysTick->CNTL0 = 0;
    SysTick->CNTL1 = 0;
    SysTick->CNTL2 = 0;
    SysTick->CNTL3 = 0;

    SysTick->CNTH0 = 0;
    SysTick->CNTH1 = 0;
    SysTick->CNTH2 = 0;
    SysTick->CNTH3 = 0;

    SysTick->CMPLR0 = (ticks >>  0) & 0xff;
    SysTick->CMPLR1 = (ticks >>  8) & 0xff;
    SysTick->CMPLR2 = (ticks >> 16) & 0xff;
    SysTick->CMPLR3 = (ticks >> 24) & 0xff;

    SysTick->CMPHR0 = 0;
    SysTick->CMPHR1 = 0;
    SysTick->CMPHR2 = 0;
    SysTick->CMPHR3 = 0;

    PFIC->CFGR = 0xFA050003;
    NVIC_SetPriority(SysTicK_IRQn, 0xf0);
    NVIC_SetPriority(Software_IRQn, 0xf0);
    NVIC_EnableIRQ(SysTicK_IRQn);
    NVIC_EnableIRQ(Software_IRQn);
    SysTick->CTLR = 1;

    return 0;
}

char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

    return ch;
}

void rt_hw_board_init(void)
{
    rt_uint32_t sysfreq;
    extern int _susrstack, _eusrstack;

    sysfreq = ch32_get_sysclock_frequency();
    systick_init(sysfreq / 8 / RT_TICK_PER_SECOND);
    write_csr(mscratch, &_eusrstack);

    /* Heap initialization */
#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)&_susrstack);
#endif

    ch32v103_pinmux_init();
    /* USART driver initialization is open by default */
#ifdef RT_USING_SERIAL
extern int rt_hw_uart_init(void);
    rt_hw_uart_init();
#endif

    /* Set the shell console output device */
#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}
