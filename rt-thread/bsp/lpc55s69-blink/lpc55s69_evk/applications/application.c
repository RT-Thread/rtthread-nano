/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-08-08     Yang         the first version
 * 2019-07-19     Magicoe      The first version for LPC55S6x
 */

#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <shell.h>
#include <finsh.h>
#endif

#include <stdio.h>

/* thread phase init */
void rt_init_thread_entry(void *parameter)
{
    /* Initialization RT-Thread Components */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_init();
#elif defined(RT_USING_FINSH)
    finsh_system_init();
#endif
}

#include "fsl_common.h"
#include "fsl_clock.h"
void dump_clock(void)
{
    rt_kprintf("OSC   clock : %d\r\n",                  CLOCK_GetFreq(kCLOCK_CoreSysClk) );
}

void dump_cc_info(void)
{
#if defined(__CC_ARM)
    rt_kprintf("using armcc, version: %d\n", __ARMCC_VERSION);
#elif defined(__CLANG_ARM)
    rt_kprintf("using armclang, version: %d\n", __ARMCC_VERSION);
#elif defined(__ICCARM__)
    rt_kprintf("using iccarm, version: %d\n", __VER__);
#elif defined(__GNUC__)
    rt_kprintf("using gcc, version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
#endif
}

void dump_link_info(void)
{
#if defined(__CC_ARM)
    
#elif defined(__CLANG_ARM)
    
#elif defined(__ICCARM__)

#elif defined(__GNUC__)
    #define DUMP_SYMBOL(__SYM)                  \
        extern int __SYM;                       \
        rt_kprintf("%s: %p\n", #__SYM, &__SYM)

    DUMP_SYMBOL(__fsymtab_start);
    DUMP_SYMBOL(__fsymtab_end);
    DUMP_SYMBOL(__vsymtab_start);
    DUMP_SYMBOL(__vsymtab_end);
    DUMP_SYMBOL(__rt_init_start);
    DUMP_SYMBOL(__rt_init_end);

    DUMP_SYMBOL(__exidx_start);
    DUMP_SYMBOL(__exidx_end);

    DUMP_SYMBOL(__etext);

    DUMP_SYMBOL(__data_start__);
    DUMP_SYMBOL(__data_end__);

    DUMP_SYMBOL(__noncachedata_start__);
    DUMP_SYMBOL(__noncachedata_init_end__);

    DUMP_SYMBOL(__noncachedata_end__);

    DUMP_SYMBOL(__bss_start__);
    DUMP_SYMBOL(__bss_end__);

    DUMP_SYMBOL(stack_start);
    DUMP_SYMBOL(stack_end);

    DUMP_SYMBOL(heap_start);
#endif
}

extern void DbgSecureThreadFloat(void *param);
extern void DbgConsole_Printf_NSE(char const *s, int id);

extern void rt_trustzone_init(void);
extern rt_ubase_t rt_trustzone_enter(rt_ubase_t module);
extern void rt_trustzone_exit(rt_ubase_t context);

static void _main_thread_entry(void *param)
{
    char buff[128];
    rt_ubase_t context;

    rt_trustzone_init();
    while (1)
    {
        rt_sprintf(buff, "thread:%d call Secure API! tick:%d\r\n", param, rt_tick_get());
        context = rt_trustzone_enter(0);
        DbgConsole_Printf_NSE(buff, context);
//        DbgSecureThreadFloat(param);
        rt_trustzone_exit(context);
        rt_thread_mdelay(3000);
    }
}

static void _main_thread_entry_1(void *param)
{
    char buff[128];
    rt_ubase_t context;

    rt_trustzone_init();
    context = rt_trustzone_enter(0);
    while (1)
    {
        rt_sprintf(buff, "thread:%d context:%d call Secure API! tick:%d\r\n", param, context, rt_tick_get());
        DbgConsole_Printf_NSE(buff, context);
//        DbgSecureThreadFloat(param);
        rt_thread_mdelay(3000);
    }
    rt_trustzone_exit(context);
}

static void _main_thread_entry_2(void *param)
{
    float a, b, c, d;
    char buff[64];

    a = 0.001;
    b = 0.001;
    c = 0.003;
    d = 0.0;
    while (1)
    {
        d += (a + b + c) * 2;
        if (d > 100000)
        {
            sprintf(buff, "thread param :%p d:%f\r\n", param, d);
            rt_kprintf(buff);
            d = 0;
        }
    }
}

int rt_application_init(void)
{
    rt_thread_t tid;
    
    dump_clock();
    dump_cc_info();
    dump_link_info();
    
    tid = rt_thread_create("init",
                           rt_init_thread_entry, RT_NULL,
                           2048, RT_THREAD_PRIORITY_MAX / 3, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    tid = rt_thread_create("0_main",
                           _main_thread_entry, (void *)0,
                           2048, 29, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    tid = rt_thread_create("1_main",
                           _main_thread_entry, (void *)1,
                           2048, 29, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    tid = rt_thread_create("2_main",
                           _main_thread_entry_1, (void *)2,
                           2048, 29, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    tid = rt_thread_create("3_main",
                           _main_thread_entry_1, (void *)3,
                           2048, 29, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    tid = rt_thread_create("4_main",
                           _main_thread_entry_2, (void *)4,
                           2048, 29, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    tid = rt_thread_create("5_main",
                           _main_thread_entry_2, (void *)5,
                           2048, 29, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);

    extern int led_sample(void);
    led_sample();

    return 0;
}
