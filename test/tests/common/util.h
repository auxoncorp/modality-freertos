#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <modality/probe.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT_TRACE_ERROR() \
    { \
        const char* err = pcTraceGetError(); \
        if(err != NULL) \
        { \
            fprintf(stderr, "***************************************\n"); \
            fprintf(stderr, "%s\n", err); \
            fprintf(stderr, "***************************************\n"); \
        } \
        assert(err == NULL); \
    }

#define ASSERT_CRITICAL_NESTING_DEPTH_ZERO() assert(uxCriticalNesting == 0)

#define MAX_TASKS (255)
#define NUM_SCHEDULER_ITERS (8)
#define NUM_TASK_ITERS (8)
#define MAX_QUEUES MPT_CFG_MAX_PROBES
#define MAX_QUEUE_LENGTH (1)
#define QUEUE_ITEM_SIZE (1)
#define MAX_MSG_BUFFERS MPT_CFG_MAX_PROBES

extern size_t uxCriticalNesting;

extern const uint32_t USER_EVENT_ID_A;
extern const uint32_t USER_EVENT_ID_B;
extern const uint32_t USER_EVENT_ID_C;
extern const uint32_t USER_EVENT_ID_D;
extern const uint32_t USER_EVENTS[MPT_CFG_MAX_PROBES];

extern const char* const TASK_NAMES[MPT_CFG_MAX_PROBES];
extern const uint16_t TASK_PROBE_IDS[MPT_CFG_MAX_PROBES];

void util_write_binary_log_to_file_iter_fn(modality_probe* probe);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */
