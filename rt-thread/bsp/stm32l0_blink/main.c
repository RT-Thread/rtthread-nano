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

#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_utils.h"

int main(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    /**/
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);

    /**/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    while (1)
    {
        rt_thread_delay(RT_TICK_PER_SECOND);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6 | LL_GPIO_PIN_7);
        
        rt_thread_delay(RT_TICK_PER_SECOND);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5 | LL_GPIO_PIN_7);
        
        rt_thread_delay(RT_TICK_PER_SECOND);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7);
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5 | LL_GPIO_PIN_6);
    }
}

