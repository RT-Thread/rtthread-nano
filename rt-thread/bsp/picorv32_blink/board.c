/*
 * Copyright (c) 2020-2020, YuZhaorong
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-09     YuZhaorong        the first version
 */
 
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>

#define  TIMER_IRQ_VECTOR   0 
#define  ECALL_IRQ_VECTOR   1 
#define  SYSTEM_BUS_VECTOR  2 
#define  SYSTEM_CORE_CLOCK 10000000l   //  10 MHZ  
// Holds the system core clock, which is the system clock 
// frequency supplied to the SysTick timer and the processor 
// core clock.
extern uint32_t riscv_timer(uint32_t time);
static uint32_t sys_timer_ticks = 0 ; 
static uint32_t _riscv_time_config(rt_uint32_t ticks)
{
    sys_timer_ticks = ticks;
    return riscv_timer(ticks);
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];     // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

void riscv_timer_handler(int vector, void *param)
{
    riscv_timer(sys_timer_ticks);
    rt_tick_increase();

}
void riscv_ecall_handler(int vector, void *param)
{

}
/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    


    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif


    /* System time Configuration */
    _riscv_time_config(SYSTEM_CORE_CLOCK / RT_TICK_PER_SECOND);
    /*Register System timer interrupt function*/
    rt_hw_interrupt_init();
    rt_hw_interrupt_install(TIMER_IRQ_VECTOR,riscv_timer_handler,RT_NULL,"riscv_timer");
    rt_hw_interrupt_install(ECALL_IRQ_VECTOR,riscv_ecall_handler,RT_NULL,"riscv_ecall");
    rt_hw_interrupt_umask(ECALL_IRQ_VECTOR);  
}


