# CMake 最低版本号要求

cmake_minimum_required (VERSION 3.1)



ENABLE_LANGUAGE(ASM)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR risc_v)


set(tools /opt/riscv32i)
set(LINKR_FILE ../src/sections.ld)
set(CMAKE_ASM_COMPILER ${tools}/bin/riscv32-unknown-elf-gcc)
set(CMAKE_C_COMPILER ${tools}/bin/riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/riscv32-unknown-elf-g++)
set(CMAKE_OBJCOPY ${tools}/bin/riscv32-unknown-elf-objcopy)
set(CMAKE_OBJDUMP ${tools}/bin/riscv32-unknown-elf-objdump)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#CXXFLAGS = -MD -Os -Wall -std=c++11
#CCFLAGS = -MD -Os -Wall
#LDFLAGS = -Wl,--gc-sections,--no-relax
#LDFLAGS = -ffunction-sections -Wl,--gc-sections
#LDLIBS =  -ffunction-sections -fdata-sections






set(CMAKE_C_FLAGS "-MD -Os -Wall" )


set(CMAKE_ASM_FLAGS "-nostdlib" )
set(LD_FLAGS "-ffunction-sections  -nostartfiles -Wl,--gc-sections")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(ENABLE_EXPORTS  True)
# 项目信息

set(PROJECT_NAME RISC_V)

set(BIN_TARGET ${PROJECT_NAME}.bin)
set(HEX_TARGET ${PROJECT_NAME}.hex)
set(MAP_TARGET ${PROJECT_NAME}.map)
set(LSS_TARGET ${PROJECT_NAME}.asm)
set(TMP_TARGET ${PROJECT_NAME}.tmp)

set(RISCV_SRC 
   ../src/start.S
   ../board.c
   ../src/main.c
  ../../../libcpu/risc-v/picorv32/interrupt_gcc.S
  ../../../libcpu/risc-v/picorv32/interrupt.c
  ../../../libcpu/risc-v/picorv32/context_gcc.S
  ../../../libcpu/risc-v/picorv32/cpuport.c
  ../../../src/clock.c
   ../../../src/components.c
   ../../../src/cpu.c
   ../../../src/idle.c
   ../../../src/ipc.c
   ../../../src/irq.c
   ../../../src/kservice.c
   ../../../src/mem.c
   ../../../src/memheap.c
   ../../../src/mempool.c
   ../../../src/object.c
   ../../../src/scheduler.c
   ../../../src/slab.c
   ../../../src/thread.c
   ../../../src/timer.c
   )
# create binary & hex files and show size of resulting firmware image

project (${PROJECT_NAME} )

include_directories(../../../libcpu/risc-v/picorv32
                    ../
                    ../../../include/
                    ../../../include/libc/
                    ./)

# 指定生成目标  
add_executable(${PROJECT_NAME}.elf ${RISCV_SRC})

target_link_libraries(${PROJECT_NAME}.elf PRIVATE  -T${LINKR_FILE} ${LD_FLAGS},-Map=${MAP_TARGET})

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND echo "${PROJECT_NAME}.elf"
        COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${BIN_TARGET}
        COMMAND ${CMAKE_OBJCOPY} -O verilog ${PROJECT_NAME}.elf ${TMP_TARGET}
        COMMAND ${CMAKE_OBJDUMP} -S ${PROJECT_NAME}.elf > ${LSS_TARGET}
        COMMAND echo "${PROJECT_NAME}.elf"
        COMMENT "Generating ${HEX_TARGET}, ${BIN_TARGET}")


