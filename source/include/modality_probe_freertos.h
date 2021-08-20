#ifndef MODALITY_PROBE_FREERTOS_H
#define MODALITY_PROBE_FREERTOS_H

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MPT_TRACING_ENABLED configUSE_TRACE_FACILITY

#if defined(MPT_TRACING_ENABLED) && (MPT_TRACING_ENABLED == 1)

/* Static tracing event IDs */
#define TRACE_EVENT_ID_TOP (0xFFFF)
#define TRACE_EVENT_TASK_CREATE (TRACE_EVENT_ID_TOP - 1)
#define TRACE_EVENT_TASK_SWITCHED_IN (TRACE_EVENT_ID_TOP - 2)
#define TRACE_EVENT_TASK_SWITCHED_OUT (TRACE_EVENT_ID_TOP - 3)
#define TRACE_EVENT_QUEUE_SEND (TRACE_EVENT_ID_TOP - 4)
#define TRACE_EVENT_QUEUE_SEND_BACK_FAILED (TRACE_EVENT_ID_TOP - 5)
#define TRACE_EVENT_QUEUE_SEND_FRONT_FAILED (TRACE_EVENT_ID_TOP - 6)
#define TRACE_EVENT_QUEUE_RECEIVE (TRACE_EVENT_ID_TOP - 7)
#define TRACE_EVENT_QUEUE_RECEIVE_FAILED (TRACE_EVENT_ID_TOP - 8)

#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()
#define TRACE_GET_TASK_PROBE_ID(pxTCB) prvTraceGetTaskProbeId(pxTCB)
#define TRACE_GET_TASK_FILTER(pxTCB) prvTraceGetTaskFilter(pxTCB)
#define TRACE_SET_TASK_FILTER(pxTCB, filter) prvTraceSetTaskFilter(pxTCB, filter)
#define TRACE_GET_FILTER_MASK() prvTraceGetFilterMask()

#undef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB) \
    if(pxNewTCB != NULL) \
    { \
        prvTraceRegisterTaskProbe(pxNewTCB, pxNewTCB->uxTCBNumber, pxNewTCB->uxPriority); \
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

/* TODO - need to switch on type, support semaphores, mutex, etc */
#undef traceQUEUE_SEND
#define traceQUEUE_SEND(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_QUEUE_SEND, pxQueue->uxMessagesWaiting); \
        } \
    }

#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(xCopyPosition == queueSEND_TO_BACK ? TRACE_EVENT_QUEUE_SEND_BACK_FAILED : TRACE_EVENT_QUEUE_SEND_FRONT_FAILED, pxQueue->uxMessagesWaiting); \
        } \
    }

/* TODO - need to switch on peek vs recv */
#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE(pxQueue) \
    if(TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & TRACE_GET_FILTER_MASK()) \
    { \
        if(pxQueue->ucQueueType == queueQUEUE_TYPE_BASE) \
        { \
            prvTraceLogEventWithPayload(TRACE_EVENT_QUEUE_RECEIVE, pxQueue->uxMessagesWaiting); \
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
    }

#endif /* defined(MPT_TRACING_ENABLED) && (MPT_TRACING_ENABLED == 1) */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODALITY_PROBE_FREERTOS_H */
