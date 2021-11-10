SRC_FILES :=
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/Core/core_riscv.c
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/Peripheral/ch32v10x_exti.c
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/Peripheral/ch32v10x_gpio.c
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/Peripheral/ch32v10x_misc.c
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/Peripheral/ch32v10x_rcc.c
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/Peripheral/ch32v10x_usart.c
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/Startup/startup_ch32v10x.S
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/ch32_drivers/_ch32v103_pinmux.c
SRC_FILES +=$(RTT_ROOT)/bsp/ch32v103/Libraries/ch32_drivers/drv_usart.c
SRC_FILES +=$(BSP_ROOT)/applications/main.c
SRC_FILES +=$(BSP_ROOT)/board/board.c
SRC_FILES +=$(BSP_ROOT)/board/system_ch32v10x.c
SRC_FILES +=$(RTT_ROOT)/components/device/device.c
SRC_FILES +=$(RTT_ROOT)/components/finsh/cmd.c
SRC_FILES +=$(RTT_ROOT)/components/finsh/msh.c
SRC_FILES +=$(RTT_ROOT)/components/finsh/shell.c
SRC_FILES +=$(RTT_ROOT)/libcpu/risc-v/ch32v/interrupt_gcc.S
SRC_FILES +=$(RTT_ROOT)/libcpu/risc-v/common/context_gcc.S
SRC_FILES +=$(RTT_ROOT)/libcpu/risc-v/common/cpuport.c
SRC_FILES +=$(RTT_ROOT)/src/clock.c
SRC_FILES +=$(RTT_ROOT)/src/components.c
SRC_FILES +=$(RTT_ROOT)/src/idle.c
SRC_FILES +=$(RTT_ROOT)/src/ipc.c
SRC_FILES +=$(RTT_ROOT)/src/ringbuffer.c
SRC_FILES +=$(RTT_ROOT)/src/irq.c
SRC_FILES +=$(RTT_ROOT)/src/kservice.c
SRC_FILES +=$(RTT_ROOT)/src/mem.c
SRC_FILES +=$(RTT_ROOT)/src/mempool.c
SRC_FILES +=$(RTT_ROOT)/src/object.c
SRC_FILES +=$(RTT_ROOT)/src/scheduler.c
SRC_FILES +=$(RTT_ROOT)/src/thread.c
SRC_FILES +=$(RTT_ROOT)/src/timer.c