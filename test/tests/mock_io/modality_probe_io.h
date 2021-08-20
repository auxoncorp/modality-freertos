#ifndef MODALITY_PROBE_IO_H
#define MODALITY_PROBE_IO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void mock_init(void)
{
}

static inline int32_t mock_read(void* data, uint32_t size, int32_t* bytes_read)
{
    (void) data;
    (void) size;
    (void) bytes_read;
    return 0;
}

static inline int32_t mock_write(void* data, uint32_t size, int32_t* bytes_written)
{
    (void) data;
    (void) size;
    (void) bytes_written;
    return 0;
}

#define TRACE_IO_INIT() mock_init()

#define TRACE_IO_READ(_data, _size, _bytes_read) mock_read(_data, _size, _bytes_read)

#define TRACE_IO_WRITE(_data, _size, _bytes_written) mock_write(_data, _size, _bytes_written)

#ifdef __cplusplus
}
#endif

#endif /* MODALITY_PROBE_IO_H */
