/*
 * Copyright (c) 2017 - 2018 , NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __APP_H__
#define __APP_H__

// C Standard Lib includes
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// LPC55xx Chip peripherals & Boards includes
#include "clock_config.h"

#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "fsl_spi.h"
#include "fsl_i2c.h"
#include "fsl_usart.h"
#include "fsl_mrt.h"
#include "fsl_inputmux.h"
#include "fsl_pint.h"
#include "fsl_iocon.h"
#include "fsl_gint.h"
#include "fsl_rtc.h"
#include "fsl_ostimer.h"
#include "fsl_iap.h"
#include "fsl_iap_ffr.h"
#include "fsl_puf.h"
#include "fsl_prince.h"
#include "fsl_rng.h"
#include "fsl_casper.h"
#include "fsl_casper.h"
#include "fsl_hashcrypt.h"
#include "fsl_common.h"
#include "fsl_iocon.h"

#include "app_printf.h"

// TrustZone
#include "tzm_config.h"
#include "arm_cmse.h"
#include "veneer_table.h"

extern volatile uint32_t g_SysTicks;


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PFR_STORAGEADDR            0x0009C000   /*  16KB  PFR Address */

#define UNSECURE_IMAGEADDR         0x00020000   /*  128KB  None Secure Image Address */
#define VENEER_TABLEADDR           0x0001F000   /*   1KB  NSC - secure, non-secure callable memory */
#define VENEER_TABLESIZE           0x00001000
#define SECURE_IMAGEADDR           0x10000000   /*  0  Secure Image Address */

#define OSINTERVAL                 1000

#define DEBUG_USART_ENABLE         1
/* Debug USART port */
#define DEBUG_USART                USART0
#define DEBUG_UART_TYPE            kSerialPort_Uart

#define BOARD_NAME                 "SmartElock_LPC5500"

#define DEBUG_UART_INSTANCE        2U
#define DEBUG_UART_CLK_FREQ        12000000U
#define DEBUG_UART_CLK_ATTACH      kFRO12M_to_FLEXCOMM2
#define DEBUG_UART_RST             kFC2_RST_SHIFT_RSTn
#define DEBUG_UART_CLKSRC          kCLOCK_Flexcomm2
#define DEBUG_UART_IRQ_HANDLER     FLEXCOMM2_IRQHandler
#define DEBUG_UART_IRQNUM          FLEXCOMM2_IRQn
#define DEBUG_UART_BAUDRATE        115200U

#endif
