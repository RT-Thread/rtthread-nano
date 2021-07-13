/*
 * Copyright (c) 2006-2020, YuZhaorong 
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-06     YuZhaorong      
 */

#include <rtthread.h>
#include <rthw.h>

#define TASK1_STACK_SIZE 512
struct rt_thread task1={0};
unsigned char task1_stack[TASK1_STACK_SIZE]={0};

#define TASK2_STACK_SIZE 512
struct rt_thread task2={0};
unsigned char task2_stack[TASK2_STACK_SIZE]={0};
void m_putchar(const char ch)
{
    *(volatile int*)0x02000008= (int)ch;
}

void rt_hw_console_output(const char *str)
{
    int i=0;
    for(i=0;'\0' != str[i];i++)
    {
         m_putchar(str[i]);
    }
}

void thread_task1_entry(void* paramenter)
{
    while (1)
    { 
        rt_kprintf("\r\n task1 runing...out A \r\n");
        rt_thread_delay(2);
    }
}
void thread_task2_entry(void* paramenter)
{
    while (1)
    { 
        rt_kprintf("\r\n task2 runing... out B \r\n");
        rt_thread_delay(3);
    }
}
int main(void)
{
    rt_hw_interrupt_umask(0);  // 注册定时器中断 
    
    
    rt_kprintf("hello picorv32 world\r\n");
    
    
    rt_thread_init(&task1, "task1",thread_task1_entry, 0,(void*)task1_stack,TASK1_STACK_SIZE,4, 100);
    rt_thread_startup(&task1);
    
    rt_thread_init(&task2, "task2",thread_task2_entry, 0,(void*)task2_stack,TASK2_STACK_SIZE,4, 100);
    rt_thread_startup(&task2);
    
    while (1)
    {
        rt_thread_delay(500);
       
    }
    return 0;
}
