BSP_ROOT ?= $(shell pwd)
RTT_ROOT ?= $(shell pwd)/../../

CROSS_COMPILE ?=/tmp/wch_tools_red/wch_GCC/bin/riscv-none-embed-

CFLAGS := -march=rv32imac -mabi=ilp32 -DUSE_PLIC -DUSE_M_TIME -DNO_INIT -mcmodel=medany -msmall-data-limit=8 -L. -nostartfiles -Dgcc -O0 -ggdb
AFLAGS := -c -march=rv32imac -mabi=ilp32 -DUSE_PLIC -DUSE_M_TIME -DNO_INIT -mcmodel=medany -msmall-data-limit=8 -L. -nostartfiles -x assembler-with-cpp -ggdb
LFLAGS := -march=rv32imac -mabi=ilp32 -DUSE_PLIC -DUSE_M_TIME -DNO_INIT -mcmodel=medany -msmall-data-limit=8 -L. -nostartfiles -Wl,--gc-sections,-Map=rt-thread.map,-cref,-u, -T board/linker_scripts/link.lds
CXXFLAGS := -march=rv32imac -mabi=ilp32 -DUSE_PLIC -DUSE_M_TIME -DNO_INIT -mcmodel=medany -msmall-data-limit=8 -L. -nostartfiles -Dgcc -O0 -ggdb
EXTERN_LIB := -lm -lc 

CPPPATHS :=-I$(RTT_ROOT)/bsp/ch32v103/Libraries/Core \
	-I$(RTT_ROOT)/bsp/ch32v103/Libraries/Peripheral \
	-I$(RTT_ROOT)/bsp/ch32v103/Libraries/ch32_drivers \
	-I$(BSP_ROOT) \
	-I$(BSP_ROOT)/applications \
	-I$(BSP_ROOT)/board \
	-I$(RTT_ROOT)/components/finsh \
	-I$(RTT_ROOT)/include \
	-I$(RTT_ROOT)/libcpu/risc-v/ch32v \
	-I$(RTT_ROOT)/libcpu/risc-v/common 

DEFINES := -DHAVE_CCONFIG_H -DRT_USING_NEWLIB -D__RTTHREAD__
