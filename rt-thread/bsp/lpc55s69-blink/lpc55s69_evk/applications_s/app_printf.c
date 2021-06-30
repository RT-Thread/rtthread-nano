/*
 * Copyright (c) 2017 - 2018 , NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include "app.h"
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void debug_init(void)
{
    usart_config_t      config;
    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUSART_ParityDisabled;
     * config.stopBitCount = kUSART_OneStopBit;
     * config.loopback = false;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    CLOCK_AttachClk(DEBUG_UART_CLK_ATTACH);
    USART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    config.enableTx = true;
    config.enableRx = true;
    
    USART_Init(DEBUG_USART, &config, 12000000);
    /* Enable RX interrupt. */
    USART_EnableInterrupts(DEBUG_USART, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable);
    NVIC_EnableIRQ(DEBUG_UART_IRQNUM);
}

/**
 * @brief   RESET BLE Module
 * @param   NULL
 * @return  NULL
 */
/* See fsl_debug_console.h for documentation of this function. */
uint32_t debug_printf(const char *formatString, ...)
{
    va_list arg;
    uint32_t logLength = 0U, result = 0U;
    uint8_t *PrintBuf = NULL;
    PrintBuf = malloc(256);                                               /* malloc buffer */
    if(PrintBuf == NULL) {
        PRINTF("Malloc BLE printf buffer failed\r\n");                    /*  */
        NVIC_SystemReset();                                               /* Can not malloc enough buffer, should be system issue, reset~ */
    }
    va_start(arg, formatString);
    logLength = vsprintf((char *)PrintBuf, formatString, arg);            /* format print log first */
    va_end(arg);
    USART_WriteBlocking(DEBUG_USART, PrintBuf, logLength);                /* Send Log data to BLE uart */
    free(PrintBuf);                                                       /* free buffer */
    return result;
}

// end file
