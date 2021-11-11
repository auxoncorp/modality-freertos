#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"

static StackType_t g_stack_a[configMINIMAL_STACK_SIZE];
static StaticTask_t g_tcb_a;

static StackType_t g_stack_b[configMINIMAL_STACK_SIZE];
static StaticTask_t g_tcb_b;

static void task_a_fn(void* params)
{
    void* task = TRACE_GET_CURRENT_TASK();
    assert(task != NULL);
    assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[0]);
    ASSERT_TRACE_ERROR();
}

static void task_b_fn(void* params)
{
    void* task = TRACE_GET_CURRENT_TASK();
    assert(task != NULL);
    assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[1]);
    ASSERT_TRACE_ERROR();
}

int main(int argc, char **argv)
{
    assert(configSUPPORT_DYNAMIC_ALLOCATION == 0);

    vTraceEnable();
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 1);

    TaskHandle_t handle_a = xTaskCreateStatic(
            task_a_fn,
            TASK_NAMES[0],
            configMINIMAL_STACK_SIZE,
            NULL, /* pvParameters */
            configMAX_PRIORITIES - 1,
            &g_stack_a[0],
            &g_tcb_a);
    ASSERT_TRACE_ERROR();
    assert(handle_a != NULL);

    TaskHandle_t handle_b = xTaskCreateStatic(
            task_b_fn,
            TASK_NAMES[1],
            configMINIMAL_STACK_SIZE,
            NULL, /* pvParameters */
            configMAX_PRIORITIES - 1,
            &g_stack_b[0],
            &g_tcb_b);
    ASSERT_TRACE_ERROR();
    assert(handle_b != NULL);

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
