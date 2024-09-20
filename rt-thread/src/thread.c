/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-28     Bernard      first version
 * 2006-04-29     Bernard      implement thread timer
 * 2006-04-30     Bernard      added THREAD_DEBUG
 * 2006-05-27     Bernard      fixed the rt_thread_yield bug
 * 2006-06-03     Bernard      fixed the thread timer init bug
 * 2006-08-10     Bernard      fixed the timer bug in thread_sleep
 * 2006-09-03     Bernard      changed rt_timer_delete to rt_timer_detach
 * 2006-09-03     Bernard      implement rt_thread_detach
 * 2008-02-16     Bernard      fixed the rt_thread_timeout bug
 * 2010-03-21     Bernard      change the errno of rt_thread_delay/sleep to
 *                             RT_EOK.
 * 2010-11-10     Bernard      add cleanup callback function in thread exit.
 * 2011-09-01     Bernard      fixed rt_thread_exit issue when the current
 *                             thread preempted, which reported by Jiaxing Lee.
 * 2011-09-08     Bernard      fixed the scheduling issue in rt_thread_startup.
 * 2012-12-29     Bernard      fixed compiling warning.
 * 2016-08-09     ArdaFu       add thread suspend and resume hook.
 * 2017-04-10     armink       fixed the rt_thread_delete and rt_thread_detach
 *                             bug when thread has not startup.
 * 2018-11-22     Jesven       yield is same to rt_schedule
 *                             add support for tasks bound to cpu
 * 2021-02-24     Meco Man     rearrange rt_thread_control() - schedule the thread when close it
 * 2021-11-15     THEWON       Remove duplicate work between idle and _thread_exit
 * 2021-12-27     Meco Man     remove .init_priority
 * 2022-01-07     Gabriel      Moving __on_rt_xxxxx_hook to thread.c
 * 2022-01-24     THEWON       let rt_thread_sleep return thread->error when using signal
 */

#include <rthw.h>
#include <rtthread.h>
#include <stddef.h>

#ifndef __on_rt_thread_inited_hook
    #define __on_rt_thread_inited_hook(thread)      __ON_HOOK_ARGS(rt_thread_inited_hook, (thread))
#endif
#ifndef __on_rt_thread_suspend_hook
    #define __on_rt_thread_suspend_hook(thread)     __ON_HOOK_ARGS(rt_thread_suspend_hook, (thread))
#endif
#ifndef __on_rt_thread_resume_hook
    #define __on_rt_thread_resume_hook(thread)      __ON_HOOK_ARGS(rt_thread_resume_hook, (thread))
#endif

#if defined(RT_USING_HOOK) && defined(RT_HOOK_USING_FUNC_PTR)
static void (*rt_thread_suspend_hook)(rt_thread_t thread);
static void (*rt_thread_resume_hook) (rt_thread_t thread);
static void (*rt_thread_inited_hook) (rt_thread_t thread);

/**
 * @brief   This function sets a hook function when the system suspend a thread.
 *
 * @note    The hook function must be simple and never be blocked or suspend.
 *
 * @param   hook is the specified hook function.
 */
void rt_thread_suspend_sethook(void (*hook)(rt_thread_t thread))
{
    rt_thread_suspend_hook = hook;
}

/**
 * @brief   This function sets a hook function when the system resume a thread.
 *
 * @note    The hook function must be simple and never be blocked or suspend.
 *
 * @param   hook is the specified hook function.
 */
void rt_thread_resume_sethook(void (*hook)(rt_thread_t thread))
{
    rt_thread_resume_hook = hook;
}

/**
 * @brief   This function sets a hook function when a thread is initialized.
 *
 * @param   hook is the specified hook function.
 */
void rt_thread_inited_sethook(void (*hook)(rt_thread_t thread))
{
    rt_thread_inited_hook = hook;
}
#endif /* defined(RT_USING_HOOK) && defined(RT_HOOK_USING_FUNC_PTR) */

static void _thread_exit(void)
{
    struct rt_thread *thread;
    rt_base_t level;

    /* get current thread */
    thread = rt_thread_self();

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* remove from schedule */
    rt_schedule_remove_thread(thread);

    /* remove it from timer list */
    rt_timer_detach(&thread->thread_timer);

    /* change stat */
    thread->stat = RT_THREAD_CLOSE;

    /* insert to defunct thread list */
    rt_thread_defunct_enqueue(thread);

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    /* switch to next task */
    rt_schedule();
}

/**
 * @brief   This function is the timeout function for thread, normally which is invoked
 *          when thread is timeout to wait some resource.
 *
 * @param   parameter is the parameter of thread timeout function
 */
static void _thread_timeout(void *parameter)
{
    struct rt_thread *thread;
    rt_base_t level;

    thread = (struct rt_thread *)parameter;

    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_SUSPEND);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* set error number */
    thread->error = -RT_ETIMEOUT;

    /* remove from suspend list */
    rt_list_remove(&(thread->tlist));

    /* insert to schedule ready list */
    rt_schedule_insert_thread(thread);

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    /* do schedule */
    rt_schedule();
}

static rt_err_t _thread_init(struct rt_thread *thread,
                             const char       *name,
                             void (*entry)(void *parameter),
                             void             *parameter,
                             void             *stack_start,
                             rt_uint32_t       stack_size,
                             rt_uint8_t        priority,
                             rt_uint32_t       tick)
{
    /* init thread list */
    rt_list_init(&(thread->tlist));

    thread->entry = (void *)entry;
    thread->parameter = parameter;

    /* stack init */
    thread->stack_addr = stack_start;
    thread->stack_size = stack_size;

    /* init thread stack */
    rt_memset(thread->stack_addr, '#', thread->stack_size);
#ifdef ARCH_CPU_STACK_GROWS_UPWARD
    thread->sp = (void *)rt_hw_stack_init(thread->entry, thread->parameter,
                                          (void *)((char *)thread->stack_addr),
                                          (void *)_thread_exit);
#else
    thread->sp = (void *)rt_hw_stack_init(thread->entry, thread->parameter,
                                          (rt_uint8_t *)((char *)thread->stack_addr + thread->stack_size - sizeof(rt_ubase_t)),
                                          (void *)_thread_exit);
#endif /* ARCH_CPU_STACK_GROWS_UPWARD */

    /* priority init */
    RT_ASSERT(priority < RT_THREAD_PRIORITY_MAX);
    thread->current_priority = priority;

    thread->number_mask = 0;

#ifdef RT_USING_EVENT
    thread->event_set = 0;
    thread->event_info = 0;
#endif /* RT_USING_EVENT */

#if RT_THREAD_PRIORITY_MAX > 32
    thread->number = 0;
    thread->high_mask = 0;
#endif /* RT_THREAD_PRIORITY_MAX > 32 */

    /* tick init */
    thread->init_tick      = tick;
    thread->remaining_tick = tick;

    /* error and flags */
    thread->error = RT_EOK;
    thread->stat  = RT_THREAD_INIT;

#ifdef RT_USING_SMP
    /* not bind on any cpu */
    thread->bind_cpu = RT_CPUS_NR;
    thread->oncpu = RT_CPU_DETACHED;

    /* lock init */
    thread->scheduler_lock_nest = 0;
    thread->cpus_lock_nest = 0;
    thread->critical_lock_nest = 0;
#endif /* RT_USING_SMP */

    /* initialize cleanup function and user data */
    thread->cleanup   = 0;
    thread->user_data = 0;

    /* initialize thread timer */
    rt_timer_init(&(thread->thread_timer),
                  thread->name,
                  _thread_timeout,
                  thread,
                  0,
                  RT_TIMER_FLAG_ONE_SHOT);

    /* initialize signal */
#ifdef RT_USING_SIGNALS
    thread->sig_mask    = 0x00;
    thread->sig_pending = 0x00;

#ifndef RT_USING_SMP
    thread->sig_ret     = RT_NULL;
#endif /* RT_USING_SMP */
    thread->sig_vectors = RT_NULL;
    thread->si_list     = RT_NULL;
#endif /* RT_USING_SIGNALS */

#ifdef RT_USING_LWP
    thread->lwp = RT_NULL;
#endif /* RT_USING_LWP */

#ifdef RT_USING_CPU_USAGE
    thread->duration_tick = 0;
#endif /* RT_USING_CPU_USAGE */

#ifdef RT_USING_PTHREADS
    thread->pthread_data = RT_NULL;
#endif /* RT_USING_PTHREADS */

#ifdef RT_USING_MODULE
    thread->module_id = 0;
#endif /* RT_USING_MODULE */

    RT_OBJECT_HOOK_CALL(rt_thread_inited_hook, (thread));

    return RT_EOK;
}

/**
 * @addtogroup Thread
 */

/**@{*/

/**
 * @brief   This function will initialize a thread. It's used to initialize a
 *          static thread object.
 *
 * @param   thread is the static thread object.
 *
 * @param   name is the name of thread, which shall be unique.
 *
 * @param   entry is the entry function of thread.
 *
 * @param   parameter is the parameter of thread enter function.
 *
 * @param   stack_start is the start address of thread stack.
 *
 * @param   stack_size is the size of thread stack.
 *
 * @param   priority is the priority of thread.
 *
 * @param   tick is the time slice if there are same priority thread.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_init(struct rt_thread *thread,
                        const char       *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size,
                        rt_uint8_t        priority,
                        rt_uint32_t       tick)
{
    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(stack_start != RT_NULL);

    /* initialize thread object */
    rt_object_init((rt_object_t)thread, RT_Object_Class_Thread, name);

    return _thread_init(thread,
                        name,
                        entry,
                        parameter,
                        stack_start,
                        stack_size,
                        priority,
                        tick);
}
RTM_EXPORT(rt_thread_init);

/**
 * @brief   This function will return self thread object.
 *
 * @return  The self thread object.
 */
rt_thread_t rt_thread_self(void)
{
#ifdef RT_USING_SMP
    rt_base_t lock;
    rt_thread_t self;

    lock = rt_hw_local_irq_disable();
    self = rt_cpu_self()->current_thread;
    rt_hw_local_irq_enable(lock);
    return self;
#else
    extern rt_thread_t rt_current_thread;

    return rt_current_thread;
#endif /* RT_USING_SMP */
}
RTM_EXPORT(rt_thread_self);

/**
 * @brief   This function will start a thread and put it to system ready queue.
 *
 * @param   thread is the thread to be started.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_startup(rt_thread_t thread)
{
    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_INIT);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);

    /* calculate priority attribute */
#if RT_THREAD_PRIORITY_MAX > 32
    thread->number      = thread->current_priority >> 3;            /* 5bit */
    thread->number_mask = 1L << thread->number;
    thread->high_mask   = 1L << (thread->current_priority & 0x07);  /* 3bit */
#else
    thread->number_mask = 1L << thread->current_priority;
#endif /* RT_THREAD_PRIORITY_MAX > 32 */

    RT_DEBUG_LOG(RT_DEBUG_THREAD, ("startup a thread:%s with priority:%d\n",
                                   thread->name, thread->current_priority));
    /* change thread stat */
    thread->stat = RT_THREAD_SUSPEND;
    /* then resume it */
    rt_thread_resume(thread);
    if (rt_thread_self() != RT_NULL)
    {
        /* do a scheduling */
        rt_schedule();
    }

    return RT_EOK;
}
RTM_EXPORT(rt_thread_startup);

/**
 * @brief   This function will detach a thread. The thread object will be removed from
 *          thread queue and detached/deleted from the system object management.
 *
 * @param   thread is the thread to be deleted.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_detach(rt_thread_t thread)
{
    rt_base_t level;

    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);
    RT_ASSERT(rt_object_is_systemobject((rt_object_t)thread));

    if ((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_CLOSE)
        return RT_EOK;

    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_INIT)
    {
        /* remove from schedule */
        rt_schedule_remove_thread(thread);
    }

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* release thread timer */
    rt_timer_detach(&(thread->thread_timer));

    /* change stat */
    thread->stat = RT_THREAD_CLOSE;

    /* insert to defunct thread list */
    rt_thread_defunct_enqueue(thread);

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}
RTM_EXPORT(rt_thread_detach);

#ifdef RT_USING_HEAP
/**
 * @brief   This function will create a thread object and allocate thread object memory.
 *          and stack.
 *
 * @param   name is the name of thread, which shall be unique.
 *
 * @param   entry is the entry function of thread.
 *
 * @param   parameter is the parameter of thread enter function.
 *
 * @param   stack_size is the size of thread stack.
 *
 * @param   priority is the priority of thread.
 *
 * @param   tick is the time slice if there are same priority thread.
 *
 * @return  If the return value is a rt_thread structure pointer, the function is successfully executed.
 *          If the return value is RT_NULL, it means this operation failed.
 */
rt_thread_t rt_thread_create(const char *name,
                             void (*entry)(void *parameter),
                             void       *parameter,
                             rt_uint32_t stack_size,
                             rt_uint8_t  priority,
                             rt_uint32_t tick)
{
    struct rt_thread *thread;
    void *stack_start;

    thread = (struct rt_thread *)rt_object_allocate(RT_Object_Class_Thread,
                                                    name);
    if (thread == RT_NULL)
        return RT_NULL;

    stack_start = (void *)RT_KERNEL_MALLOC(stack_size);
    if (stack_start == RT_NULL)
    {
        /* allocate stack failure */
        rt_object_delete((rt_object_t)thread);

        return RT_NULL;
    }

    _thread_init(thread,
                 name,
                 entry,
                 parameter,
                 stack_start,
                 stack_size,
                 priority,
                 tick);

    return thread;
}
RTM_EXPORT(rt_thread_create);

/**
 * @brief   This function will delete a thread. The thread object will be removed from
 *          thread queue and deleted from system object management in the idle thread.
 *
 * @param   thread is the thread to be deleted.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_delete(rt_thread_t thread)
{
    rt_base_t level;

    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);
    RT_ASSERT(rt_object_is_systemobject((rt_object_t)thread) == RT_FALSE);

    if ((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_CLOSE)
        return RT_EOK;

    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_INIT)
    {
        /* remove from schedule */
        rt_schedule_remove_thread(thread);
    }

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* release thread timer */
    rt_timer_detach(&(thread->thread_timer));

    /* change stat */
    thread->stat = RT_THREAD_CLOSE;

    /* insert to defunct thread list */
    rt_thread_defunct_enqueue(thread);

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}
RTM_EXPORT(rt_thread_delete);
#endif /* RT_USING_HEAP */

/**
 * @brief   This function will let current thread yield processor, and scheduler will
 *          choose the highest thread to run. After yield processor, the current thread
 *          is still in READY state.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_yield(void)
{
    struct rt_thread *thread;
    rt_base_t level;

    thread = rt_thread_self();
    level = rt_hw_interrupt_disable();
    thread->remaining_tick = thread->init_tick;
    thread->stat |= RT_THREAD_STAT_YIELD;
    rt_schedule();
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}
RTM_EXPORT(rt_thread_yield);

/**
 * @brief   This function will let current thread sleep for some ticks. Change current thread state to suspend,
 *          when the thread timer reaches the tick value, scheduler will awaken this thread.
 *
 * @param   tick is the sleep ticks.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_sleep(rt_tick_t tick)
{
    rt_base_t level;
    struct rt_thread *thread;

    /* set to current thread */
    thread = rt_thread_self();
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* reset thread error */
    thread->error = RT_EOK;

    /* suspend thread */
    rt_thread_suspend(thread);

    /* reset the timeout of thread timer and start it */
    rt_timer_control(&(thread->thread_timer), RT_TIMER_CTRL_SET_TIME, &tick);
    rt_timer_start(&(thread->thread_timer));

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    rt_schedule();

    /* clear error number of this thread to RT_EOK */
    if (thread->error == -RT_ETIMEOUT)
        thread->error = RT_EOK;

    return thread->error;
}

/**
 * @brief   This function will let current thread delay for some ticks.
 *
 * @param   tick is the delay ticks.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_delay(rt_tick_t tick)
{
    return rt_thread_sleep(tick);
}
RTM_EXPORT(rt_thread_delay);

/**
 * @brief   This function will let current thread delay until (*tick + inc_tick).
 *
 * @param   tick is the tick of last wakeup.
 *
 * @param   inc_tick is the increment tick.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_delay_until(rt_tick_t *tick, rt_tick_t inc_tick)
{
    rt_base_t level;
    struct rt_thread *thread;
    rt_tick_t cur_tick;

    RT_ASSERT(tick != RT_NULL);

    /* set to current thread */
    thread = rt_thread_self();
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* reset thread error */
    thread->error = RT_EOK;

    cur_tick = rt_tick_get();
    if (cur_tick - *tick < inc_tick)
    {
        rt_tick_t left_tick;

        *tick += inc_tick;
        left_tick = *tick - cur_tick;

        /* suspend thread */
        rt_thread_suspend(thread);

        /* reset the timeout of thread timer and start it */
        rt_timer_control(&(thread->thread_timer), RT_TIMER_CTRL_SET_TIME, &left_tick);
        rt_timer_start(&(thread->thread_timer));

        /* enable interrupt */
        rt_hw_interrupt_enable(level);

        rt_schedule();

        /* clear error number of this thread to RT_EOK */
        if (thread->error == -RT_ETIMEOUT)
        {
            thread->error = RT_EOK;
        }
    }
    else
    {
        *tick = cur_tick;
        rt_hw_interrupt_enable(level);
    }

    return thread->error;
}
RTM_EXPORT(rt_thread_delay_until);

/**
 * @brief   This function will let current thread delay for some milliseconds.
 *
 * @param   ms is the delay ms time.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_mdelay(rt_int32_t ms)
{
    rt_tick_t tick;

    tick = rt_tick_from_millisecond(ms);

    return rt_thread_sleep(tick);
}
RTM_EXPORT(rt_thread_mdelay);

/**
 * @brief   This function will control thread behaviors according to control command.
 *
 * @param   thread is the specified thread to be controlled.
 *
 * @param   cmd is the control command, which includes.
 *
 *              RT_THREAD_CTRL_CHANGE_PRIORITY for changing priority level of thread.
 *
 *              RT_THREAD_CTRL_STARTUP for starting a thread.
 *
 *              RT_THREAD_CTRL_CLOSE for delete a thread.
 *
 *              RT_THREAD_CTRL_BIND_CPU for bind the thread to a CPU.
 *
 * @param   arg is the argument of control command.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_control(rt_thread_t thread, int cmd, void *arg)
{
    rt_base_t level;

    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);

    switch (cmd)
    {
        case RT_THREAD_CTRL_CHANGE_PRIORITY:
        {
            /* disable interrupt */
            level = rt_hw_interrupt_disable();

            /* for ready thread, change queue */
            if ((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_READY)
            {
                /* remove thread from schedule queue first */
                rt_schedule_remove_thread(thread);

                /* change thread priority */
                thread->current_priority = *(rt_uint8_t *)arg;

                /* recalculate priority attribute */
    #if RT_THREAD_PRIORITY_MAX > 32
                thread->number      = thread->current_priority >> 3;            /* 5bit */
                thread->number_mask = 1 << thread->number;
                thread->high_mask   = 1 << (thread->current_priority & 0x07);   /* 3bit */
    #else
                thread->number_mask = 1 << thread->current_priority;
    #endif /* RT_THREAD_PRIORITY_MAX > 32 */

                /* insert thread to schedule queue again */
                rt_schedule_insert_thread(thread);
            }
            else
            {
                thread->current_priority = *(rt_uint8_t *)arg;

                /* recalculate priority attribute */
    #if RT_THREAD_PRIORITY_MAX > 32
                thread->number      = thread->current_priority >> 3;            /* 5bit */
                thread->number_mask = 1 << thread->number;
                thread->high_mask   = 1 << (thread->current_priority & 0x07);   /* 3bit */
    #else
                thread->number_mask = 1 << thread->current_priority;
    #endif /* RT_THREAD_PRIORITY_MAX > 32 */
            }

            /* enable interrupt */
            rt_hw_interrupt_enable(level);
            break;
        }

        case RT_THREAD_CTRL_STARTUP:
        {
            return rt_thread_startup(thread);
        }

        case RT_THREAD_CTRL_CLOSE:
        {
            rt_err_t rt_err;

            if (rt_object_is_systemobject((rt_object_t)thread) == RT_TRUE)
            {
                rt_err = rt_thread_detach(thread);
            }
    #ifdef RT_USING_HEAP
            else
            {
                rt_err = rt_thread_delete(thread);
            }
    #endif /* RT_USING_HEAP */
            rt_schedule();
            return rt_err;
        }

    #ifdef RT_USING_SMP
        case RT_THREAD_CTRL_BIND_CPU:
        {
            rt_uint8_t cpu;

            if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_INIT)
            {
                /* we only support bind cpu before started phase. */
                return RT_ERROR;
            }

            cpu = (rt_uint8_t)(rt_size_t)arg;
            thread->bind_cpu = cpu > RT_CPUS_NR? RT_CPUS_NR : cpu;
            break;
        }
    #endif /* RT_USING_SMP */

        default:
            break;
    }

    return RT_EOK;
}
RTM_EXPORT(rt_thread_control);

/**
 * @brief   This function will suspend the specified thread and change it to suspend state.
 *
 * @note    This function ONLY can suspend current thread itself.
 *              rt_thread_suspend(rt_thread_self());
 *
 *          Do not use the rt_thread_suspend to suspend other threads. You have no way of knowing what code a
 *          thread is executing when you suspend it. If you suspend a thread while sharing a resouce with
 *          other threads and occupying this resouce, starvation can occur very easily.
 *
 * @param   thread is the thread to be suspended.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_suspend(rt_thread_t thread)
{
    rt_base_t stat;
    rt_base_t level;

    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);
    RT_ASSERT(thread == rt_thread_self());

    RT_DEBUG_LOG(RT_DEBUG_THREAD, ("thread suspend:  %s\n", thread->name));

    stat = thread->stat & RT_THREAD_STAT_MASK;
    if ((stat != RT_THREAD_READY) && (stat != RT_THREAD_RUNNING))
    {
        RT_DEBUG_LOG(RT_DEBUG_THREAD, ("thread suspend: thread disorder, 0x%2x\n", thread->stat));
        return -RT_ERROR;
    }

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* change thread stat */
    rt_schedule_remove_thread(thread);
    thread->stat = RT_THREAD_SUSPEND | (thread->stat & ~RT_THREAD_STAT_MASK);

    /* stop thread timer anyway */
    rt_timer_stop(&(thread->thread_timer));

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    RT_OBJECT_HOOK_CALL(rt_thread_suspend_hook, (thread));
    return RT_EOK;
}
RTM_EXPORT(rt_thread_suspend);

/**
 * @brief   This function will resume a thread and put it to system ready queue.
 *
 * @param   thread is the thread to be resumed.
 *
 * @return  Return the operation status. If the return value is RT_EOK, the function is successfully executed.
 *          If the return value is any other values, it means this operation failed.
 */
rt_err_t rt_thread_resume(rt_thread_t thread)
{
    rt_base_t level;

    /* parameter check */
    RT_ASSERT(thread != RT_NULL);
    RT_ASSERT(rt_object_get_type((rt_object_t)thread) == RT_Object_Class_Thread);

    RT_DEBUG_LOG(RT_DEBUG_THREAD, ("thread resume:  %s\n", thread->name));

    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_SUSPEND)
    {
        RT_DEBUG_LOG(RT_DEBUG_THREAD, ("thread resume: thread disorder, %d\n",
                                       thread->stat));

        return -RT_ERROR;
    }

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* remove from suspend list */
    rt_list_remove(&(thread->tlist));

    rt_timer_stop(&thread->thread_timer);

    /* insert to schedule ready list */
    rt_schedule_insert_thread(thread);

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    RT_OBJECT_HOOK_CALL(rt_thread_resume_hook, (thread));
    return RT_EOK;
}
RTM_EXPORT(rt_thread_resume);

/**
 * @brief   This function will find the specified thread.
 *
 * @note    Please don't invoke this function in interrupt status.
 *
 * @param   name is the name of thread finding.
 *
 * @return  If the return value is a rt_thread structure pointer, the function is successfully executed.
 *          If the return value is RT_NULL, it means this operation failed.
 */
rt_thread_t rt_thread_find(char *name)
{
    return (rt_thread_t)rt_object_find(name, RT_Object_Class_Thread);
}

RTM_EXPORT(rt_thread_find);

/**@}*/
