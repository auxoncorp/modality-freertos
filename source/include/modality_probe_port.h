#ifndef MODALITY_PROBE_PORT_H
#define MODALITY_PROBE_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"

#if !defined(MPT_CFG_ALLOC_CRITICAL_SECTION) || !defined(MPT_CFG_ENTER_CRITICAL_SECTION) || !defined(MPT_CFG_EXIT_CRITICAL_SECTION)
    #error "Missing critical section definition. Define MPT_CFG_ALLOC_CRITICAL_SECTION, MPT_CFG_ENTER_CRITICAL_SECTION, and MPT_CFG_EXIT_CRITICAL_SECTION in modality_probe_config.h for your hardware."
#else
    #define TRACE_ALLOC_CRITICAL_SECTION() MPT_CFG_ALLOC_CRITICAL_SECTION()
    #define TRACE_ENTER_CRITICAL_SECTION() MPT_CFG_ENTER_CRITICAL_SECTION()
    #define TRACE_EXIT_CRITICAL_SECTION() MPT_CFG_EXIT_CRITICAL_SECTION()
#endif

#ifndef STRLEN
#define STRLEN(a) strlen((a))
#endif

#ifndef MPT_CAT2
#define MPT_CAT2(a, b) a##b
#endif

#ifndef TRACE_MALLOC
#define TRACE_MALLOC(size) pvPortMalloc(size)
#endif

#ifndef TRACE_FREE
#define TRACE_FREE(ptr) vPortFree(ptr)
#endif

#define TRACE_READ_LOWER16(input) ((uint16_t)((input) & 0x0000FFFF))
#define TRACE_READ_UPPER16(input) ((uint16_t)(((input) >> 16) & 0x0000FFFF))
#define TRACE_WRITE_LOWER16(output, input)  (((output) & 0xFFFF0000) | (input))
#define TRACE_WRITE_UPPER16(output, input) (((output) & 0x0000FFFF) | (((uint32_t)(input)) << 16))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODALITY_PROBE_PORT_H */
