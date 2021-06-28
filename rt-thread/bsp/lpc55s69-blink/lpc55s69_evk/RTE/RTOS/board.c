/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>

#include "fsl_usart.h"
#include "board.h"
#include "clock_config.h"
#include "veneer_table.h"

uint32_t __SysTick_Config(uint32_t ticks)
{
    if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
    {
        return (1UL);                                                   /* Reload value impossible */
    }

    SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
    NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);  /* set Priority for Systick Interrupt */
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
    return (0UL);                                                     /* Function successful */
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}


/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
    /* Hardware Initialization */
    BOARD_InitPins();

    CLOCK_EnableClock(kCLOCK_InputMux);

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);

    GPIO_PortInit(GPIO, 0);
    GPIO_PortInit(GPIO, 1);

    /* NVIC Configuration */
#define NVIC_VTOR_MASK              0x3FFFFF80
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x10000000 */
    SCB->VTOR  = (0x10000000 & NVIC_VTOR_MASK);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x00000000 */
    SCB->VTOR  = (0x00020000 & NVIC_VTOR_MASK);
#endif

    /* init systick */
    __SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    /* set pend exception priority */
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization board with RT-Thread Components */
    rt_components_board_init();
#endif

#ifdef RT_USING_HEAP
    rt_kprintf("sram heap, begin: 0x%p, end: 0x%p\n", HEAP_BEGIN, HEAP_END);
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif
}

/* TODO: OS Tick increase */
void MemManage_Handler(void)
{
    extern void HardFault_Handler(void);

    rt_kprintf("Memory Fault!\n");
    HardFault_Handler();
}

/* TODO: uart init and console output */
#define UART_CLK_SRC  kCLOCK_Flexcomm0
#define UART_CLK_FREQ kFRO12M_to_FLEXCOMM0
#define UART_BASE     USART0
#define UART_IRQN     FLEXCOMM0_IRQn

static int uart_init(void)
{
    usart_config_t config;

    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(UART_CLK_FREQ);

    USART_GetDefaultConfig(&config);
    /* config uart */
    config.baudRate_Bps = 115200;
    config.bitCountPerChar = kUSART_8BitsPerChar;
    config.stopBitCount = kUSART_OneStopBit;
    config.parityMode = kUSART_ParityDisabled;
    config.enableTx = true;
    config.enableRx = true;

    USART_Init(UART_BASE, &config, CLOCK_GetFreq(UART_CLK_SRC));

    return 0;
}
INIT_BOARD_EXPORT(uart_init);

/* For console print */
void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            while (!(kUSART_TxFifoNotFullFlag & USART_GetStatusFlags(UART_BASE)));
            USART_WriteByte(UART_BASE, (uint8_t)a);
        }
        while (!(kUSART_TxFifoNotFullFlag & USART_GetStatusFlags(UART_BASE)));
        USART_WriteByte(UART_BASE, *(str + i));
    }
}


