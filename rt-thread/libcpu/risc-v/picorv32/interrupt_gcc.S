/*
 * Copyright (c) 2020-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-06     YuZhaorong   add PicoRV32    
 */
 

#include "custom_ops.S"
.global irq
.global irq_entry 
#define REGBYTES 4
.section .text.interrupt
.balign 16
irq_entry:
	/* save registers */
    addi sp, sp, -32 * REGBYTES
    
	picorv32_setq_insn(q2, x1)
	
	picorv32_getq_insn(x1, q0)
	
	sw x1,   0*REGBYTES(sp)  // 保存 epc 指针
	
	picorv32_getq_insn(x1, q2)  // 保存 return address 
	sw x1,   1 * REGBYTES(sp)
	
	sw x5,   5 * REGBYTES(sp)
	sw x6,   6 * REGBYTES(sp)
	sw x7,   7 * REGBYTES(sp)
	sw x8,   8 * REGBYTES(sp)
	sw x9,   9 * REGBYTES(sp)
	sw x10, 10 * REGBYTES(sp)
	sw x11, 11 * REGBYTES(sp)
	sw x12, 12 * REGBYTES(sp)
	sw x13, 13 * REGBYTES(sp)
	sw x14, 14 * REGBYTES(sp)
	sw x15, 15 * REGBYTES(sp)
	sw x16, 16 * REGBYTES(sp)
	sw x17, 17 * REGBYTES(sp)
	sw x18, 18 * REGBYTES(sp)
	sw x19, 19 * REGBYTES(sp)
	sw x20, 20 * REGBYTES(sp)
	sw x21, 21 * REGBYTES(sp)
	sw x22, 22 * REGBYTES(sp)
	sw x23, 23 * REGBYTES(sp)
	sw x24, 24 * REGBYTES(sp)
	sw x25, 25 * REGBYTES(sp)
	sw x26, 26 * REGBYTES(sp)
	sw x27, 27 * REGBYTES(sp)
	sw x28, 28 * REGBYTES(sp)
	sw x29, 29 * REGBYTES(sp)
	sw x30, 30 * REGBYTES(sp)
	sw x31, 31 * REGBYTES(sp)
	
    picorv32_setq_insn(q3, x2) // 保存sp 至q3 寄存器

	
    /* switch to interrupt stack */
    la sp , irq_stack // 加载 irq 堆栈

    /* interrupt handle */
    call  rt_interrupt_enter
    	/* call interrupt handler C function */
    picorv32_getq_insn(a1, q1)
	// call to C function
	jal ra, irq
    
    call  rt_interrupt_leave

    /* switch to from thread stack */
    picorv32_getq_insn(sp, q3)
    /* need to switch new thread */
    la    s0, rt_thread_switch_interrupt_flag
    lw    s2, 0(s0)
    beqz  s2, rt_hw_context_switch_interrupt_exit
    /* clear switch interrupt flag */
    sw    zero, 0(s0)


    la    s0, rt_interrupt_from_thread
    lw  s1, 0(s0)
    sw sp, 0(s1)

    la    s0, rt_interrupt_to_thread
    lw  s1, 0(s0)
    lw  sp, 0(s1)

    lw  a0,  0 * REGBYTES(sp)
    picorv32_setq_insn(q0, a0)

/* restore registers */
rt_hw_context_switch_interrupt_exit:


	lw x1,   0 * REGBYTES(sp)
	picorv32_setq_insn(q0, x1)

	lw x1,   1 * REGBYTES(sp)
	picorv32_setq_insn(q2, x1)

	lw x5,   5 * REGBYTES(sp)
	lw x6,   6 * REGBYTES(sp)
	lw x7,   7 * REGBYTES(sp)
	lw x8,   8 * REGBYTES(sp)
	lw x9,   9 * REGBYTES(sp)
	lw x10, 10 * REGBYTES(sp)
	lw x11, 11 * REGBYTES(sp)
	lw x12, 12 * REGBYTES(sp)
	lw x13, 13 * REGBYTES(sp)
	lw x14, 14 * REGBYTES(sp)
	lw x15, 15 * REGBYTES(sp)
	lw x16, 16 * REGBYTES(sp)
	lw x17, 17 * REGBYTES(sp)
	lw x18, 18 * REGBYTES(sp)
	lw x19, 19 * REGBYTES(sp)
	lw x20, 20 * REGBYTES(sp)
	lw x21, 21 * REGBYTES(sp)
	lw x22, 22 * REGBYTES(sp)
	lw x23, 23 * REGBYTES(sp)
	lw x24, 24 * REGBYTES(sp)
	lw x25, 25 * REGBYTES(sp)
	lw x26, 26 * REGBYTES(sp)
	lw x27, 27 * REGBYTES(sp)
	lw x28, 28 * REGBYTES(sp)
	lw x29, 29 * REGBYTES(sp)
	lw x30, 30 * REGBYTES(sp)
	lw x31, 31 * REGBYTES(sp)
	
	picorv32_getq_insn(x1, q2)

    addi  sp, sp, 32 * REGBYTES

	picorv32_retirq_insn()
	
.section .data	
irq_regs:
	// registers are saved to this memory region during interrupt handling
	// the program counter is saved as register 0
	.fill 32,4

	// stack for the interrupt handler
	.fill 128,4
irq_stack:

