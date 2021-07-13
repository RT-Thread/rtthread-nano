/*
 * Copyright (c) 2020-2020, YuZhaorong 
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-06     YuZhaorong      
 */
.section .text
.global _start
.global _pvstart
.global riscv_timer
.global riscv_maskirq
.global riscv_getirq
#include "custom_ops.S"
.section .text.entry
_pvstart: 
/* zero-initialize all registers */
    addi x1, zero, 0
    addi x2, zero, 0
    addi x3, zero, 0
    addi x4, zero, 0
    addi x5, zero, 0
    addi x6, zero, 0
    addi x7, zero, 0
    addi x8, zero, 0
    addi x9, zero, 0
    addi x10, zero, 0
    addi x11, zero, 0
    addi x12, zero, 0
    addi x13, zero, 0
    addi x14, zero, 0
    addi x15, zero, 0
    addi x16, zero, 0
    addi x17, zero, 0
    addi x18, zero, 0
    addi x19, zero, 0
    addi x20, zero, 0
    addi x21, zero, 0
    addi x22, zero, 0
    addi x23, zero, 0
    addi x24, zero, 0
    addi x25, zero, 0
    addi x26, zero, 0
    addi x27, zero, 0
    addi x28, zero, 0
    addi x29, zero, 0
    addi x30, zero, 0
    addi x31, zero, 0

/* set stack pointer */

    lui sp, %hi(_riscv_sp)
    addi sp, sp, %lo(_riscv_sp)

/* (stack is aligned to 16 bytes in riscv calling convention) */
    addi sp,sp,-16
    sw zero,0(sp)
    sw zero,4(sp)
    sw zero,8(sp)
    sw zero,12(sp)
//	picorv32_waitirq_insn(zero)
    picorv32_maskirq_insn(zero, zero)
    j _start
    ebreak


_start:
# Initialize global pointer
1:  auipc gp, %pcrel_hi(__global_pointer$)
    addi  gp, gp, %pcrel_lo(1b)
# Clear the bss segment
    la      a0, _edata
    la      a1, _end
_bss_init: 
    addi	a0,a0,4
    sw	    zero,-4(a0)
    bgeu	a1,a0,_bss_init
  
# Init the data segment 
    la      a0, _data
    la      a1, _edata
    la      a2, _data_lma  
_data_init: 
    addi	a2,a2,4
    lw	    a5,-4(a2)
    addi	a0,a0,4
    sw	    a5,-4(a0)
    bgeu	a1,a0,_data_init
# call entry  
    li      a0, 0             
    call    entry
    ebreak

riscv_maskirq:
	picorv32_maskirq_insn(a0, a0)
	ret
riscv_timer:
	picorv32_timer_insn(a0, a0)
	ret
riscv_getirq:
	picorv32_getq_insn(a0, q1)
	ret

