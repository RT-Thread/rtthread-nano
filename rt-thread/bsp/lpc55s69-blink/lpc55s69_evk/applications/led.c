/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
  */
 
#include "fsl_gpio.h"

#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <shell.h>
#include <finsh.h>
#endif

void led_init(rt_uint32_t port, rt_uint32_t pin)
{
    gpio_pin_config_t pin_config = {kGPIO_DigitalOutput, 1};
    GPIO_PinInit(GPIO, port, pin, &pin_config);
}

void led_on(rt_uint32_t port, rt_uint32_t pin)
{
    GPIO_PinWrite(GPIO, port,  pin, 1);
}

void led_off(rt_uint32_t port, rt_uint32_t pin)
{
    GPIO_PinWrite(GPIO, port,  pin, 0);
}

void led_entry(void *para)
{
    while(1)
    {
        led_on(1,6);
        rt_thread_mdelay(500);
        led_off(1,6);
        rt_thread_mdelay(500);
    }
}

int led_sample(void)
{
    rt_thread_t tid;
    
    rt_kprintf("This is a led sample\n");
    led_init(1,6);
    
    tid = rt_thread_create("led",
                           led_entry, (void *)5,
                           1024, 15, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
    return 0;
}

