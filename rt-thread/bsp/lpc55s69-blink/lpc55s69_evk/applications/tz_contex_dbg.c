/******************************************************************************
 * @file     tz_context.c
 * @brief    Context Management for Armv8-M TrustZone - Sample implementation
 * @version  V1.1.1
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2016-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stdint.h"

#define TZ_PROCESS_STACK_SLOTS     8U
#define TZ_PROCESS_STACK_SIZE      256U

typedef struct
{
  uint32_t sp_top;      // stack space top
  uint32_t sp_limit;    // stack space limit
  uint32_t sp;          // current stack pointer
}stack_info_t;

static stack_info_t ProcessStackInfo  [TZ_PROCESS_STACK_SLOTS];
static uint64_t     ProcessStackMemory[TZ_PROCESS_STACK_SLOTS][TZ_PROCESS_STACK_SIZE/8U];
static uint32_t     ProcessStackFreeSlot = 0xFFFFFFFFU;
static uint32_t     ProcessStackCurrSlot = 0xFFFFFFFFU;

uint32_t TZ_InitContextSystem_S_DBG (void)
{
    uint32_t n;

    for (n = 0U; n < TZ_PROCESS_STACK_SLOTS; n++)
    {
        ProcessStackInfo[n].sp = 0U;
        ProcessStackInfo[n].sp_limit = (uint32_t)&ProcessStackMemory[n];
        ProcessStackInfo[n].sp_top   = (uint32_t)&ProcessStackMemory[n] + TZ_PROCESS_STACK_SIZE;
        *((uint32_t *)ProcessStackMemory[n]) = n + 1U;
    }
    *((uint32_t *)ProcessStackMemory[--n]) = 0xFFFFFFFFU;

    ProcessStackFreeSlot = 0U;

  return 1U;    // Success
}

uint32_t TZ_AllocModuleContext_S_DBG (void)
{
    uint32_t slot;

    if (ProcessStackFreeSlot == 0xFFFFFFFFU)
    {
        return 0xFFFFFFFFU;  // No slot available
    }

    slot = ProcessStackFreeSlot;
    ProcessStackFreeSlot = *((uint32_t *)ProcessStackMemory[slot]);

    ProcessStackInfo[slot].sp = ProcessStackInfo[slot].sp_top;
    ProcessStackCurrSlot = slot;

    return (slot);
}

uint32_t TZ_FreeModuleContext_S_DBG (uint32_t slot)
{

  ProcessStackInfo[slot].sp = 0U;

  *((uint32_t *)ProcessStackMemory[slot]) = ProcessStackFreeSlot;
  ProcessStackFreeSlot = slot;

  ProcessStackCurrSlot = 0xFFFFFFFFU;

  return slot;    // Success
}

uint32_t TZ_LoadContext_S_DBG (uint32_t id)
{
    uint32_t slot;

    if (id >=0 && id < TZ_PROCESS_STACK_SLOTS)
    {
        //__set_PSPLIM(ProcessStackInfo[slot].sp_limit);
        //__set_PSP   (ProcessStackInfo[slot].sp);
        ProcessStackCurrSlot = id;
    }
    return id;    // Success
}

uint32_t TZ_StoreContext_S_DBG (uint32_t id)
{
    uint32_t tmp = ProcessStackCurrSlot;

    //__set_PSPLIM(ProcessStackInfo[ProcessStackFreeSlot].sp_limit);
    //__set_PSP   (ProcessStackInfo[ProcessStackFreeSlot].sp);

    ProcessStackCurrSlot = 0xFFFFFFFF;

    return tmp;
}
