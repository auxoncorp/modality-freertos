#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"

static void task_a_fn(void* params)
{
    vTraceProcessControlPlaneMessages();
    assert(TRACE_GET_CURRENT_TASK() == NULL);
    assert(xTaskGetApplicationTaskTag(xTaskGetCurrentTaskHandle()) == NULL);
    assert(pvTraceGetControlPlaneBuffer() == NULL);
    ASSERT_TRACE_ERROR();
}

static void task_b_fn(void* params)
{
    vTraceProcessControlPlaneMessages();
    assert(TRACE_GET_CURRENT_TASK() == NULL);
    assert(xTaskGetApplicationTaskTag(xTaskGetCurrentTaskHandle()) == NULL);
    assert(pvTraceGetControlPlaneBuffer() == NULL);
    ASSERT_TRACE_ERROR();
}

int main(int argc, char **argv)
{
    BaseType_t ret;

    vTraceEnable(0);
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 0);

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

    vTraceStop();
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 0);

    ASSERT_CRITICAL_NESTING_DEPTH_ZERO();

    return EXIT_SUCCESS;
}
