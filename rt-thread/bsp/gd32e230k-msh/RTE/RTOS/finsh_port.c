/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <finsh_config.h>

#ifdef RT_USING_FINSH

/* TODO: console getchar */
RT_WEAK char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

//#error "Please implement the code according to your chip"

    return ch;
}

#endif /* RT_USING_FINSH */

