/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-11-05     iysheng           the first version
 */

#include "ch32_pinmux.h"

static struct _ch32_pinmux_map gs_ch32v103_map = {
    /* 0 1 2 3
     * 4 5 6 7
     * 8 9 a b
     * c d e f */
    {{{PINMUX_DEFAULT, PINMUX_DEFAULT, GPIO_Mode_AF_PP, GPIO_Mode_IN_FLOATING,
     PINMUX_DEFAULT, PINMUX_DEFAULT, GPIO_Mode_Out_PP, PINMUX_DEFAULT,
     PINMUX_DEFAULT, GPIO_Mode_AF_PP, GPIO_Mode_IN_FLOATING, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT},
     GPIOA, RCC_APB2Periph_GPIOA}, /* GPIOA */
    {{PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, GPIO_Mode_AF_PP, GPIO_Mode_IN_FLOATING,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT},
     GPIOB, RCC_APB2Periph_GPIOB}, /* GPIOB */
    {{PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
    }, GPIOC, RCC_APB2Periph_GPIOC}, /* GPIOC */
    {{PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
     PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT, PINMUX_DEFAULT,
    }, GPIOD, RCC_APB2Periph_GPIOD}, /* GPIOD */
    },
};

/**
  * @brief Init pin mux config
  * retval .
  */
int ch32v103_pinmux_init(void)
{
    int port_index = 0, pin_index = 0;
    GPIO_InitTypeDef gpio_initstruct = {0};
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;

    for (; port_index < MAX_PORT_COUNTS; port_index++)
    {
        for (pin_index = 0; pin_index < MAX_PIN_COUNTS_1PORT; pin_index++)
        {
            if (gs_ch32v103_map.port[port_index].pin_mode[pin_index] != PINMUX_DEFAULT)
            {
                if (PORT_CLK_STATE_DISABLE == gs_ch32v103_map.port_clk_state[port_index])
                {
                    RCC_APB2PeriphClockCmd(gs_ch32v103_map.port[port_index].port_clk, ENABLE);
                    gs_ch32v103_map.port_clk_state[port_index] = PORT_CLK_STATE_ENABLE;
                }
                gpio_initstruct.GPIO_Pin = 1 << pin_index;
                gpio_initstruct.GPIO_Mode = gs_ch32v103_map.port[port_index].pin_mode[pin_index];
                GPIO_Init(gs_ch32v103_map.port[port_index].port_base, &gpio_initstruct);
            }
            else
            {
                continue;
            }
        }
    }

    return 0;
}
