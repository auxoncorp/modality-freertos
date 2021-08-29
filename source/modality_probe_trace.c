#include "FreeRTOS.h"

#if !defined(MPT_TRACING_ENABLED) && (configUSE_TRACE_FACILITY == 1)
#error "Modality probe tracing header file modality_probe_trace.h needs to be included in your FreeRTOSConfig.h file"
#endif

#if defined(MPT_TRACING_ENABLED) && (MPT_TRACING_ENABLED == 1)

#include "modality_probe_trace.h"
#include "task.h"

#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
#include "message_buffer.h"
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <modality/probe.h>

typedef struct
{
    void* tcb;
    modality_probe* probe;
    modality_probe_causal_snapshot snapshot;
#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
    MessageBufferHandle_t control_msg_buffer;
    uint8_t *task_local_buffer; /* Size is MPT_CFG_CONTROL_PLANE_TASK_BUFFER_SIZE */
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */
} tcb_probe_s;

static volatile uint32_t g_is_enabled = 0;

static const char* g_error_msg = NULL;

static uint16_t g_filter_mask = 0xFFFF;

static void* g_current_tcb = NULL;
static void* g_prev_tcb = NULL;

static tcb_probe_s g_tcb_probes[MPT_CFG_MAX_PROBES] = { { NULL } };

static uint16_t g_excluded_task_probe_ids[MPT_CFG_MAX_EXCLUDED_TASKS] = { 0 };

#if defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1)
static uint8_t g_probe_storage[MPT_CFG_STORAGE_SIZE];
static size_t g_next_storage_index = 0;
#endif /* defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1) */

#if defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1)
#if defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1)
static StackType_t g_io_task_stack[MPT_CFG_IO_TASK_STACK_SIZE] = { 0 };
static StaticTask_t g_io_task_tcb;
#endif /* defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1) */
const char TRACE_IO_TASK_NAME[] = MPT_CFG_IO_TASK_NAME;
static TaskHandle_t g_io_task_handle = NULL;
static uint8_t g_io_buffer[MPT_CFG_IO_TASK_BUFFER_SIZE] = { 0 };
static portTASK_FUNCTION_PROTO(prvModalityProbeIo, pvParameters);
static void io_task_probe_report_iter_fn(modality_probe* probe);
static void io_task_probe_mutator_announcement_iter_fn(modality_probe* probe);
#endif /* defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1) */

/* Internal functions */
#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
static MessageBufferHandle_t alloc_message_buffer(void);
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */
static uint8_t* alloc_storage(size_t size_bytes);
static modality_probe* alloc_probe(size_t size_bytes, uint32_t probe_id);
static void add_tcb_probe(void* task, modality_probe* probe);
static tcb_probe_s* get_tcb_probe(void* task);
static int is_task_excluded(uint16_t probe_id);
static uint16_t probe_id_hash(const char* task_name, uint8_t length);

void vTraceEnable(int startOption)
{
    (void) startOption;
    TRACE_ALLOC_CRITICAL_SECTION();

    if(g_is_enabled != 0)
    {
        /* Already enabled */
        return;
    }

    TRACE_ASSERT(
            prvTraceGetCurrentTaskHandle() == NULL,
            "vTraceEnable should be called before vTaskStartScheduler()",
            MPT_UNUSED);

    TRACE_ENTER_CRITICAL_SECTION();

#if defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1)
    if(g_io_task_handle == NULL)
    {
        TRACE_IO_INIT();

        vTraceExcludeTask(TRACE_IO_TASK_NAME);

        TRACE_DEBUG_PRINTF(("Creating %s task\n", TRACE_IO_TASK_NAME));

#if defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1)
        g_io_task_handle = xTaskCreateStatic(
                prvModalityProbeIo,
                TRACE_IO_TASK_NAME,
                MPT_CFG_IO_TASK_STACK_SIZE,
                NULL, /* pvParameters */
                MPT_CFG_IO_TASK_PRIORITY,
                &g_io_task_stack[0],
                &g_io_task_tcb);
#else
        xTaskCreate(
                prvModalityProbeIo,
                TRACE_IO_TASK_NAME,
                MPT_CFG_IO_TASK_STACK_SIZE,
                NULL, /* pvParameters */
                MPT_CFG_IO_TASK_PRIORITY,
                &g_io_task_handle);
#endif /* defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1) */

        if(g_io_task_handle == NULL)
        {
            prvTraceError("Failed to create ModalityProbeIo task");
        }
    }
#endif /* defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1) */

    g_is_enabled = 1;

    TRACE_EXIT_CRITICAL_SECTION();
}

void vTraceStop(void)
{
    g_is_enabled = 0;
}

int xTraceIsEnabled(void)
{
    return g_is_enabled != 0;
}

const char* pcTraceGetError(void)
{
    return g_error_msg;
}

void vTraceExcludeTask(const char* pcName)
{
    int i;
    uint16_t probe_id;
    int slot_was_found = 0;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    probe_id = probe_id_hash(pcName, STRLEN(pcName));

    for(i = 0; i < MPT_CFG_MAX_EXCLUDED_TASKS; i += 1)
    {
        if(g_excluded_task_probe_ids[i] == 0)
        {
            g_excluded_task_probe_ids[i] = probe_id;
            slot_was_found = 1;
            break;
        }
    }

    if(slot_was_found == 0)
    {
        prvTraceError("No more excluded task slots available, increase MPT_CFG_MAX_EXCLUDED_TASKS");
    }

    TRACE_EXIT_CRITICAL_SECTION();
}

void vTraceProbeIterator(TraceProbeIteratorFunction_t pvIteratorFunction)
{
    int i;

    if(pvIteratorFunction != NULL)
    {
        for(i = 0; i < MPT_CFG_MAX_PROBES; i += 1)
        {
            if(g_tcb_probes[i].probe != NULL)
            {
                pvIteratorFunction(g_tcb_probes[i].probe);
            }
        }
    }
}

#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
void* pvTraceGetControlPlaneBuffer(void)
{
    tcb_probe_s* tcb_probe;
    MessageBufferHandle_t buffer = NULL;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    tcb_probe = get_tcb_probe(TRACE_GET_CURRENT_TASK());
    if(tcb_probe != NULL)
    {
        buffer = tcb_probe->control_msg_buffer;
    }

    TRACE_EXIT_CRITICAL_SECTION();

    return buffer;
}

void vTraceProcessControlPlaneMessages(void)
{
    size_t err;
    size_t should_forward;
    tcb_probe_s* tcb_probe;
    modality_probe* probe = NULL;
    size_t bytes_recvd = 0;
    MessageBufferHandle_t control_msg_buffer = NULL;
    uint8_t *task_local_buffer = NULL;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    tcb_probe = get_tcb_probe(TRACE_GET_CURRENT_TASK());
    if(tcb_probe != NULL)
    {
        probe = tcb_probe->probe;
        control_msg_buffer = tcb_probe->control_msg_buffer;
        task_local_buffer = tcb_probe->task_local_buffer;
    }

    TRACE_EXIT_CRITICAL_SECTION();

    if((control_msg_buffer != NULL) && (task_local_buffer != NULL))
    {
        bytes_recvd = xMessageBufferReceive(
                control_msg_buffer,
                task_local_buffer,
                MPT_CFG_CONTROL_PLANE_TASK_BUFFER_SIZE,
                0);
    }

    if(bytes_recvd != 0)
    {
        TRACE_ENTER_CRITICAL_SECTION();

        err = modality_probe_process_control_message(probe, task_local_buffer, bytes_recvd, &should_forward);

        TRACE_EXIT_CRITICAL_SECTION();

        TRACE_DEBUG_PRINTF(
                ("Task %s processed a control message\n", pcTaskGetName(tcb_probe->tcb)));

        TRACE_ASSERT(err == 0, "Failed to process Modality probe control message", MPT_UNUSED);
    }
}
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */

/* Private functions */
void prvTraceError(const char* msg)
{
    vTraceStop();

    if(g_error_msg == NULL)
    {
        g_error_msg = msg;
        TRACE_DEBUG_PRINTF(("TRACE ERROR: %s\n", msg));
    }
}

uint16_t prvTraceGetFilterMask(void)
{
    return g_filter_mask;
}

void* prvTraceGetCurrentTaskHandle(void)
{
    return xTaskGetCurrentTaskHandle();
}

uint16_t prvTraceGetTaskProbeId(void* task)
{
    return TRACE_READ_LOWER16(uxTaskGetTaskNumber(task));
}

void prvTraceSetTaskProbeId(void* task, uint16_t probe_id)
{
    vTaskSetTaskNumber(task, TRACE_WRITE_LOWER16(uxTaskGetTaskNumber(task), probe_id));
}

uint16_t prvTraceGetTaskFilter(void* task)
{
    return TRACE_READ_UPPER16(uxTaskGetTaskNumber(task));
}

void prvTraceSetTaskFilter(void* task, uint16_t filter)
{
    vTaskSetTaskNumber(task, TRACE_WRITE_UPPER16(uxTaskGetTaskNumber(task), filter));
}

void prvTraceRegisterTaskProbe(void* task, uint32_t uxTCBNumber, uint32_t uxPriority)
{
    size_t err;
    uint16_t probe_id;
    int is_excluded;
    modality_probe *probe = NULL;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    if(g_is_enabled != 0)
    {
        char* task_name = pcTaskGetName(task);
        probe_id = probe_id_hash(task_name, STRLEN(task_name));

        prvTraceSetTaskProbeId(task, probe_id);
        prvTraceSetTaskFilter(task, 0);

        is_excluded = is_task_excluded(probe_id);

        if(is_excluded == 0)
        {
            TRACE_DEBUG_PRINTF(("Registering probe ID %lu for task '%s'\n", probe_id, task_name));
            probe = alloc_probe(MPT_CFG_TASK_PROBE_SIZE, probe_id);
        }

        if(probe != NULL)
        {
            add_tcb_probe(task, probe);

#if defined(MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG) && (MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG == 1)
            /* Make the compiler happy, ISO C forbids conversion of object pointer to function pointer type */
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif /* __GNUC__ */
            vTaskSetApplicationTaskTag(task, (void*) probe);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /* __GNUC__ */
#endif /* defined(MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG) && (MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG == 1) */

            prvTraceSetTaskFilter(task, 0xFFFF);

            err = modality_probe_record_event_with_payload(probe, TRACE_EVENT_TASK_CREATE, uxPriority);
            TRACE_ASSERT(err == 0, "Failed to record initial task creation event", MPT_UNUSED);
        }
    }

    TRACE_EXIT_CRITICAL_SECTION();
}

void prvTraceProduceSnapshot(void)
{
    size_t err;
    tcb_probe_s* tcb_probe;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    if((g_is_enabled != 0) && (g_current_tcb != NULL))
    {
        tcb_probe = get_tcb_probe(g_current_tcb);
        TRACE_ASSERT(
                tcb_probe != NULL,
                "No tcb_probe_s associated with the current task (1)",
                MPT_UNUSED);

        if(tcb_probe != NULL)
        {
            err = modality_probe_produce_snapshot(tcb_probe->probe, &tcb_probe->snapshot);
            TRACE_ASSERT(err == 0, "Failed to produce Modality probe snapshot", MPT_UNUSED);
        }
    }

    TRACE_EXIT_CRITICAL_SECTION();
}

void prvTraceMergeSnapshot(void)
{
    size_t err;
    tcb_probe_s* curr;
    tcb_probe_s* prev;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    if((g_is_enabled != 0) && (g_current_tcb != NULL) && (g_prev_tcb != NULL))
    {
        curr = get_tcb_probe(g_current_tcb);
        TRACE_ASSERT(
                curr != NULL,
                "No tcb_probe_s associated with the current task (2)",
                MPT_UNUSED);

        prev = get_tcb_probe(g_prev_tcb);
        TRACE_ASSERT(
                prev != NULL,
                "No tcb_probe_s associated with the previous task",
                MPT_UNUSED);

        err = modality_probe_merge_snapshot(curr->probe, &prev->snapshot);
        TRACE_ASSERT(err == 0, "Failed to merge Modality probe snapshot", MPT_UNUSED);
    }

    TRACE_EXIT_CRITICAL_SECTION();
}

void prvTraceLogEvent(uint32_t id)
{
    size_t err;
    tcb_probe_s* tcb_probe;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    if((g_is_enabled != 0) && (g_current_tcb != NULL))
    {
        tcb_probe = get_tcb_probe(g_current_tcb);
        TRACE_ASSERT(
                tcb_probe != NULL,
                "No tcb_probe_s associated with the current task (3)",
                MPT_UNUSED);

        if(tcb_probe != NULL)
        {
            err = modality_probe_record_event(tcb_probe->probe, id);
            TRACE_ASSERT(err == 0, "Failed to log Modality event", MPT_UNUSED);
        }
    }

    TRACE_EXIT_CRITICAL_SECTION();
}

void prvTraceLogEventWithPayload(uint32_t id, uint32_t payload)
{
    size_t err;
    tcb_probe_s* tcb_probe;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    if((g_is_enabled != 0) && (g_current_tcb != NULL))
    {
        tcb_probe = get_tcb_probe(g_current_tcb);
        TRACE_ASSERT(
                tcb_probe != NULL,
                "No tcb_probe_s associated with the current task (4)",
                MPT_UNUSED);

        if(tcb_probe != NULL)
        {
            err = modality_probe_record_event_with_payload(tcb_probe->probe, id, payload);
            TRACE_ASSERT(err == 0, "Failed to log Modality event with payload", MPT_UNUSED);
        }
    }

    TRACE_EXIT_CRITICAL_SECTION();
}

/* Called in the kernel context */
uint32_t prvTraceSwitchedTCB(void* pNewTCB)
{
    uint32_t did_switch_tcb = 0;

    if(g_current_tcb != pNewTCB)
    {
        g_prev_tcb = g_current_tcb;
        g_current_tcb = pNewTCB;
        did_switch_tcb = 1;
    }

    return did_switch_tcb;
}

/* Internal functions */
#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
static MessageBufferHandle_t alloc_message_buffer(void)
{
    MessageBufferHandle_t buffer = NULL;
    buffer = xMessageBufferCreate(MPT_CFG_CONTROL_PLANE_BUFFER_SIZE);
    return buffer;
}
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */

static uint8_t* alloc_storage(size_t size_bytes)
{
    uint8_t* storage = NULL;

#if defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1)
    if((g_next_storage_index + size_bytes) <= MPT_CFG_STORAGE_SIZE)
    {
        storage = &g_probe_storage[g_next_storage_index];
        g_next_storage_index += size_bytes;
    }
#else
    storage = (uint8_t*) TRACE_MALLOC(size_bytes);
#endif /* defined(MPT_CFG_STATIC_ALLOCATION) && (MPT_CFG_STATIC_ALLOCATION == 1) */

    return storage;
}

static modality_probe* alloc_probe(size_t size_bytes, uint32_t probe_id)
{
    modality_probe* probe = NULL;

    uint8_t* storage = alloc_storage(size_bytes);
    if(storage == NULL)
    {
        prvTraceError("Failed to allocate probe storage, increase MPT_CFG_STORAGE_SIZE");
    }
    else if(probe_id != 0)
    {
        size_t err = modality_probe_initialize(
                storage,
                size_bytes,
                probe_id,
                MODALITY_PROBE_TIME_RESOLUTION_UNSPECIFIED,
                MODALITY_PROBE_WALL_CLOCK_ID_LOCAL_ONLY,
                NULL,
                NULL,
                &probe);
        TRACE_ASSERT(err == 0, "Failed to initialize Modality probe", NULL);
    }

    return probe;
}

static void add_tcb_probe(void* task, modality_probe* probe)
{
    int i;
    size_t err;
    int slot_was_found = 0;

    for(i = 0; i < MPT_CFG_MAX_PROBES; i += 1)
    {
        if(g_tcb_probes[i].tcb == NULL)
        {
            err = modality_probe_produce_snapshot(probe, &g_tcb_probes[i].snapshot);
            TRACE_ASSERT(err == 0, "Failed to produce Modality probe snapshot", MPT_UNUSED);
            g_tcb_probes[i].tcb = task;
            g_tcb_probes[i].probe = probe;
            slot_was_found = 1;
            break;
        }
    }

    if(slot_was_found == 0)
    {
        prvTraceError("No more TCB probe slots available, increase MPT_CFG_MAX_PROBES");
    }

#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
    if(slot_was_found != 0)
    {
        MessageBufferHandle_t buffer;
        uint8_t* task_local_buffer;

        buffer = alloc_message_buffer();
        task_local_buffer = (uint8_t*) TRACE_MALLOC(MPT_CFG_CONTROL_PLANE_TASK_BUFFER_SIZE);

        if(buffer == NULL)
        {
            prvTraceError("Failed to allocate probe control plane message buffer");
        }
        if(task_local_buffer == NULL)
        {
            prvTraceError("Failed to allocate probe task local control plane buffer");
        }

        if((buffer != NULL) && (task_local_buffer != NULL))
        {
            g_tcb_probes[i].control_msg_buffer = buffer;
            g_tcb_probes[i].task_local_buffer = task_local_buffer;
        }
    }
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */
}

static tcb_probe_s* get_tcb_probe(void* task)
{
    int i;
    tcb_probe_s* tcb_probe = NULL;

    for(i = 0; i < MPT_CFG_MAX_PROBES; i += 1)
    {
        if(g_tcb_probes[i].tcb == task)
        {
            tcb_probe = &g_tcb_probes[i];
            break;
        }
    }

    return tcb_probe;
}

static int is_task_excluded(uint16_t probe_id)
{
    int i;
    int is_excluded = 0;

    for(i = 0; i < MPT_CFG_MAX_EXCLUDED_TASKS; i += 1)
    {
        if(g_excluded_task_probe_ids[i] == probe_id)
        {
            is_excluded = 1;
            break;
        }
    }

    return is_excluded;
}

/* CRC-16/IBM-3740 */
static uint16_t probe_id_hash(const char* task_name, uint8_t length)
{
    unsigned char x;
    uint16_t crc = 0xFFFF;

    TRACE_ASSERT(task_name != NULL, "Task name must not be NULL", 0);
    TRACE_ASSERT(length != 0, "Task name length == 0", 0);

    while(length-- != 0)
    {
        x = crc >> 8 ^ *task_name++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t) (x << 12)) ^ ((uint16_t) (x << 5)) ^ ((uint16_t) x);
    }

    return crc;
}

#if defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1)
static portTASK_FUNCTION(prvModalityProbeIo, pvParameters)
{
    (void) pvParameters;
    size_t err;
    int32_t status;
    int32_t num_bytes;
    uint32_t target_probe_id;
    uint16_t iters = 0;

#ifdef MPT_CFG_IO_TASK_STARTUP_DELAY
    vTaskDelay(MPT_CFG_IO_TASK_STARTUP_DELAY);
#endif /* MPT_CFG_IO_TASK_STARTUP_DELAY */

    while(1)
    {
        /* Check for incoming control plane messages */
        num_bytes = 0;
        status = TRACE_IO_READ(&g_io_buffer[0], MPT_CFG_IO_TASK_BUFFER_SIZE, &num_bytes);

        if(status != 0)
        {
            prvTraceError("TRACE_IO_READ returned an error");
        }

        if((status == 0) && (num_bytes > 0))
        {
            err = modality_probe_get_control_message_destination(
                    &g_io_buffer[0],
                    (size_t) num_bytes,
                    &target_probe_id);
            if(err != 0)
            {
                TRACE_DEBUG_PRINTF(("TRACE_IO_READ recieved invalid control message (err=%zu)\n", err));
            }
            else
            {
#if defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1)
                int i;
                for(i = 0; i < MPT_CFG_MAX_PROBES; i += 1)
                {
                    if((g_tcb_probes[i].tcb != NULL) && (g_tcb_probes[i].control_msg_buffer != NULL))
                    {
                        uint32_t probe_id = TRACE_GET_TASK_PROBE_ID(g_tcb_probes[i].tcb);
                        size_t bytes_sent_to_buffer = 0;
                        if((target_probe_id == MODALITY_PROBE_ID_BROADCAST) || (target_probe_id == probe_id))
                        {
                            bytes_sent_to_buffer = xMessageBufferSend(
                                    g_tcb_probes[i].control_msg_buffer,
                                    &g_io_buffer[0],
                                    (size_t) num_bytes,
                                    0);
                            if(bytes_sent_to_buffer == 0)
                            {
                                TRACE_DEBUG_PRINTF(
                                        ("Control message buffer send for task %s timed out\n", pcTaskGetName(g_tcb_probes[i].tcb)));
                            }
                            else
                            {
                                TRACE_DEBUG_PRINTF(
                                        ("Sent control message to task %s\n", pcTaskGetName(g_tcb_probes[i].tcb)));
                            }
                        }
                    }
                }
#endif /* defined(MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER) && (MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1) */
            }
        }

        /* Send probe reports */
        vTraceProbeIterator(io_task_probe_report_iter_fn);

        /* Send mutator announcements */
        if(iters > MPT_CFG_IO_TASK_ITERS_PER_MUTATOR_ANNOUNCEMENT)
        {
            iters = 0;
            vTraceProbeIterator(io_task_probe_mutator_announcement_iter_fn);
        }

        vTaskDelay(MPT_CFG_IO_TASK_DELAY);

        iters += 1;
    }
}

static void io_task_probe_report_iter_fn(modality_probe* probe)
{
    size_t err;
    int32_t status;
    size_t report_size_bytes = 0;
    int32_t num_bytes = 0;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    err = modality_probe_report(probe, &g_io_buffer[0], MPT_CFG_IO_TASK_BUFFER_SIZE, &report_size_bytes);

    TRACE_EXIT_CRITICAL_SECTION();

    TRACE_ASSERT(err == 0, "Failed to generate Modality probe report", MPT_UNUSED);

    if(report_size_bytes != 0)
    {
        status = TRACE_IO_WRITE(&g_io_buffer[0], (uint32_t) report_size_bytes, &num_bytes);
        if(status != 0)
        {
            prvTraceError("TRACE_IO_WRITE returned an error (1)");
        }

        if(num_bytes != (int32_t) report_size_bytes)
        {
            TRACE_DEBUG_PRINTF(("TRACE_IO_WRITE sent partial report buffer\n"));
        }
    }
}

static void io_task_probe_mutator_announcement_iter_fn(modality_probe* probe)
{
    size_t err;
    int32_t status;
    size_t anncmnt_size_bytes = 0;
    int32_t num_bytes = 0;
    TRACE_ALLOC_CRITICAL_SECTION();

    TRACE_ENTER_CRITICAL_SECTION();

    err = modality_probe_announce_mutators(probe, &g_io_buffer[0], MPT_CFG_IO_TASK_BUFFER_SIZE, &anncmnt_size_bytes);

    TRACE_EXIT_CRITICAL_SECTION();

    TRACE_ASSERT(err == 0, "Failed to generate Modality mutator announcement", MPT_UNUSED);

    if(anncmnt_size_bytes != 0)
    {
        status = TRACE_IO_WRITE(&g_io_buffer[0], (uint32_t) anncmnt_size_bytes, &num_bytes);
        if(status != 0)
        {
            prvTraceError("TRACE_IO_WRITE returned an error (2)");
        }

        if(num_bytes != (int32_t) anncmnt_size_bytes)
        {
            TRACE_DEBUG_PRINTF(("TRACE_IO_WRITE sent partial mutator announcement buffer\n"));
        }
    }
}
#endif /* defined(MPT_CFG_INCLUDE_IO_TASK) && (MPT_CFG_INCLUDE_IO_TASK == 1) */

/* Modality probe API wrappers */
#if defined(MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS) && (MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS == 1)

#define MPT_WP_API_WRAPPER(fn_suffix, type) \
    size_t MPT_CAT2(mpt_, fn_suffix)(modality_probe *probe, uint32_t event_id, type payload) \
    { \
        TRACE_ALLOC_CRITICAL_SECTION(); \
        TRACE_ENTER_CRITICAL_SECTION(); \
        const size_t err = MPT_CAT2(modality_probe_, fn_suffix)(probe, event_id, payload); \
        TRACE_EXIT_CRITICAL_SECTION(); \
        return err; \
    }

#define MPT_WP_WT_API_WRAPPER(fn_suffix, type) \
    size_t MPT_CAT2(mpt_, fn_suffix)(modality_probe *probe, uint32_t event_id, type payload, uint64_t time_ns) \
    { \
        TRACE_ALLOC_CRITICAL_SECTION(); \
        TRACE_ENTER_CRITICAL_SECTION(); \
        const size_t err = MPT_CAT2(modality_probe_, fn_suffix)(probe, event_id, payload, time_ns); \
        TRACE_EXIT_CRITICAL_SECTION(); \
        return err; \
    }

size_t mpt_record_event(modality_probe *probe, uint32_t event_id)
{
    TRACE_ALLOC_CRITICAL_SECTION();
    TRACE_ENTER_CRITICAL_SECTION();
    const size_t err = modality_probe_record_event(probe, event_id);
    TRACE_EXIT_CRITICAL_SECTION();
    return err;
}
size_t mpt_record_event_with_time(modality_probe *probe, uint32_t event_id, uint64_t time_ns)
{
    TRACE_ALLOC_CRITICAL_SECTION();
    TRACE_ENTER_CRITICAL_SECTION();
    const size_t err = modality_probe_record_event_with_time(probe, event_id, time_ns);
    TRACE_EXIT_CRITICAL_SECTION();
    return err;
}

MPT_WP_API_WRAPPER(record_event_with_payload_i8, int8_t)
MPT_WP_API_WRAPPER(record_event_with_payload_u8, uint8_t)
MPT_WP_API_WRAPPER(record_event_with_payload_i16, int16_t)
MPT_WP_API_WRAPPER(record_event_with_payload_u16, uint16_t)
MPT_WP_API_WRAPPER(record_event_with_payload_i32, int32_t)
MPT_WP_API_WRAPPER(record_event_with_payload_u32, uint32_t)
MPT_WP_API_WRAPPER(record_event_with_payload_bool, bool)
MPT_WP_API_WRAPPER(record_event_with_payload_f32, float)

MPT_WP_WT_API_WRAPPER(record_event_with_payload_i8_with_time, int8_t)
MPT_WP_WT_API_WRAPPER(record_event_with_payload_u8_with_time, uint8_t)
MPT_WP_WT_API_WRAPPER(record_event_with_payload_i16_with_time, int16_t)
MPT_WP_WT_API_WRAPPER(record_event_with_payload_u16_with_time, uint16_t)
MPT_WP_WT_API_WRAPPER(record_event_with_payload_i32_with_time, int32_t)
MPT_WP_WT_API_WRAPPER(record_event_with_payload_u32_with_time, uint32_t)
MPT_WP_WT_API_WRAPPER(record_event_with_payload_bool_with_time, bool)
MPT_WP_WT_API_WRAPPER(record_event_with_payload_f32_with_time, float)

#endif /* defined(MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS) && (MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS == 1) */

#endif /* defined(MPT_TRACING_ENABLED) && (MPT_TRACING_ENABLED == 1) */
