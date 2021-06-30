/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 * 2010-02-04     Magicoe      add board.h to LPC176x bsp
 * 2013-12-18     Bernard      porting to LPC4088 bsp
 * 2017-08-02     XiaoYang     porting to LPC54608 bsp
 */

#ifndef __BOARD_H__
#define __BOARD_H__


#include <stdint.h>

#include <rtthread.h>

#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_reset.h"
#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "pin_mux.h"

// <RDTConfigurator URL="http://www.rt-thread.com/eclipse">

// </RDTConfigurator>
#if defined(__CC_ARM) || defined(__CLANG_ARM)
extern int Image$$ARM_LIB_HEAP$$ZI$$Base;
#define HEAP_BEGIN  ((void *)&Image$$ARM_LIB_HEAP$$ZI$$Base)
#elif defined(__ICCARM__)
#pragma section="HEAP"
#define HEAP_BEGIN  (__segment_end("HEAP"))
#elif defined(__GNUC__)
extern int __bss_end;
#define HEAP_BEGIN  ((void *)&__bss_end)
#endif
#define HEAP_END    (void*)(0x20000000 + 0x40000)

void rt_hw_board_init(void);

#endif
