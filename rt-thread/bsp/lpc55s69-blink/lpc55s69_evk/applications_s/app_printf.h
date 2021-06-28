/*
 * Copyright (c) 2017 - 2018 , NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_PRINTF_H__
#define __APP_PRINTF_H__

#include "stdint.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
 
extern void     debug_init(void);
extern uint32_t debug_printf(const char *formatString, ...);
 
#define PRINTF  debug_printf

#endif

