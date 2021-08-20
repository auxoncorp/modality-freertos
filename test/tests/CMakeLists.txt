cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

macro(add_c_test NAME COND)
    if(${COND})
        add_executable(
            ${NAME}
            ${NAME}/main.c
            ../../source/modality_probe_trace.c
            common/mock_freertos.c
            common/util.c)
        target_compile_options(
            ${NAME}
            PRIVATE
            -Wall -Wextra -Werror
            -Wsign-conversion -Wno-unused-parameter -Wpedantic
            -Wshadow -Wmissing-include-dirs -Wstrict-prototypes)
        target_include_directories(
            ${NAME}
            PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${NAME}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../source/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mock_io>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS-Kernel/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix>)
        target_link_libraries(
            ${NAME}
            PRIVATE
            ModalityProbe::LibModalityProbe)
        target_compile_definitions(
            ${NAME}
            PRIVATE
            TEST_NAME="${NAME}"
            MODALITY_PROBE_TRACE_TESTS=1
            projCOVERAGE_TEST=0)
        add_test(NAME ${NAME} COMMAND ${NAME})
    endif()
endmacro()

add_c_test(queueing ON)
add_c_test(tcb_interactions ON)
add_c_test(disabled_noop ON)
add_c_test(task_filtering ON)
add_c_test(no_io_task ON)
add_c_test(no_control_plane_buffers ON)
add_c_test(static_alloc ON)
