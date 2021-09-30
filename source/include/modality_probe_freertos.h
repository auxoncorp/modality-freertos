#ifndef MODALITY_PROBE_FREERTOS_H
#define MODALITY_PROBE_FREERTOS_H

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MPT_TRACING_ENABLED configUSE_TRACE_FACILITY

/*
 * Tracing event IDs
 *
 * Naming convention is TRACE_EVENT_<CLASS>_<FUNCTION>.
 * The manifest-gen script generates event entries named <CLASS>_<FUNCTION>, e.g.
 * the event TRACE_EVENT_TASK_CREATE will be in the manifest as TASK_CREATE.
 * A <class> tag will also be added to the event's metadata.
 *
 */
#define TRACE_EVENT_ID_TOP (0xFFFF)

#define TRACE_EVENT_TASK_CREATE (TRACE_EVENT_ID_TOP - 1)
#define TRACE_EVENT_TASK_SWITCHED_IN (TRACE_EVENT_ID_TOP - 2)
#define TRACE_EVENT_TASK_SWITCHED_OUT (TRACE_EVENT_ID_TOP - 3)
#define TRACE_EVENT_TASK_SUSPEND (TRACE_EVENT_ID_TOP - 4)
#define TRACE_EVENT_TASK_DELAY (TRACE_EVENT_ID_TOP - 5)
#define TRACE_EVENT_TASK_DELAY_UNTIL (TRACE_EVENT_ID_TOP - 6)

#define TRACE_EVENT_QUEUE_SEND (TRACE_EVENT_ID_TOP - 100)
#define TRACE_EVENT_QUEUE_SEND_BLOCKED (TRACE_EVENT_ID_TOP - 101)
#define TRACE_EVENT_QUEUE_SEND_FAILED (TRACE_EVENT_ID_TOP - 102)
#define TRACE_EVENT_QUEUE_SEND_FRONT (TRACE_EVENT_ID_TOP - 103)
#define TRACE_EVENT_QUEUE_SEND_FRONT_BLOCKED (TRACE_EVENT_ID_TOP - 104)
#define TRACE_EVENT_QUEUE_SEND_FRONT_FAILED (TRACE_EVENT_ID_TOP - 105)
#define TRACE_EVENT_QUEUE_SEND_FROMISR (TRACE_EVENT_ID_TOP - 106)
#define TRACE_EVENT_QUEUE_SEND_FROMISR_FAILED (TRACE_EVENT_ID_TOP - 107)
#define TRACE_EVENT_QUEUE_SEND_FRONT_FROMISR (TRACE_EVENT_ID_TOP - 108)
#define TRACE_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED (TRACE_EVENT_ID_TOP - 109)

#define TRACE_EVENT_QUEUE_RECEIVE (TRACE_EVENT_ID_TOP - 200)
#define TRACE_EVENT_QUEUE_RECEIVE_BLOCKED (TRACE_EVENT_ID_TOP - 201)
#define TRACE_EVENT_QUEUE_RECEIVE_FAILED (TRACE_EVENT_ID_TOP - 202)
#define TRACE_EVENT_QUEUE_RECEIVE_FROMISR (TRACE_EVENT_ID_TOP - 203)
#define TRACE_EVENT_QUEUE_RECEIVE_FROMISR_FAILED (TRACE_EVENT_ID_TOP - 204)

#define TRACE_EVENT_SEMAPHORE_GIVE (TRACE_EVENT_ID_TOP - 300)
#define TRACE_EVENT_SEMAPHORE_GIVE_BLOCKED (TRACE_EVENT_ID_TOP - 301)
#define TRACE_EVENT_SEMAPHORE_GIVE_FAILED (TRACE_EVENT_ID_TOP - 302)
#define TRACE_EVENT_SEMAPHORE_GIVE_FROMISR (TRACE_EVENT_ID_TOP - 303)
#define TRACE_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED (TRACE_EVENT_ID_TOP - 304)

#define TRACE_EVENT_SEMAPHORE_TAKE (TRACE_EVENT_ID_TOP - 400)
#define TRACE_EVENT_SEMAPHORE_TAKE_BLOCKED (TRACE_EVENT_ID_TOP - 401)
#define TRACE_EVENT_SEMAPHORE_TAKE_FAILED (TRACE_EVENT_ID_TOP - 402)
#define TRACE_EVENT_SEMAPHORE_TAKE_FROMISR (TRACE_EVENT_ID_TOP - 403)
#define TRACE_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED (TRACE_EVENT_ID_TOP - 404)

#define TRACE_EVENT_MUTEX_GIVE (TRACE_EVENT_ID_TOP - 500)
#define TRACE_EVENT_MUTEX_GIVE_BLOCKED (TRACE_EVENT_ID_TOP - 501)
#define TRACE_EVENT_MUTEX_GIVE_FAILED (TRACE_EVENT_ID_TOP - 502)

#define TRACE_EVENT_MUTEX_TAKE (TRACE_EVENT_ID_TOP - 600)
#define TRACE_EVENT_MUTEX_TAKE_BLOCKED (TRACE_EVENT_ID_TOP - 601)
#define TRACE_EVENT_MUTEX_TAKE_FAILED (TRACE_EVENT_ID_TOP - 602)

#if defined(MPT_TRACING_ENABLED) && (MPT_TRACING_ENABLED == 1)

#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()
#define TRACE_GET_TASK_PROBE_ID(pxTCB) prvTraceGetTaskProbeId(pxTCB)
#define TRACE_GET_TASK_FILTER(pxTCB) prvTraceGetTaskFilter(pxTCB)
#define TRACE_SET_TASK_FILTER(pxTCB, filter) prvTraceSetTaskFilter(pxTCB, filter)
#define TRACE_GET_FILTER_MASK() prvTraceGetFilterMask()

/* Fix to add xCopyPosition parameter to xQueueGiveFromISR */
#define xQueueGiveFromISR(a,b) MPT_CAT2(xQueueGiveFromISR__, uxQueueType) (a,b)
#define xQueueGiveFromISR__pcHead(__a, __b) MptWrapper_xQueueGiveFromISR(__a, __b, const BaseType_t xCopyPosition); \
BaseType_t xQueueGiveFromISR(__a, __b) { return MptWrapper_xQueueGiveFromISR(xQueue, pxHigherPriorityTaskWoken, queueSEND_TO_BACK); } \
BaseType_t MptWrapper_xQueueGiveFromISR(__a, __b, const BaseType_t xCopyPosition)
#define xQueueGiveFromISR__uxQueueType(__a, __b) xQueueGiveFromISR(__a,__b)

#undef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB) \
    if(pxNewTCB != NULL) \
    { \
        prvTraceRegisterTaskProbe(pxNewTCB, pxNewTCB->uxTCBNumber, pxNewTCB->uxPriority); \
    }

#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND(pxTaskToSuspend) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        prvTraceLogEventWithPayload(TRACE_EVENT_TASK_SUSPEND, TRACE_GET_TASK_PROBE_ID(pxTaskToSuspend)); \
    }

#undef traceTASK_DELAY
#define traceTASK_DELAY() \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        prvTraceLogEventWithPayload(TRACE_EVENT_TASK_DELAY, xTicksToDelay); \
    }

#undef traceTASK_DELAY_UNTIL
#define traceTASK_DELAY_UNTIL(xTimeToWake) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        prvTraceLogEventWithPayload(TRACE_EVENT_TASK_DELAY_UNTIL, xTimeToWake); \
    }

#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(prvTraceSwitchedTCB(pxCurrentTCB)) \
        { \
            prvTraceMergeSnapshot(); \
        } \
        prvTraceLogEventWithPayload(TRACE_EVENT_TASK_SWITCHED_IN, pxCurrentTCB->uxPriority); \
    }

#undef traceTASK_SWITCHED_OUT
#define traceTASK_SWITCHED_OUT() \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        prvTraceLogEvent(TRACE_EVENT_TASK_SWITCHED_OUT); \
        prvTraceProduceSnapshot(); \
    }

#undef traceQUEUE_SEND
#define traceQUEUE_SEND(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(xCopyPosition == queueSEND_TO_BACK ? TRACE_EVENT_QUEUE_SEND : TRACE_EVENT_QUEUE_SEND_FRONT, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_GIVE, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_MUTEX) || (pxQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)) \
        { \
            prvTraceLogEvent(TRACE_EVENT_MUTEX_GIVE); \
        } \
    }

#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(xCopyPosition == queueSEND_TO_BACK ? TRACE_EVENT_QUEUE_SEND_FAILED : TRACE_EVENT_QUEUE_SEND_FRONT_FAILED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_GIVE_FAILED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_MUTEX) || (pxQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)) \
        { \
            prvTraceLogEvent(TRACE_EVENT_MUTEX_GIVE_FAILED); \
        } \
    }

#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(xCopyPosition == queueSEND_TO_BACK ? TRACE_EVENT_QUEUE_SEND_BLOCKED : TRACE_EVENT_QUEUE_SEND_FRONT_BLOCKED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_GIVE_BLOCKED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_MUTEX) || (pxQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)) \
        { \
            prvTraceLogEvent(TRACE_EVENT_MUTEX_GIVE_BLOCKED); \
        } \
    }

#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(xCopyPosition == queueSEND_TO_BACK ? TRACE_EVENT_QUEUE_SEND_FROMISR : TRACE_EVENT_QUEUE_SEND_FRONT_FROMISR, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_GIVE_FROMISR, pxQueue->uxMessagesWaiting); \
        } \
    }

#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(xCopyPosition == queueSEND_TO_BACK ? TRACE_EVENT_QUEUE_SEND_FROMISR_FAILED : TRACE_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED, pxQueue->uxMessagesWaiting); \
        } \
    }

#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_QUEUE_RECEIVE, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_TAKE, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_MUTEX) || (pxQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)) \
        { \
            prvTraceLogEvent(TRACE_EVENT_MUTEX_TAKE); \
        } \
    }

#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_QUEUE_RECEIVE_FAILED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_TAKE_FAILED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_MUTEX) || (pxQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)) \
        { \
            prvTraceLogEvent(TRACE_EVENT_MUTEX_TAKE_FAILED); \
        } \
    }

#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_QUEUE_RECEIVE_BLOCKED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_TAKE_BLOCKED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_MUTEX) || (pxQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)) \
        { \
            prvTraceLogEvent(TRACE_EVENT_MUTEX_TAKE_BLOCKED); \
        } \
    }

#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_QUEUE_RECEIVE_FROMISR, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_TAKE_FROMISR, pxQueue->uxMessagesWaiting); \
        } \
    }

#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_QUEUE_RECEIVE_FROMISR_FAILED, pxQueue->uxMessagesWaiting); \
        } \
        else if((pxQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) || (pxQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE)) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED, pxQueue->uxMessagesWaiting); \
        } \
    }

#endif /* defined(MPT_TRACING_ENABLED) && (MPT_TRACING_ENABLED == 1) */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODALITY_PROBE_FREERTOS_H */
