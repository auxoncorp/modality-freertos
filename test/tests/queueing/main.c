#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"

static QueueHandle_t g_msg_queue = NULL;
static int g_items_sent = 0;
static int g_items_recvd = 0;

static void task_a_fn(void* params)
{
    static uint8_t payload = 1;

    const BaseType_t ret = xQueueSend(g_msg_queue, &payload, 0);
    if(ret == pdTRUE)
    {
        g_items_sent += 1;
    }

    ASSERT_TRACE_ERROR();

    void* task = TRACE_GET_CURRENT_TASK();
    assert(task != NULL);
    assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[0]);
}

static void task_b_fn(void* params)
{
    uint8_t payload = 0;
    const BaseType_t ret = xQueueReceive(g_msg_queue, &payload, 0);
    if(ret == pdTRUE)
    {
        g_items_recvd += 1;
    }

    ASSERT_TRACE_ERROR();

    void* task = TRACE_GET_CURRENT_TASK();
    assert(task != NULL);
    assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[1]);
}

int main(int argc, char **argv)
{
    BaseType_t ret;

    vTraceEnable();
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 1);

    g_msg_queue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(uint8_t));
    ASSERT_TRACE_ERROR();
    assert(g_msg_queue != NULL);

    ret = xTaskCreate(
            task_a_fn,
            TASK_NAMES[0],
            configMINIMAL_STACK_SIZE,
            (void*) &USER_EVENTS[0],
            configMAX_PRIORITIES - 1,
            NULL);
    ASSERT_TRACE_ERROR();
    assert(ret == pdPASS);

    ret = xTaskCreate(
            task_b_fn,
            TASK_NAMES[1],
            configMINIMAL_STACK_SIZE,
            (void*) &USER_EVENTS[1],
            configMAX_PRIORITIES - 1,
            NULL);
    ASSERT_TRACE_ERROR();
    assert(ret == pdPASS);

    vTaskStartScheduler();
    ASSERT_TRACE_ERROR();

    vTraceDisable();
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 0);

    assert(g_items_sent == NUM_SCHEDULER_ITERS);
    assert(g_items_recvd == NUM_SCHEDULER_ITERS);

    taskENTER_CRITICAL();
    vTraceProbeIterator(util_write_binary_log_to_file_iter_fn);
    taskEXIT_CRITICAL();

    ASSERT_CRITICAL_NESTING_DEPTH_ZERO();

    return EXIT_SUCCESS;
}
