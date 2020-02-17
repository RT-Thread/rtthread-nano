/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-06     yangjie      the first version
 */

#include "gd32vf103v_eval.h"
#include <rtthread.h>

int main(void)
{
    gd_eval_led_init(LED1);

    while (1)
    {
        gd_eval_led_on(LED1);
        rt_thread_mdelay(500);
        gd_eval_led_off(LED1);
        rt_thread_mdelay(500);
    }
}
