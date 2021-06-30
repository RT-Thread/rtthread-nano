/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include <board.h>
#include "gd32e230_libopt.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {
    }
    /* USER CODE END Error_Handler */
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    NVIC_SetPriority(SysTick_IRQn, 0);
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
 * This function will initial GD32 board.
 */
void rt_hw_board_init()
{
    /* NVIC Configuration */
#define NVIC_VTOR_MASK              0x3FFFFF80
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x10000000 */
    SCB->VTOR  = (0x10000000 & NVIC_VTOR_MASK);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    SCB->VTOR  = (0x08000000 & NVIC_VTOR_MASK);
#endif

    SystemClock_Config();

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init((void*)HEAP_BEGIN, (void*)HEAP_END);
#endif
}

/*@}*/



/*
 * The following is an example code of FinSH porting,
 * please reimplement the corresponding APIs according to the MCU you use.
 *
 * This part should be implemented by users:
 * (1) uart_init()
 * (2) rt_hw_console_output()
 * (3) rt_hw_console_getchar()
 */
#define UART_PERIPH  USART0
#define UART_IRQN    USART0_IRQn

#define UART_TX_PORT GPIOA
#define UART_TX_AF   GPIO_AF_1
#define UART_TX_PIN  GPIO_PIN_9

#define UART_RX_PORT GPIOA
#define UART_RX_AF   GPIO_AF_1
#define UART_RX_PIN  GPIO_PIN_10

#define UART_RX_CLK  RCU_GPIOA
#define UART_TX_CLK  RCU_GPIOA
#define UART_CLK     RCU_USART0

static int gd32_uart_init(void)
{
    /* enable USART clock */
    rcu_periph_clock_enable(UART_RX_CLK);
    rcu_periph_clock_enable(UART_CLK);
    
    /* connect port to USARTx_Tx */
    gpio_af_set(UART_TX_PORT, UART_TX_AF, UART_TX_PIN);
    gpio_mode_set(UART_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_TX_PIN);
    gpio_output_options_set(UART_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, UART_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_af_set(UART_RX_PORT, UART_RX_AF, UART_RX_PIN);
    gpio_mode_set(UART_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_RX_PIN);
    gpio_output_options_set(UART_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, UART_RX_PIN);
    
    usart_baudrate_set(UART_PERIPH, 115200);
    usart_word_length_set(UART_PERIPH, USART_WL_8BIT);
    usart_stop_bit_set(UART_PERIPH, USART_STB_1BIT);
    usart_parity_config(UART_PERIPH, USART_PM_NONE);
    
    usart_receive_config(UART_PERIPH, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART_PERIPH, USART_TRANSMIT_ENABLE);
    usart_enable(UART_PERIPH);
    
#if 0
    NVIC_SetPriority(UART_IRQN, 0);
    NVIC_EnableIRQ(UART_IRQN);
#endif
    
    return 0;
}
INIT_BOARD_EXPORT(gd32_uart_init);

void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';


    size = rt_strlen(str);

    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            usart_data_transmit(UART_PERIPH, (uint32_t )a);
            while((usart_flag_get(UART_PERIPH, USART_FLAG_TC) == RESET));
        }
        usart_data_transmit(UART_PERIPH, (uint32_t)*(str + i));
        while((usart_flag_get(UART_PERIPH, USART_FLAG_TC) == RESET));
    }
}

char rt_hw_console_getchar(void)
{
    /* the initial value of ch must < 0 */
    int ch = -1;

    if (usart_flag_get(UART_PERIPH, USART_FLAG_RBNE) != RESET)
    {
        ch = usart_data_receive(UART_PERIPH);
    }
    else
    {
        rt_thread_mdelay(10);
    }
    return ch;
}



