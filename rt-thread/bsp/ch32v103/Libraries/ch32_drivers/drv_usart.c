/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-08-10     charlown          first version
 * 2021-11-05     iysheng           port to ch32v10x
 */

#include <rtthread.h>
#include "board.h"
#include "riscv-ops.h"
#include "drv_uart.h"
#include "ch32v10x_rcc.h"
#include "ch32v10x_usart.h"
#include "ch32v10x_misc.h"
#include "ringbuffer.h"

#define UART_RECV_BUFFER_SIZE    64

#ifdef RT_USING_SERIAL
#ifndef ITEM_NUM
#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
#endif

struct usart_device
{
    struct rt_device parent;
    char *name;
    USART_TypeDef *periph;
    IRQn_Type irqn;
    struct rt_ringbuffer * recv_buffer;
};

#ifdef BSP_USING_UART1
static struct usart_device usart_device1 =
    {
        .name = "uart1",
        .periph = USART1,
        .irqn = USART1_IRQn,
};
#endif

#ifdef BSP_USING_UART2
static struct usart_device usart_device2 =
    {
        .name = "uart2",
        .periph = USART2,
        .irqn = USART2_IRQn,
};
#endif
#ifdef BSP_USING_UART3
static struct usart_device usart_device3 =
    {
        .name = "uart3",
        .periph = USART3,
        .irqn = USART3_IRQn,
};
#endif

static rt_err_t ch32v1_usart_init(rt_device_t serial)
{
    struct usart_device * usart_device = (struct usart_device *) serial;
    USART_InitTypeDef USART_InitStructure;

    USART_StructInit(&USART_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_Init(usart_device->periph, &USART_InitStructure);
    USART_Cmd(usart_device->periph, ENABLE);

    usart_device->recv_buffer = rt_ringbuffer_create(UART_RECV_BUFFER_SIZE);
    if (!usart_device->recv_buffer)
    {
        return RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t ch32v1_usart_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
        default:
            return RT_EINVAL;
    }
}

static rt_size_t ch32v1_usart_write(rt_device_t serial, rt_off_t pos, const void *buffer, rt_size_t size)
{
    struct usart_device *usart_dev;
    int i = 0;

    RT_ASSERT(serial != RT_NULL);

    usart_dev = (struct usart_device *)serial;

    RT_ASSERT(usart_dev != RT_NULL);

    for (; i < size; i++)
    {
        USART_SendData(usart_dev->periph, *((uint8_t *)buffer + i));
        while (USART_GetFlagStatus(usart_dev->periph, USART_FLAG_TXE) == RESET);
    }

    return size;
}

static rt_size_t ch32v1_usart_read(rt_device_t serial, rt_off_t pos, void *buffer, rt_size_t size)
{
    struct usart_device *usart_dev;
    rt_size_t recv_len;

    RT_ASSERT(serial != RT_NULL);

    usart_dev = (struct usart_device *)serial;

    RT_ASSERT(usart_dev != RT_NULL);

    do {
        recv_len = rt_ringbuffer_get(usart_dev->recv_buffer, buffer, size);
    } while(serial == rt_console_get_device() && !recv_len);

    return recv_len;
}


rt_err_t ch32v1_usart_open(rt_device_t serial, rt_uint16_t oflag)
{
    struct usart_device * usart_dev;
    NVIC_InitTypeDef NVIC_InitStruct;

    usart_dev = (struct usart_device *)serial;

    RT_ASSERT(usart_dev != RT_NULL);

    if (oflag & RT_DEVICE_FLAG_INT_RX)
    {
        NVIC_InitStruct.NVIC_IRQChannel = usart_dev->irqn;
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
        USART_ITConfig(usart_dev->periph, USART_IT_RXNE, ENABLE);
    }

    return RT_EOK;
}

static const struct rt_device_ops usart_ops = {
    .init = ch32v1_usart_init,
    .open = ch32v1_usart_open,
    .write = ch32v1_usart_write,
    .read = ch32v1_usart_read,
    .control = ch32v1_usart_control,
};

static void usart_isr(struct usart_device *usart_dev)
{

    RT_ASSERT(usart_dev != RT_NULL);

    if ((USART_GetITStatus(usart_dev->periph, USART_IT_RXNE) != RESET) && (RESET != USART_GetFlagStatus(usart_dev->periph, USART_FLAG_RXNE)))
    {
        USART_ClearITPendingBit(usart_dev->periph, USART_IT_RXNE);
        USART_ClearFlag(usart_dev->periph, USART_FLAG_RXNE);
        rt_ringbuffer_putchar(usart_dev->recv_buffer, USART_ReceiveData(usart_dev->periph) & 0xff);
    }
    else
    {
        if (USART_GetFlagStatus(usart_dev->periph, USART_FLAG_CTS) != RESET)
        {
            USART_ClearFlag(usart_dev->periph, USART_FLAG_CTS);
        }

        if (USART_GetFlagStatus(usart_dev->periph, USART_FLAG_TXE) != RESET)
        {
            USART_ClearFlag(usart_dev->periph, USART_FLAG_TXE);
        }

        if (USART_GetFlagStatus(usart_dev->periph, USART_FLAG_LBD) != RESET)
        {
            USART_ClearFlag(usart_dev->periph, USART_FLAG_LBD);
        }
    }
}

#ifdef BSP_USING_UART1
void USART1_IRQHandler(void)
{
    switch_to_interrupt_stack();
    rt_interrupt_enter();

    usart_isr(&usart_device1);

    rt_interrupt_leave();
    switch_from_interrupt_stack();
}
#endif

#ifdef BSP_USING_UART2
__attribute__((interrupt()))
void USART2_IRQHandler(void)
{
    switch_to_interrupt_stack();
    rt_interrupt_enter();

    usart_isr(&usart_device2);

    rt_interrupt_leave();
    switch_from_interrupt_stack();
}
#endif
#ifdef BSP_USING_UART3
__attribute__((interrupt()))
void USART3_IRQHandler(void)
{
    switch_to_interrupt_stack();
    rt_interrupt_enter();

    usart_isr(&usart_device3);

    rt_interrupt_leave();
    switch_from_interrupt_stack();
}
#endif

int rt_hw_uart_init(void)
{
#ifdef BSP_USING_UART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    usart_device1.parent.ops = &usart_ops;
    rt_device_register(&usart_device1.parent, usart_device1.name,
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX);
#endif

#ifdef BSP_USING_UART2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    usart_device2.parent.ops = &usart_ops;
    rt_device_register(&usart_device2.parent, usart_device2.name,
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX);
#endif

#ifdef BSP_USING_UART3
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    usart_device3.parent.ops = &usart_ops;
    rt_device_register(&usart_device3.parent, usart_device3.name,
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX);
#endif

    return RT_EOK;
}

#endif /* BSP_USING_UART */
