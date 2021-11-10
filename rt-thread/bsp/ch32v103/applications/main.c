/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-04     iysheng      first version
 */

#include <rtthread.h>
#include <board.h>

int main(void)
{

    while(1)
    {
        *(volatile rt_uint32_t *)0x4001080c = 0x00;
        rt_thread_mdelay(1000);
        *((volatile rt_uint32_t *)0x4001080c) = 0x40;
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
