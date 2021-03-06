#ifndef MODALITY_PROBE_TRACE_H
#define MODALITY_PROBE_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "modality_probe_config.h"
#include "modality_probe_port.h"
#include "modality_probe_freertos.h"

#if (MPT_TRACING_ENABLED == 0)
#define MODALITY_PROBE_MACROS_ENABLED 0
#define MODALITY_MUTATOR_MACROS_ENABLED 0
#endif

#include <modality/probe.h>
#include "modality_probe_macros.h"

#define MPT_UNUSED
#define MPT_NOOP_STATEMENT do {} while( 0 )

typedef void (*TraceProbeIteratorFunction_t)(modality_probe* probe);

#if (MPT_TRACING_ENABLED == 1)

#if defined(MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG) && (MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG == 1)
#if !defined(configUSE_APPLICATION_TASK_TAG) || (configUSE_APPLICATION_TASK_TAG == 0)
#error "When using MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG, add configUSE_APPLICATION_TASK_TAG = 1 in FreeRTOSConfig.h"
#endif /* !defined(configUSE_APPLICATION_TASK_TAG) || (configUSE_APPLICATION_TASK_TAG == 0) */
#endif /* defined(MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG) && (MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG == 1) */

#if defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1)
#include "modality_probe_io.h"

extern const char TRACE_IO_TASK_NAME[];

#ifndef TRACE_IO_INIT
#define TRACE_IO_INIT() MPT_NOOP_STATEMENT
#endif

#ifndef TRACE_IO_READ
#error "TRACE_IO_READ not defined, it is required to be defined in modality_probe_io.h when using the IO task"
#endif

#ifndef TRACE_IO_WRITE
#error "TRACE_IO_WRITE not defined, it is required to be defined in modality_probe_io.h when using the IO task"
#endif

#if !defined(INCLUDE_vTaskDelay) || (INCLUDE_vTaskDelay == 0)
#error "When including the IO task (MPT_CFG_INCLUDE_IO_TASK == 1), add INCLUDE_vTaskDelay = 1 to FreeRTOSConfig.h"
#endif

#endif /* defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1) */

#if !defined(INCLUDE_xTaskGetCurrentTaskHandle) || (INCLUDE_xTaskGetCurrentTaskHandle == 0)
#error "Add INCLUDE_xTaskGetCurrentTaskHandle = 1 to FreeRTOSConfig.h"
#endif

#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
#if !defined(configSUPPORT_DYNAMIC_ALLOCATION) || (configSUPPORT_DYNAMIC_ALLOCATION == 0)
#error "When using control plane buffers, add configSUPPORT_DYNAMIC_ALLOCATION = 1 in FreeRTOSConfig.h"
#endif /* !defined(configSUPPORT_DYNAMIC_ALLOCATION) || (configSUPPORT_DYNAMIC_ALLOCATION == 0) */
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */

/*
 * Enable tracing.
 */
void vTraceEnable(void);

/*
 * Disable tracing.
 */
void vTraceDisable(void);

/*
 * Returns 0 is tracing is disabled, otherwise returns 1.
 */
int xTraceIsEnabled(void);

/*
 * Retrieve the error string pointer or NULL.
 */
const char* pcTraceGetError(void);

/*
 * Exclude a task by name from being traced.
 * Must be called prior to creating the task.
 */
void vTraceExcludeTask(const char* pcName);

/*
 * Calls the iterator function for each probe in the global registry.
 */
void vTraceProbeIterator(TraceProbeIteratorFunction_t pvIteratorFunction);

#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
/* Returns the MessageBufferHandle_t associated with the current task, if any */
void* pvTraceGetControlPlaneBuffer(void);

/*
 * Process a control plane message from the task's message queue, if one exists.
 */
void vTraceProcessControlPlaneMessages(void);
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */

#if defined(MPT_CFG_USE_TRACE_ASSERT) && (MPT_CFG_USE_TRACE_ASSERT != 0)
/* NOTE: When enabled, failing TRACE_ASSERT's may leave interrupts disabled to aid in debugging */
#define TRACE_ASSERT(eval, msg, ret) \
if (!(eval)) \
{ \
    prvTraceError("TRACE_ASSERT: " msg); \
    return ret; \
}
#else
#define TRACE_ASSERT(eval, msg, ret) MPT_NOOP_STATEMENT
#endif /* defined(MPT_CFG_USE_TRACE_ASSERT) && (MPT_CFG_USE_TRACE_ASSERT != 0) */

#ifndef TRACE_DEBUG_PRINTF
#define TRACE_DEBUG_PRINTF(x) MPT_NOOP_STATEMENT
#endif

/* Private functions */
void prvTraceError(const char* msg);
uint16_t prvTraceGetFilterMask(void);
void* prvTraceGetCurrentTaskHandle(void);
uint16_t prvTraceGetTaskProbeId(void* task);
void prvTraceSetTaskProbeId(void* task, uint16_t probe_id);
uint16_t prvTraceGetTaskFilter(void* task);
void prvTraceSetTaskFilter(void* task, uint16_t filter);
void prvTraceRegisterTaskProbe(void* task, uint32_t uxTCBNumber, uint32_t uxPriority);
void prvTraceProduceSnapshot(void);
void prvTraceMergeSnapshot(void);
void prvTraceLogEvent(uint32_t id);
void prvTraceLogEventWithPayload(uint32_t id, uint32_t payload);
uint32_t prvTraceSwitchedTCB(void* pNewTCB);

#else /* MPT_TRACING_ENABLED == 0 */

#define TRACE_ASSERT(eval, msg, ret) MPT_NOOP_STATEMENT
#ifndef TRACE_DEBUG_PRINTF
#define TRACE_DEBUG_PRINTF(x) MPT_NOOP_STATEMENT
#endif

#define TRACE_IO_TASK_NAME ""

#define vTraceEnable(x) MPT_NOOP_STATEMENT
#define vTraceDisable(x) MPT_NOOP_STATEMENT
#define xTraceIsEnabled() (0)
#define pcTraceGetError() (NULL)
#define vTraceExcludeTask(x) MPT_NOOP_STATEMENT
#define vTraceProbeIterator(x) MPT_NOOP_STATEMENT
#define pvTraceGetControlPlaneBuffer() (NULL)
#define vTraceProcessControlPlaneMessages() MPT_NOOP_STATEMENT

#define TRACE_GET_CURRENT_TASK() (NULL)
#define TRACE_GET_TASK_PROBE_ID(pxTCB) (0)
#define TRACE_GET_TASK_FILTER(pxTCB) (0)
#define TRACE_SET_TASK_FILTER(pxTCB, filter) MPT_NOOP_STATEMENT
#define TRACE_GET_FILTER_MASK() (0)

#endif /* MPT_TRACING_ENABLED == 1 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODALITY_PROBE_TRACE_H */
