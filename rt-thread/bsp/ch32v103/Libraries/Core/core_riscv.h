/********************************** (C) COPYRIGHT  *******************************
* File Name          : core_riscv.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : RISC-V Core Peripheral Access Layer Header File
*******************************************************************************/
#ifndef __CORE_RISCV_H__
#define __CORE_RISCV_H__

/* IO definitions */
#ifdef __cplusplus
  #define     __I     volatile                /*!< defines 'read only' permissions      */
#else
  #define     __I     volatile const          /*!< defines 'read only' permissions     */
#endif
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */

/* Standard Peripheral Library old types (maintained for legacy purpose) */
typedef __I uint32_t vuc32;  /* Read Only */
typedef __I uint16_t vuc16;  /* Read Only */
typedef __I uint8_t vuc8;   /* Read Only */

typedef const uint32_t uc32;  /* Read Only */
typedef const uint16_t uc16;  /* Read Only */
typedef const uint8_t uc8;   /* Read Only */

typedef __I int32_t vsc32;  /* Read Only */
typedef __I int16_t vsc16;  /* Read Only */
typedef __I int8_t vsc8;   /* Read Only */

typedef const int32_t sc32;  /* Read Only */
typedef const int16_t sc16;  /* Read Only */
typedef const int8_t sc8;   /* Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

#define   RV_STATIC_INLINE  static  inline

/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct{
  __I  uint32_t ISR[8];
  __I  uint32_t IPR[8];
  __IO uint32_t ITHRESDR;
  __IO uint32_t FIBADDRR;
  __IO uint32_t CFGR;
  __I  uint32_t GISR;
  uint8_t RESERVED0[0x10];
  __IO uint32_t FIOFADDRR[4];
  uint8_t RESERVED1[0x90];
  __O  uint32_t IENR[8];
  uint8_t RESERVED2[0x60];
  __O  uint32_t IRER[8];
  uint8_t RESERVED3[0x60];
  __O  uint32_t IPSR[8];
  uint8_t RESERVED4[0x60];
  __O  uint32_t IPRR[8];
  uint8_t RESERVED5[0x60];
  __IO uint32_t IACTR[8];
  uint8_t RESERVED6[0xE0];
  __IO uint8_t IPRIOR[256];
  uint8_t RESERVED7[0x810];
  __IO uint32_t SCTLR;
}PFIC_Type;

/* memory mapped structure for SysTick */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint8_t CNTL0;
  __IO uint8_t CNTL1;
  __IO uint8_t CNTL2;
  __IO uint8_t CNTL3;
  __IO uint8_t CNTH0;
  __IO uint8_t CNTH1;
  __IO uint8_t CNTH2;
  __IO uint8_t CNTH3;
  __IO uint8_t CMPLR0;
  __IO uint8_t CMPLR1;
  __IO uint8_t CMPLR2;
  __IO uint8_t CMPLR3;
  __IO uint8_t CMPHR0;
  __IO uint8_t CMPHR1;
  __IO uint8_t CMPHR2;
  __IO uint8_t CMPHR3;
}SysTick_Type;


#define PFIC            ((PFIC_Type *) 0xE000E000 )
#define NVIC            PFIC
#define NVIC_KEY1       ((uint32_t)0xFA050000)
#define	NVIC_KEY2				((uint32_t)0xBCAF0000)
#define	NVIC_KEY3				((uint32_t)0xBEEF0000)

#define SysTick         ((SysTick_Type *) 0xE000F000)

/* ##########################   NVIC functions  #################################### */

/*******************************************************************************
* Function Name  : NVIC_EnableIRQ
* Description    : Enable Interrupt
* Input          : IRQn: Interrupt Numbers
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn){
  NVIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*******************************************************************************
* Function Name  : NVIC_DisableIRQ
* Description    : Disable Interrupt
* Input          : IRQn: Interrupt Numbers
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn)
{
  NVIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*******************************************************************************
* Function Name  : NVIC_GetStatusIRQ
* Description    : Get Interrupt Enable State
* Input          : IRQn: Interrupt Numbers
* Return         : 1: Interrupt Enable
*                  0: Interrupt Disable
*******************************************************************************/
RV_STATIC_INLINE uint32_t NVIC_GetStatusIRQ(IRQn_Type IRQn)
{
  return((uint32_t) ((NVIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

/*******************************************************************************
* Function Name  : NVIC_GetPendingIRQ
* Description    : Get Interrupt Pending State
* Input          : IRQn: Interrupt Numbers
* Return         : 1: Interrupt Pending Enable
*                  0: Interrupt Pending Disable
*******************************************************************************/
RV_STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  return((uint32_t) ((NVIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

/*******************************************************************************
* Function Name  : NVIC_SetPendingIRQ
* Description    : Set Interrupt Pending
* Input          : IRQn: Interrupt Numbers
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  NVIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*******************************************************************************
* Function Name  : NVIC_ClearPendingIRQ
* Description    : Clear Interrupt Pending
* Input          : IRQn: Interrupt Numbers
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  NVIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*******************************************************************************
* Function Name  : NVIC_GetActive
* Description    : Get Interrupt Active State
* Input          : IRQn: Interrupt Numbers
* Return         : 1: Interrupt Active
*                  0: Interrupt No Active
*******************************************************************************/
RV_STATIC_INLINE uint32_t NVIC_GetActive(IRQn_Type IRQn)
{
  return((uint32_t)((NVIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

/*******************************************************************************
* Function Name  : NVIC_SetPriority
* Description    : Set Interrupt Priority
* Input          : IRQn: Interrupt Numbers
*                  priority: bit7:pre-emption priority
*                            bit6-bit4: subpriority
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority)
{
  NVIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

/*******************************************************************************
* Function Name  : __WFI
* Description    : Wait for Interrupt
* Input          : None
* Return         : None
*******************************************************************************/
__attribute__( ( always_inline ) ) RV_STATIC_INLINE void __WFI(void)
{
  NVIC->SCTLR &= ~(1<<3);	// wfi
  asm volatile ("wfi");
}

/*******************************************************************************
* Function Name  : __WFE
* Description    : Wait for Events
* Input          : None
* Return         : None
*******************************************************************************/
__attribute__( ( always_inline ) ) RV_STATIC_INLINE void __WFE(void)
{
  NVIC->SCTLR |= (1<<3)|(1<<5);		// (wfi->wfe)+(__sev)
  asm volatile ("wfi");
  NVIC->SCTLR |= (1<<3);
  asm volatile ("wfi");
}

/*******************************************************************************
* Function Name  : NVIC_SetFastIRQ
* Description    : Set Fast Interrupt
* Input          : addr��Fast interrupt service function base address.
*                  IRQn��Interrupt Numbers
*                  num��Fast Interrupt Numbers
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_SetFastIRQ(uint32_t addr, IRQn_Type IRQn, uint8_t num){
  if(num > 3)  return ;
  NVIC->FIBADDRR = addr;
  NVIC->FIOFADDRR[num] = ((uint32_t)IRQn<<24)|(addr&0xfffff);
}

/*******************************************************************************
* Function Name  : NVIC_SystemReset
* Description    : Initiate a system reset request
* Input          : None
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_SystemReset(void)
{
  NVIC->CFGR = NVIC_KEY3|(1<<7);
}

/*******************************************************************************
* Function Name  : NVIC_HaltPushCfg
* Description    : Enable Hardware Stack
* Input          : NewState: DISABLE or ENABLE
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_HaltPushCfg(FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
  	NVIC->CFGR = NVIC_KEY1;
  }
  else{
  	NVIC->CFGR = NVIC_KEY1|(1<<0);
  }
}

/*******************************************************************************
* Function Name  : NVIC_INTNestCfg
* Description    : Enable Interrupt Nesting
* Input          : NewState: DISABLE or ENABLE
* Return         : None
*******************************************************************************/
RV_STATIC_INLINE void NVIC_INTNestCfg(FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
  	NVIC->CFGR = NVIC_KEY1;
  }
  else
  {
  	NVIC->CFGR = NVIC_KEY1|(1<<1);
  }
}

#endif/* __CORE_RISCV_H__ */





