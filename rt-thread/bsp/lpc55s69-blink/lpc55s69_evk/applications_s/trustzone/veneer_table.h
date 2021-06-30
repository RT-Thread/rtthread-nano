/*
 * Copyright 2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _VENEER_TABLE_
#define _VENEER_TABLE_

/*******************************************************************************
 * This example doesn't use any function called from secure world.
 ******************************************************************************/


/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef  int (*callbackptr)(char const * s1, char const * s2);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Entry function for debug PRINTF (DbgConsole_Printf)
 *
 * This function provides interface between secure and normal worlds
 * This function is called from normal world only
 *
 * @param s     String to be printed
 *
*/
void DbgConsole_Printf_NSE(char const *s, int id);

/*!
 * @brief Entry function for two string comparison
 *
 * This function compares two strings by using non-secure callback
 * This function is called from normal world only
 *
 * @param callback     pointer to strcmp() function
 * @param s1           String to be compared
 * @param s2           String to be compared
 * @return             result of strcmp function
 *                     >0 for s1 > s2
 *                     =0 for s1 = s2
 *                     <0 for s1 < s2
*/

uint32_t StringCompare_NSE(volatile callbackptr callback, char const *s1, char const *s2);


typedef int (*led_func)(uint8_t s1)   __attribute__((cmse_nonsecure_call));

#endif /* _VENEER_TABLE_ */
