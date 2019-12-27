/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-06     yangjie      first edition
 */
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include <rtthread.h>

#define LD2_Pin LL_GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
static void MX_GPIO_Init(void);

int main(void)
{
    MX_GPIO_Init();

    while (1)
    {
        LL_GPIO_SetOutputPin(LD2_GPIO_Port, LD2_Pin);
        rt_thread_mdelay(500);

        LL_GPIO_ResetOutputPin(LD2_GPIO_Port, LD2_Pin);
        rt_thread_mdelay(500);
    }
}

static void MX_GPIO_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

