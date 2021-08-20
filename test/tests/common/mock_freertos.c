#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "message_buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "util.h"

#ifdef NDEBUG
#error "NDEBUG should not be defined in tests"
#endif
#ifndef MODALITY_PROBE_TRACE_TESTS
#error "MODALITY_PROBE_TRACE_TESTS != 1"
#endif

#define prvGetTCBFromHandle( pxHandle )    ( ( ( pxHandle ) == NULL ) ? pxCurrentTCB : ( pxHandle ) )

typedef struct
{
    UBaseType_t uxTCBNumber;
    UBaseType_t uxTaskNumber;
    UBaseType_t uxPriority;
    TaskHookFunction_t pxTaskTag;
    TaskFunction_t pxTaskCode;
    void* pvParameters;
    char pcTaskName[configMAX_TASK_NAME_LEN];
} TCB_t;

/* Simple 1-octet depth queue */
typedef struct
{
    UBaseType_t uxQueueNumber;
    UBaseType_t uxMessagesWaiting;
    UBaseType_t uxLength;
    uint8_t ucQueueType;
    uint8_t buffer;
} QueueDefinition;

/* No-op stream buffer */
typedef struct
{
    size_t xBufferSizeBytes;
    size_t xTriggerLevelBytes;
    BaseType_t xIsMessageBuffer;
    UBaseType_t uxStreamBufferNumber;
} StreamBufferDef_t;

/* Not static so tests can access */
size_t uxCriticalNesting;

UBaseType_t mock_uxTaskNumber = 0;
TCB_t mock_tasks[MAX_TASKS] = {{0}};
TCB_t* pxCurrentTCB = NULL;

UBaseType_t mock_uxQueueNumber = 0;
QueueDefinition mock_queues[MAX_QUEUES] = {{0}};

UBaseType_t mock_uxStreamBufferNumber = 0;
StreamBufferDef_t mock_msg_buffers[MAX_MSG_BUFFERS] = {{0}};

void* pvPortMalloc(size_t xSize)
{
    return malloc(xSize);
}

void vPortFree(void* pv)
{
    free(pv);
}

void vPortEnterCritical(void)
{
    uxCriticalNesting += 1;
}

void vPortExitCritical(void)
{
    assert(uxCriticalNesting > 0);
    uxCriticalNesting -= 1;
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    return (TaskHandle_t) pxCurrentTCB;
}

BaseType_t xTaskCreate(
        TaskFunction_t pxTaskCode,
        const char * const pcName,
        const configSTACK_DEPTH_TYPE usStackDepth,
        void * const pvParameters,
        UBaseType_t uxPriority,
        TaskHandle_t * const pxCreatedTask)
{
    assert(mock_uxTaskNumber < MAX_TASKS);
    TCB_t* const pxNewTCB = &mock_tasks[mock_uxTaskNumber];
    mock_uxTaskNumber += 1;
    pxNewTCB->uxTCBNumber = mock_uxTaskNumber;
    pxNewTCB->uxPriority = uxPriority;
    (void) strncpy(pxNewTCB->pcTaskName, pcName, configMAX_TASK_NAME_LEN);
    pxNewTCB->pxTaskCode = pxTaskCode;
    pxNewTCB->pvParameters = pvParameters;
    if(pxCreatedTask != NULL)
    {
        *pxCreatedTask = (TaskHandle_t) pxNewTCB;
    }
    traceTASK_CREATE(pxNewTCB);
    return pdPASS;
}

TaskHandle_t xTaskCreateStatic(
        TaskFunction_t pxTaskCode,
        const char * const pcName,
        const uint32_t ulStackDepth,
        void * const pvParameters,
        UBaseType_t uxPriority,
        StackType_t * const puxStackBuffer,
        StaticTask_t * const pxTaskBuffer)
{
    TaskHandle_t pxCreatedTask = NULL;
    BaseType_t ret = xTaskCreate(
            pxTaskCode,
            pcName,
            (configSTACK_DEPTH_TYPE) ulStackDepth,
            pvParameters,
            uxPriority,
            &pxCreatedTask);
    if(ret == pdPASS)
    {
        return pxCreatedTask;
    }
    else
    {
        return NULL;
    }
}

void vTaskSetApplicationTaskTag(
        TaskHandle_t xTask,
        TaskHookFunction_t pxHookFunction)
{
    TCB_t* tcb;
    if(xTask == NULL)
    {
        tcb = pxCurrentTCB;
    }
    else
    {
        tcb = (TCB_t*) xTask;
    }
    assert(tcb != NULL);
    tcb->pxTaskTag = pxHookFunction;
}

TaskHookFunction_t xTaskGetApplicationTaskTag(TaskHandle_t xTask)
{
    TCB_t* const tcb = (TCB_t*) xTask;
    assert(tcb != NULL);
    return tcb->pxTaskTag;
}

UBaseType_t uxTaskGetTaskNumber(TaskHandle_t xTask)
{
    UBaseType_t uxReturn;
    TCB_t const * pxTCB;

    if(xTask != NULL)
    {
        pxTCB = (TCB_t*) xTask;
        uxReturn = pxTCB->uxTaskNumber;
    }
    else
    {
        uxReturn = 0U;
    }
    return uxReturn;
}

void vTaskSetTaskNumber(
        TaskHandle_t xTask,
        const UBaseType_t uxHandle)
{
    TCB_t * pxTCB;

    if(xTask != NULL)
    {
        pxTCB = (TCB_t*) xTask;
        pxTCB->uxTaskNumber = uxHandle;
    }
}

char * pcTaskGetName(TaskHandle_t xTaskToQuery)
{
    TCB_t * pxTCB;
    pxTCB = prvGetTCBFromHandle( ((TCB_t*) xTaskToQuery) );
    assert(pxTCB != NULL);
    return &( pxTCB->pcTaskName[ 0 ] );
}

void vTaskDelay(const TickType_t xTicksToDelay)
{
    /* no-op */
    (void) xTicksToDelay;
}

/* Simple mock scheduler, runs each task function NUM_TASK_ITERS times (in the order they were created)
 * NUM_SCHEDULER_ITERS times */
void vTaskStartScheduler(void)
{
    int sched_iter;
    int task_iter;
    int task_idx;
    for(sched_iter = 0; sched_iter < NUM_SCHEDULER_ITERS; sched_iter += 1)
    {
        for(task_idx = 0; task_idx < MAX_TASKS; task_idx += 1)
        {
            pxCurrentTCB = &mock_tasks[task_idx];
            if(pxCurrentTCB->uxTCBNumber == 0)
            {
                continue;
            }

            traceTASK_SWITCHED_IN();

            for(task_iter = 0; task_iter < NUM_TASK_ITERS; task_iter += 1)
            {
                if(pxCurrentTCB->pxTaskCode != NULL)
                {
#if defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1)
                    /* Don't call the IO task fn, it's not mocked out and would never return */
                    if(strncmp(pxCurrentTCB->pcTaskName, TRACE_IO_TASK_NAME, configMAX_TASK_NAME_LEN) != 0)
                    {
                        pxCurrentTCB->pxTaskCode(pxCurrentTCB->pvParameters);
                    }
                    else
                    {
                        /* Make sure we filter out the IO task by default */
                        assert(TRACE_GET_TASK_FILTER(pxCurrentTCB) == 0);
                    }
#else
                    pxCurrentTCB->pxTaskCode(pxCurrentTCB->pvParameters);
#endif /* defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1) */
                }
            }

            traceTASK_SWITCHED_OUT();
        }
    }
}

QueueHandle_t xQueueGenericCreate(
        const UBaseType_t uxQueueLength,
        const UBaseType_t uxItemSize,
        const uint8_t ucQueueType)
{
    assert(uxQueueLength == 1);
    assert(uxItemSize == 1);
    assert(mock_uxQueueNumber < MAX_QUEUES);
    QueueDefinition* const pxQueue = &mock_queues[mock_uxQueueNumber];
    mock_uxQueueNumber += 1;
    pxQueue->uxQueueNumber = mock_uxQueueNumber;
    pxQueue->uxMessagesWaiting = 0;
    pxQueue->uxLength = MAX_QUEUE_LENGTH;
    pxQueue->ucQueueType = ucQueueType;
    return (QueueHandle_t) pxQueue;
}

BaseType_t xQueueGenericSend(
        QueueHandle_t xQueue,
        const void * const pvItemToQueue,
        TickType_t xTicksToWait,
        const BaseType_t xCopyPosition)
{
    BaseType_t ret = pdTRUE;
    assert(xQueue != NULL);
    QueueDefinition* const pxQueue = (QueueDefinition*) xQueue;
    if(pxQueue->uxMessagesWaiting < pxQueue->uxLength)
    {
        traceQUEUE_SEND(pxQueue);
        (void) memcpy(&pxQueue->buffer, pvItemToQueue, QUEUE_ITEM_SIZE);
        pxQueue->uxMessagesWaiting += 1;
    }
    else
    {
        traceQUEUE_SEND_FAILED(pxQueue);
        ret = errQUEUE_FULL;
    }
    return ret;
}

BaseType_t xQueueReceive(
        QueueHandle_t xQueue,
        void * const pvBuffer,
        TickType_t xTicksToWait)
{
    BaseType_t ret = pdFALSE;
    assert(xQueue != NULL);
    QueueDefinition* const pxQueue = (QueueDefinition*) xQueue;
    if(pxQueue->uxMessagesWaiting != 0)
    {
        (void) memcpy(pvBuffer, &pxQueue->buffer, QUEUE_ITEM_SIZE);
        traceQUEUE_RECEIVE(pxQueue);
        pxQueue->uxMessagesWaiting -= 1;
        ret = pdTRUE;
    }
    else
    {
        traceQUEUE_RECEIVE_FAILED(pxQueue);
    }
    return ret;
}

StreamBufferHandle_t xStreamBufferGenericCreate(
        size_t xBufferSizeBytes,
        size_t xTriggerLevelBytes,
        BaseType_t xIsMessageBuffer)
{
    assert(mock_uxStreamBufferNumber < MAX_MSG_BUFFERS);
    StreamBufferDef_t* const pxMessageBuffer = &mock_msg_buffers[mock_uxStreamBufferNumber];
    mock_uxStreamBufferNumber += 1;
    pxMessageBuffer->uxStreamBufferNumber = mock_uxStreamBufferNumber;
    pxMessageBuffer->xBufferSizeBytes = xBufferSizeBytes;
    pxMessageBuffer->xTriggerLevelBytes = xTriggerLevelBytes;
    pxMessageBuffer->xIsMessageBuffer = xIsMessageBuffer;
    return (StreamBufferHandle_t) pxMessageBuffer;
}

size_t xStreamBufferReceive(
        StreamBufferHandle_t xStreamBuffer,
        void * pvRxData,
        size_t xBufferLengthBytes,
        TickType_t xTicksToWait)
{
    assert(xStreamBuffer != NULL);
    assert(pvRxData != NULL);
    assert(xBufferLengthBytes != 0);
    return 0;
}

size_t xStreamBufferSend(
        StreamBufferHandle_t xStreamBuffer,
        const void * pvTxData,
        size_t xDataLengthBytes,
        TickType_t xTicksToWait)
{
    assert(xStreamBuffer != NULL);
    assert(pvTxData != NULL);
    assert(xDataLengthBytes != 0);
    return xDataLengthBytes;
}
