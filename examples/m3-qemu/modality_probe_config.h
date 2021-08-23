#ifndef MODALITY_PROBE_CONFIG_H
#define MODALITY_PROBE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define MPT_HAS_DEBUG_PRINTF 1
#if defined(MPT_HAS_DEBUG_PRINTF) && (MPT_HAS_DEBUG_PRINTF == 1)
    extern void vLoggingPrintf( const char *pcFormatString, ... );
    #define TRACE_DEBUG_PRINTF(x) vLoggingPrintf x
#endif

#define MPT_CFG_TASK_PROBE_SIZE 1024

#define MPT_CFG_MAX_PROBES 8

#define MPT_CFG_MAX_EXCLUDED_TASKS 2

#define MPT_CFG_USE_TRACE_ASSERT 1

#define MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG 1

#define MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS 1

/*
#define MPT_CFG_INCLUDE_IO_TASK 1

#define MPT_CFG_IO_TASK_NAME "ModalityProbeIo"

#define MPT_CFG_IO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

#define MPT_CFG_IO_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

#define MPT_CFG_IO_TASK_DELAY 10

#define MPT_CFG_IO_TASK_ITERS_PER_MUTATOR_ANNOUNCEMENT 200

#define MPT_CFG_IO_TASK_BUFFER_SIZE 256

#define MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER 1

#define MPT_CFG_CONTROL_PLANE_BUFFER_SIZE 512
#define MPT_CFG_CONTROL_PLANE_TASK_BUFFER_SIZE 128
*/

#include "cmsis_gcc.h"
#define MPT_CFG_ALLOC_CRITICAL_SECTION() uint32_t __irq_status;
#define MPT_CFG_ENTER_CRITICAL_SECTION() {__irq_status = __get_PRIMASK(); __set_PRIMASK(1);} /* Disable all interrupts */
#define MPT_CFG_EXIT_CRITICAL_SECTION() {__set_PRIMASK(__irq_status);}

#ifdef __cplusplus
}
#endif

#endif /* MODALITY_PROBE_CONFIG_H */
