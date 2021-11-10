phony := all
all:

include config.mk

TARGET := rtthread.elf
include src.mk

$(if $(strip $(RTT_ROOT)),,$(error RTT_ROOT not defined))

include $(BSP_ROOT)/rtthread.mk
