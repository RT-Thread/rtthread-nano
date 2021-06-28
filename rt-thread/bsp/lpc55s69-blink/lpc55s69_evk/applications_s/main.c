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

/* typedef for non-secure callback functions */
typedef void (*funcptr_ns) (void) __attribute__((cmse_nonsecure_call));

funcptr_ns ResetHandler_ns;

/*******************************************************************************
 * Code
 ******************************************************************************/
void HardFault_Handler(uint32_t * hardfault_args)
{
    PRINTF("HardFault_UserHandler\r\n");
    NVIC_SystemReset();
    while(1);
}

/*!
 * @brief Application-specific implementation of the SystemInitHook() weak function.
 */
void SystemInitHook(void) 
{
/* The TrustZone should be configured as early as possible after RESET.
 * Therefore it is called from SystemInit() during startup. The SystemInitHook() weak function 
 * overloading is used for this purpose.
*/
    BOARD_InitTrustZone();
}

int main(void)
{
    SYSCON->TRACECLKDIV = 0;
/* Optionally enable RAM banks that may be off by default at reset */
    SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL0_SRAM_CTRL1_MASK | SYSCON_AHBCLKCTRL0_SRAM_CTRL2_MASK
                          | SYSCON_AHBCLKCTRL0_SRAM_CTRL3_MASK | SYSCON_AHBCLKCTRL0_SRAM_CTRL4_MASK;

    /* Init the boards */
    CLOCK_EnableClock(kCLOCK_InputMux);
    CLOCK_EnableClock(kCLOCK_Iocon);

    BOARD_BootClockFROHF96M();                                     
    /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */
    // Init by main.c 
    CLOCK_EnableClock(kCLOCK_Iocon);
    // Debug Usart Pin Init
    IOCON->PIO[0][27] = (IOCON_FUNC1  | IOCON_MODE_INACT  | IOCON_DIGITAL_EN );  // Debug Usart TXD pin, Flexcomm2
    IOCON->PIO[1][24] = (IOCON_FUNC1  | IOCON_MODE_INACT  | IOCON_DIGITAL_EN );  // Debug Usart RXD pin, Flexcomm2
    CLOCK_DisableClock(kCLOCK_Iocon);

    SCB->NSACR = 0xC00;

    debug_init();
    PRINTF("LPC55S69 EVK Hello Word from Secure Wold.\r\n");

//    NVIC_SetTargetState(PIN_INT0_IRQn);
//    NVIC_SetTargetState(PIN_INT1_IRQn);
//    NVIC_SetTargetState(PIN_INT2_IRQn);
//    NVIC_SetTargetState(PIN_INT3_IRQn);
//    NVIC_SetTargetState(PIN_INT4_IRQn);
//    NVIC_SetTargetState(PIN_INT5_IRQn);
//    NVIC_SetTargetState(PIN_INT6_IRQn);
//    NVIC_SetTargetState(PIN_INT7_IRQn);
//    
//    NVIC_SetTargetState(GINT0_IRQn);                                             /* GINT0     Interrupt can be access by NS */
//    NVIC_SetTargetState(GINT1_IRQn);                                             /* GINT0     Interrupt can be access by NS */
//    
//    NVIC_SetTargetState(MRT0_IRQn);                                              /* MRT0      Interrupt can be access by NS */

//    NVIC_SetTargetState(FLEXCOMM0_IRQn);
//    NVIC_SetTargetState(FLEXCOMM1_IRQn);                                         /* Flexcomm2 Interrupt can be access by NS */
//    NVIC_SetTargetState(FLEXCOMM3_IRQn);                                         /* Flexcomm3 Interrupt can be access by NS */
//    NVIC_SetTargetState(FLEXCOMM4_IRQn);                                         /* Flexcomm4 Interrupt can be access by NS */
//    NVIC_SetTargetState(FLEXCOMM5_IRQn);                                         /* Flexcomm4 Interrupt can be access by NS */
//    NVIC_SetTargetState(FLEXCOMM6_IRQn);                                         /* Flexcomm6 Interrupt can be access by NS */
//    NVIC_SetTargetState(FLEXCOMM7_IRQn);                                         /* Flexcomm7 Interrupt can be access by NS */
//    NVIC_SetTargetState(DMA0_IRQn);
//    
    
    __TZ_set_MSP_NS(*((uint32_t *)(UNSECURE_IMAGEADDR)));                        /* Set non-secure main stack (MSP_NS) */
    SCB_NS->VTOR = UNSECURE_IMAGEADDR;                                           /* Set non-secure vector table */
    ResetHandler_ns = (funcptr_ns)(*((uint32_t *)((UNSECURE_IMAGEADDR) + 4U)));  /* Get non-secure reset handler */
    PRINTF("Entering normal world.\r\n");                                        /* Call non-secure application */
    ResetHandler_ns();                                                           /* Jump to normal world */

    while(1)
    {
    }
}

// end file
