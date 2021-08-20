#include "FreeRTOS.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <modality/probe.h>

#include "util.h"

#ifndef TEST_NAME
#error "TEST_NAME not defined"
#endif

#if (MPT_CFG_MAX_PROBES != 4)
#error "Expecting MPT_CFG_MAX_PROBES == 4"
#endif

const uint32_t USER_EVENT_ID_A = TRACE_EVENT_ID_TOP + 1;
const uint32_t USER_EVENT_ID_B = TRACE_EVENT_ID_TOP + 2;
const uint32_t USER_EVENT_ID_C = TRACE_EVENT_ID_TOP + 3;
const uint32_t USER_EVENT_ID_D = TRACE_EVENT_ID_TOP + 4;
const uint32_t USER_EVENTS[MPT_CFG_MAX_PROBES] =
{
    USER_EVENT_ID_A,
    USER_EVENT_ID_B,
    USER_EVENT_ID_C,
    USER_EVENT_ID_D,
};

const char* const TASK_NAMES[MPT_CFG_MAX_PROBES] =
{
    "A",
    "B",
    "C",
    "D",
};

/* CRC-16/IBM-3740 of the TASK_NAMES */
const uint16_t TASK_PROBE_IDS[MPT_CFG_MAX_PROBES] =
{
    0xB915,
    0x8976,
    0x9957,
    0xE9B0,
};

static const char LOGFILE[] = TEST_NAME "_tracelogs.bin";
static uint8_t g_report_buffer[MPT_CFG_STORAGE_SIZE];

void vLoggingPrintf( const char *pcFormatString, ... )
{
    va_list args;
    va_start(args, pcFormatString);
    vprintf(pcFormatString, args);
    va_end(args);
}

void util_write_binary_log_to_file_iter_fn(modality_probe* probe)
{
    size_t bytes_written = 0;
    const size_t err = modality_probe_report(probe, &g_report_buffer[0], sizeof(g_report_buffer), &bytes_written);
    assert(err == 0);

    if(bytes_written != 0)
    {
        FILE* file = fopen(LOGFILE, "ab+");
        assert(file != NULL);

        const size_t ret = fwrite(&g_report_buffer[0], 1, bytes_written, file);
        assert(ret == bytes_written);

        const int status = fclose(file);
        assert(status == 0);
    }
}
