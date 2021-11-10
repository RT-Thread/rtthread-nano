/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-11-05     iysheng           the first version
 */

#ifndef __CH32_PINMUX_H__
#define __CH32_PINMUX_H__

#include <rtconfig.h>
#include <rtdef.h>
#include <stdint.h>
#ifdef SOC_RISCV_SERIES_CH32V103
#include "ch32v10x.h"
#include "ch32v10x_exti.h"
#include "ch32v10x_rcc.h"
#include "ch32v10x_gpio.h"
#endif

#define PINMUX_DEFAULT        0xff
#define PINAFIO_DEFAULT       0xff

#define MAX_PORT_COUNTS        4
#define MAX_PIN_COUNTS_1PORT   16

enum port_clk_state_E {
    PORT_CLK_STATE_DISABLE,
    PORT_CLK_STATE_ENABLE,
};

struct _ch32_port4pinmux {
    uint8_t pin_mode[MAX_PIN_COUNTS_1PORT];
    GPIO_TypeDef* port_base;
    uint32_t port_clk;
};

struct _ch32_pinmux_map {
    struct _ch32_port4pinmux port[MAX_PORT_COUNTS];
    uint8_t port_clk_state[MAX_PORT_COUNTS];
};

/**
  * @brief Init pin mux config
  * retval .
  */
int ch32v103_pinmux_init(void);
#endif /* ifndef __CH32_PINMUX_H__ */
