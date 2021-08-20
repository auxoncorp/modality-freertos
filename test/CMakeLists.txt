cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(modality-probe-trace-tests LANGUAGES C)

find_package(ModalityProbe REQUIRED)

set(
    FREERTOS_KERNEL_VERSION
    "V10.4.4"
    CACHE
    STRING
    "Sets the FreeRTOS kernel version (git tag) to use")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel")
    execute_process(
        COMMAND git clone --depth 1 --branch "${FREERTOS_KERNEL_VERSION}" https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_CLONE_RESULT
        OUTPUT_VARIABLE GIT_CLONE_ERROR
        ERROR_VARIABLE GIT_CLONE_ERROR)
    if(GIT_CLONE_RESULT AND NOT GIT_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "Cloning FreeRTOS kernel ${FREERTOS_KERNEL_VERSION} failed:\n${GIT_CLONE_ERROR}")
    endif()
endif()

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 99)
endif()

set(ASAN ON CACHE BOOL "Use address sanitzer")

function(append value)
  foreach(variable ${ARGN})
    set(${variable}
        "${${variable}} ${value}"
        PARENT_SCOPE)
  endforeach(variable)
endfunction()

if(ASAN)
    message(STATUS "Building with ASAN")
    append("-fno-omit-frame-pointer" CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    if(uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
      append("-O1" CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    endif()
    append("-fsanitize=address" CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
endif()

enable_testing()
add_subdirectory(tests)
