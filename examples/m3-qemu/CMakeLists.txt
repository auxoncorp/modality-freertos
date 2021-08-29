cmake_minimum_required(VERSION 3.5)

project(m3-qemu LANGUAGES C)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_COMPILER_WORKS TRUE CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS TRUE CACHE INTERNAL "")
set(CMAKE_C_ABI_COMPILED TRUE CACHE INTERNAL "")
set(CMAKE_CXX_ABI_COMPILED TRUE CACHE INTERNAL "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -Og")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(BUILD_SHARED_LIBS OFF)

set(
    MODALITY_PROBE_ROOT
    "/usr/local/modality"
    CACHE
    PATH
    "Modality tar package installation directory")
set(
    MODALITY_PROBE_TARGET_TRIPLE
    "thumbv7em-none-eabi"
    CACHE
    STRING
    "Target triple")

list(APPEND CMAKE_MODULE_PATH "${MODALITY_PROBE_ROOT}/cmake")

find_package(ModalityProbe REQUIRED)

set(
    FREERTOS_TAG
    "202107.00"
    CACHE
    STRING
    "Sets the FreeRTOS git tag to use")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/FreeRTOS")
    message(STATUS "Cloning FreeRTOS ${FREERTOS_TAG}")
    execute_process(
        COMMAND git clone --depth 1 --recurse-submodules -j 4 --branch "${FREERTOS_TAG}" https://github.com/FreeRTOS/FreeRTOS.git "${CMAKE_SOURCE_DIR}/FreeRTOS"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_CLONE_RESULT
        OUTPUT_VARIABLE GIT_CLONE_ERROR
        ERROR_VARIABLE GIT_CLONE_ERROR)
    if(GIT_CLONE_RESULT AND NOT GIT_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "Cloning FreeRTOS ${FREERTOS_TAG} failed:\n${GIT_CLONE_ERROR}")
    endif()
endif()

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 99)
endif()

execute_process(
    COMMAND ${CMAKE_C_COMPILER} -mthumb -march=armv7-m -print-file-name=libc_nano.a
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE LIBC_RESULT
    OUTPUT_VARIABLE LIBC
    ERROR_VARIABLE LIBC_ERROR
)
if(LIBC_RESULT AND NOT LIBC_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to determine toolchain's libc:\n${LIBC_ERROR}")
endif()
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -mthumb -march=armv7-m -print-file-name=librdimon_nano.a
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE LIBRDIMON_RESULT
    OUTPUT_VARIABLE LIBRDIMON
    ERROR_VARIABLE LIBRDIMON_ERROR
)
if(LIBRDIMON_RESULT AND NOT LIBRDIMON_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to determine toolchain's librdimon:\n${LIBRDIMON_ERROR}")
endif()

set(
    KERNEL_SOURCES
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_3.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/queue.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/tasks.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/timers.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/event_groups.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/list.c)

set(
    TCPIP_SOURCES
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_DNS.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_DHCP.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_ARP.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_TCP_WIN.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_Stream_Buffer.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_IP.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_TCP_IP.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_UDP_IP.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/FreeRTOS_Sockets.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/NetworkInterface/MPS2_AN385/NetworkInterface.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/NetworkInterface/MPS2_AN385/ether_lan9118/smsc9220_eth_drv.c
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/BufferManagement/BufferAllocation_2.c)

add_executable(
    m3-qemu
    main.c
    main_blinky.c
    startup.c
    syscall.c
    modality_probe_io.c
    ${KERNEL_SOURCES}
    ${TCPIP_SOURCES}
    ${CMAKE_SOURCE_DIR}/../../source/modality_probe_trace.c)

target_compile_definitions(
    m3-qemu
    PRIVATE
    QEMU_SOC_MPS2
    HEAP3)

target_compile_options(
    m3-qemu
    PRIVATE
    -mcpu=cortex-m3 -mthumb
    -Wall -Wextra -Werror
    -Wno-unused-parameter
    -Wshadow -Wmissing-include-dirs -Wstrict-prototypes
    -Wno-error=implicit-function-declaration
    -Wno-builtin-declaration-mismatch)

target_include_directories(
    m3-qemu
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/CMSIS
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM3
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include
    ${CMAKE_SOURCE_DIR}/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC
    ${CMAKE_SOURCE_DIR}/../../source/include)

target_link_options(
    m3-qemu
    PRIVATE
    -T ${CMAKE_SOURCE_DIR}/mps2_m3.ld
    -nostartfiles
    -specs=nano.specs
    --specs=rdimon.specs)

target_link_libraries(
    m3-qemu
    PRIVATE
    -Wl,--allow-multiple-definition
    ModalityProbe::LibModalityProbe
    ${LIBC}
    ${LIBRDIMON})

add_custom_target(simulate)
add_custom_command(
    TARGET simulate
    POST_BUILD
    MAIN_DEPENDENCY m3-qemu
    COMMAND
    qemu-system-arm
    -machine mps2-an385 -cpu cortex-m3 -m 16M
    -monitor null -semihosting --semihosting-config enable=on,target=native -serial stdio
    -netdev tap,id=net0,ifname=qemu-net,script=no
    -net nic,macaddr=52:54:00:12:34:AD,model=lan9118,netdev=net0
    -nographic
    -kernel m3-qemu
    COMMENT "Starting QEMU")
