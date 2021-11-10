/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v10x_misc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : This file provides all the miscellaneous firmware functions .
*********************************************************************************/
#include "ch32v10x_misc.h"

//__IO uint32_t NVIC_Priority_Group = NVIC_PriorityGroup_1;

/******************************************************************************************
* Function Name  : NVIC_PriorityGroupConfig
* Description    : Configures the priority grouping: pre-emption priority and subpriority. 
* Input          : 0为关闭中断嵌套，1为开启中断嵌套
* Return         : None
*******************************************************************************************/
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
    uint32_t temp = PFIC->CFGR;

    if(1 == NVIC_PriorityGroup)                 //开启中断嵌套
    {
        temp &= 0x0000FFFD;                     //0打开
        temp |= (0xFA05 << 16);                 //0打开,同步写入KEY1。
        PFIC->CFGR =temp;
    }
    else if(0 == NVIC_PriorityGroup)            //关闭中断嵌套
    {
        PFIC->CFGR |= (0xFA05 << 16) | 1<<1;    //1关闭,同步写入KEY1。
    }
//	NVIC_Priority_Group = NVIC_PriorityGroup;
}

/******************************************************************************************
* Function Name  : NVIC_Init
* Description    : Initializes the NVIC peripheral according to the specified parameters in 
*                  the NVIC_InitStruct. 
* Input          : NVIC_InitStruct: pointer to a NVIC_InitTypeDef structure that contains the
*                                   configuration information for the specified NVIC peripheral.                 
* Return         : None
*******************************************************************************************/
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)
{
	//uint8_t tmppre = 0;

	if((PFIC->CFGR&(1<<1)) != 0)                    //中断嵌套标志位：1为关闭
	{
		NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, NVIC_InitStruct->NVIC_IRQChannelSubPriority<<4);
	}
	else if((PFIC->CFGR&(1<<1)) == 0)               //中断嵌套标志位：0为打开
	{
        if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority == 1)
        {
            NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, (1<<7)|(NVIC_InitStruct->NVIC_IRQChannelSubPriority<<4));
        }
        else
        {
            NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, (0<<7)|(NVIC_InitStruct->NVIC_IRQChannelSubPriority<<4));
        }
	}
//	else if(NVIC_Priority_Group == NVIC_PriorityGroup_2)
//	{
//    if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority <= 1)
//    {
//      tmppre = NVIC_InitStruct->NVIC_IRQChannelSubPriority + (4*NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority);
//    	NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, (0<<7)|(tmppre<<4));
//    }
//    else
//    {
//      tmppre = NVIC_InitStruct->NVIC_IRQChannelSubPriority + (4*(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority-2));
//    	NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, (1<<7)|(tmppre<<4));
//    }
//	}
//	else if(NVIC_Priority_Group == NVIC_PriorityGroup_3)
//	{
//    if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority <= 3)
//    {
//      tmppre = NVIC_InitStruct->NVIC_IRQChannelSubPriority + (2*NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority);
//    	NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, (0<<7)|(tmppre<<4));
//    }
//    else
//    {
//      tmppre = NVIC_InitStruct->NVIC_IRQChannelSubPriority + (2*(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority-4));
//    	NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, (1<<7)|(tmppre<<4));
//    }
//	}
//	else if(NVIC_Priority_Group == NVIC_PriorityGroup_4)
//	{
//		NVIC_SetPriority( NVIC_InitStruct->NVIC_IRQChannel, NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority<<4);
//	}

  if (NVIC_InitStruct->NVIC_IRQChannelCmd != DISABLE)
  {
  	NVIC_EnableIRQ(NVIC_InitStruct->NVIC_IRQChannel);
  }
  else
  {
  	NVIC_DisableIRQ(NVIC_InitStruct->NVIC_IRQChannel);
  }
}



