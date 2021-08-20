#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"

static int g_logged_events_per_tcb[MPT_CFG_MAX_PROBES] = {0};

static void task_fn(void* params)
{
    assert(params != NULL);
    uint32_t event_id = *(uint32_t*) params;
    assert(
            (event_id == USER_EVENT_ID_A)
            || (event_id == USER_EVENT_ID_B)
            || (event_id == USER_EVENT_ID_C)
            || (event_id == USER_EVENT_ID_D));

    void* task = TRACE_GET_CURRENT_TASK();
    assert(task != NULL);

    /* Make the compiler happy, ISO C forbids conversion of object pointer to function pointer type */
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    modality_probe* probe = (modality_probe*) xTaskGetApplicationTaskTag(xTaskGetCurrentTaskHandle());
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    if(event_id == USER_EVENT_ID_A)
    {
        assert(TRACE_GET_TASK_FILTER(task) != 0);
        assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[0]);
        g_logged_events_per_tcb[0] += 1;
        assert(pvTraceGetControlPlaneBuffer() != NULL);
        assert(probe != NULL);

        size_t err = MODALITY_PROBE_RECORD(probe, event_id);
        assert(err == 0);
    }
    else if(event_id == USER_EVENT_ID_B)
    {
        assert(TRACE_GET_TASK_FILTER(task) == 0);
        assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[1]);
        g_logged_events_per_tcb[1] += 1;
        assert(pvTraceGetControlPlaneBuffer() == NULL);
        assert(probe == NULL);
    }
    else if(event_id == USER_EVENT_ID_C)
    {
        assert(TRACE_GET_TASK_FILTER(task) == 0);
        assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[2]);
        g_logged_events_per_tcb[2] += 1;
        assert(pvTraceGetControlPlaneBuffer() == NULL);
        assert(probe == NULL);
    }
    else if(event_id == USER_EVENT_ID_D)
    {
        assert(TRACE_GET_TASK_FILTER(task) != 0);
        assert(TRACE_GET_TASK_PROBE_ID(task) == TASK_PROBE_IDS[3]);
        g_logged_events_per_tcb[3] += 1;
        assert(pvTraceGetControlPlaneBuffer() != NULL);
        assert(probe != NULL);

        size_t err = MODALITY_PROBE_RECORD(probe, event_id);
        assert(err == 0);
    }

    vTraceProcessControlPlaneMessages();

    ASSERT_TRACE_ERROR();
}

int main(int argc, char **argv)
{
    BaseType_t ret;
    int i;

    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 0);

    vTraceEnable(0);
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 1);

    // TASK_NAMES[1] and TASK_NAMES[2] will not be traced
    vTraceExcludeTask(TASK_NAMES[1]);
    vTraceExcludeTask(TASK_NAMES[2]);

    for(i = 0; i < MPT_CFG_MAX_PROBES; i += 1)
    {
        ret = xTaskCreate(
                task_fn,
                TASK_NAMES[i],
                configMINIMAL_STACK_SIZE,
                (void*) &USER_EVENTS[i],
                configMAX_PRIORITIES - 1,
                NULL);
        ASSERT_TRACE_ERROR();
        assert(ret == pdPASS);
    }

    vTaskStartScheduler();
    ASSERT_TRACE_ERROR();

    vTraceStop();
    ASSERT_TRACE_ERROR();
    assert(xTraceIsEnabled() == 0);

    for(i = 0; i < MPT_CFG_MAX_PROBES; i += 1)
    {
        assert(g_logged_events_per_tcb[i] == (NUM_SCHEDULER_ITERS * NUM_TASK_ITERS));
    }

    taskENTER_CRITICAL();
    vTraceProbeIterator(util_write_binary_log_to_file_iter_fn);
    taskEXIT_CRITICAL();

    ASSERT_CRITICAL_NESTING_DEPTH_ZERO();

    return EXIT_SUCCESS;
}
