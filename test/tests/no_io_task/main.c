#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"

static void task_a_fn(void* params)
{
    void* task = TRACE_GET_CURRENT_TASK();
    assert(task != NULL);
    assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[0]);

    assert(pvTraceGetControlPlaneBuffer() != NULL);
    vTraceProcessControlPlaneMessages();
    ASSERT_TRACE_ERROR();
}

static void task_b_fn(void* params)
{
    void* task = TRACE_GET_CURRENT_TASK();
    assert(task != NULL);
    assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[1]);

    assert(pvTraceGetControlPlaneBuffer() != NULL);
    vTraceProcessControlPlaneMessages();
    ASSERT_TRACE_ERROR();
}

int main(int argc, char **argv)
{
    BaseType_t ret;

    vTraceEnable();
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 1);

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

    taskENTER_CRITICAL();
    vTraceProbeIterator(util_write_binary_log_to_file_iter_fn);
    taskEXIT_CRITICAL();

    ASSERT_CRITICAL_NESTING_DEPTH_ZERO();

    return EXIT_SUCCESS;
}
