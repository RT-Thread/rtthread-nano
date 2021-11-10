/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-26     iysheng      first version
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "ch32v10x.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CH32_FLASH_START_ADRESS     ((uint32_t)0x08000000)
#define FLASH_PAGE_SIZE             (64)
#define CH32_FLASH_SIZE             (1024 * 1024)
#define CH32_FLASH_END_ADDRESS      ((uint32_t)(CH32_FLASH_START_ADRESS + CH32_FLASH_SIZE))

#define CH32_SRAM_SIZE      20
#define CH32_SRAM_END       (0x20000000 + CH32_SRAM_SIZE * 1024)

#if defined(__CC_ARM) || defined(__CLANG_ARM)
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN      ((void *)&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="CSTACK"
#define HEAP_BEGIN      (__segment_end("CSTACK"))
#else
extern int _ebss;
#define HEAP_BEGIN      ((void *)&_ebss)
#endif

#define HEAP_END        CH32_SRAM_END

#ifdef __cplusplus
}
#endif


rt_uint32_t ch32_get_sysclock_frequency(void);

#ifdef BSP_USING_UART
void ch32f1_usart_clock_and_io_init(USART_TypeDef* usartx);
#endif

#ifdef BSP_USING_SPI
void ch32f1_spi_clock_and_io_init(SPI_TypeDef* spix);
rt_uint32_t ch32f1_spi_clock_get(SPI_TypeDef* spix);
#endif

#ifdef BSP_USING_HWI2C
void ch32f1_i2c_clock_and_io_init(I2C_TypeDef* i2cx);
void ch32f1_i2c_config(I2C_TypeDef* i2cx);
#endif

#ifdef BSP_USING_TIM
void ch32f1_tim_clock_init(TIM_TypeDef *timx);
rt_uint32_t ch32f1_tim_clock_get(TIM_TypeDef *timx);

#ifdef BSP_USING_HWTIMER
struct rt_hwtimer_info* ch32f1_hwtimer_info_config_get(TIM_TypeDef *timx);
#endif

#ifdef BSP_USING_PWM
void ch32f1_pwm_io_init(TIM_TypeDef *timx, rt_uint8_t channel);
#endif
#endif



#endif /* __BOARD_H__ */
