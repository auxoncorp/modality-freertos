# Modality probe FreeRTOS tracing backend

## Getting Started

1. Update build script
  ```make
  INCLUDE_DIRS    += -I/path/to/modality-freertos/source/include

  SOURCE_FILES    += /path/to/modality-freertos/source/modality_probe_trace.c

  LIBRARIES       += libmodality_probe.a
  ```
2. Update `FreeRTOSConfig.h`
  ```c
  #define configUSE_TRACE_FACILITY 1

  /* Placed at the bottom, after all other configuration definitions */
  #include "modality_probe_trace.h"
  ```
3. Create a `modality_probe_config.h` configuration file, see below for examples
4. Define an IO interface
  ```c
  /* modality_probe_io.h */

  /* Optional */
  #define TRACE_IO_INIT() my_trace_io_init()

  #define TRACE_IO_READ(_data, _size, _bytes_read) my_trace_io_read(_data, _size, _bytes_read)

  #define TRACE_IO_WRITE(_data, _size, _bytes_written) my_trace_io_write(_data, _size, _bytes_written)
```
5. Initialize tracing before starting the scheduler
  ```c
  int main(void)
  {
      vTraceEnable(0);

      /* Optionally filter out tasks from being traced */
      vTraceExcludeTask("IDLE");

      /* ... application setup ... */

      vTaskStartScheduler();
  }
  ```

## Examples

The examples assume the Modality tar package has been installed in `/usr/local/modality`,
see the CMake option `MODALITY_PROBE_ROOT`.

### m3-qemu

This is the `CORTEX_M3_MPS2_QEMU_GCC` example with networking from `FreeRTOS_Plus_TCP_Echo_Qemu_mps2` added for trace IO.

```
Host IP address (qemu-net):   192.0.2.2
FreeRTOS IP address:          192.0.2.80
FreeRTOS Subnet mask:         255.255.255.0
FreeRTOS Mac address:         52:54:00:12:34:AD
FreeRTOS control plane port:  34320
Modality collector port:      2718
Modality collector address:   192.0.2.2
```

```bash
# In terminal #1
# Temporarily adds qemu-net interface, with IP address 192.0.2.2
sudo ./setup-networking.sh
```

```bash
# In terminal #2
cd examples/m3-qemu

# Create the 'm3-qemu' SUT and open a session named 'qemu' (can run ./m3-qemu-sut-up.sh to do the following)
modality sut create .
modality sut use m3-qemu
modality session open qemu m3-qemu
modality session use qemu

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
make

# Press CTRL+C to quit
make simulate

# View the trace log
modality log
```

## Configuration File

Applications must provide a `modality_probe_config.h` header file - in which the parameters described
in this section can be defined.

```c
/* Example modality_probe_config.h */

#ifndef MODALITY_PROBE_CONFIG_H
#define MODALITY_PROBE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define MPT_HAS_DEBUG_PRINTF 1
#if defined(MPT_HAS_DEBUG_PRINTF) && (MPT_HAS_DEBUG_PRINTF == 1)
    extern void vLoggingPrintf( const char *pcFormatString, ... );
    #define TRACE_DEBUG_PRINTF(x) vLoggingPrintf x
#endif

#define MPT_CFG_STATIC_ALLOCATION 0

#define MPT_CFG_TASK_PROBE_SIZE 1024

#define MPT_CFG_MAX_PROBES 5

#define MPT_CFG_MAX_EXCLUDED_TASKS 2

#define MPT_CFG_USE_TRACE_ASSERT 1

#define MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG 1

#define MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS 1

#define MPT_CFG_INCLUDE_IO_TASK 1

#define MPT_CFG_IO_TASK_NAME "ModalityProbeIo"

#define MPT_CFG_IO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

#define MPT_CFG_IO_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

#define MPT_CFG_IO_TASK_DELAY 10

#define MPT_CFG_IO_TASK_STARTUP_DELAY (1000 / portTICK_PERIOD_MS)

#define MPT_CFG_IO_TASK_ITERS_PER_MUTATOR_ANNOUNCEMENT 200

#define MPT_CFG_IO_TASK_BUFFER_SIZE 256

#define MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER 1

#define MPT_CFG_CONTROL_PLANE_BUFFER_SIZE 512
#define MPT_CFG_CONTROL_PLANE_TASK_BUFFER_SIZE 128

#define MPT_CFG_ALLOC_CRITICAL_SECTION()
#define MPT_CFG_ENTER_CRITICAL_SECTION() portENTER_CRITICAL()
#define MPT_CFG_EXIT_CRITICAL_SECTION() portEXIT_CRITICAL()

#ifdef __cplusplus
}
#endif

#endif /* MODALITY_PROBE_CONFIG_H */
```

* Debug Settings
  - `MPT_HAS_DEBUG_PRINTF` and `TRACE_DEBUG_PRINTF`

    The tracing implementation outputs debugging messages by calling the `TRACE_DEBUG_PRINTF()` macro.
    To obtain debugging messages set `MPT_HAS_DEBUG_PRINTF = 1`, then define `TRACE_DEBUG_PRINTF()`
    to a function that takes a `printf()` style format string and variable number of inputs,
    and sends the formatted messages to an output.
    * Send debug messages to `vLoggingPrintf()`
        ```c
        #define MPT_HAS_DEBUG_PRINTF 1
        #if defined(MPT_HAS_DEBUG_PRINTF) && (MPT_HAS_DEBUG_PRINTF == 1)
            extern void vLoggingPrintf( const char *pcFormatString, ... );
            #define TRACE_DEBUG_PRINTF(x) vLoggingPrintf x
        #endif
        ```
    * Debug messages disabled
        ```c
        #define MPT_HAS_DEBUG_PRINTF 0
        ```
  - `MPT_CFG_USE_TRACE_ASSERT`

    Enable the internal tracing asserts by setting `MPT_CFG_USE_TRACE_ASSERT = 1`.
    It is recommened to have this enabled during development and integration for error checking.
    When an assert is triggered, `vTraceStop()` is called and the error message can be retrieved by calling `pcTraceGetError()`.
  - Tracing can be disabled globally by setting `configUSE_TRACE_FACILITY == 0` in `FreeRTOSConfig.h`.

* Critical Section and Porting
  - `MPT_CFG_ALLOC_CRITICAL_SECTION`, `MPT_CFG_ENTER_CRITICAL_SECTION`, and `MPT_CFG_EXIT_CRITICAL_SECTION`

    Critical section definitions must be defined based on the hardware/port being used.
    * Win32/POSIX port
      ```c
      #define MPT_CFG_ALLOC_CRITICAL_SECTION()
      #define MPT_CFG_ENTER_CRITICAL_SECTION() portENTER_CRITICAL()
      #define MPT_CFG_EXIT_CRITICAL_SECTION() portEXIT_CRITICAL()
      ```
    * ARM Cortex-M port, using the CMSIS API
      ```c
      #define MPT_CFG_ALLOC_CRITICAL_SECTION() uint32_t __irq_status;
      #define MPT_CFG_ENTER_CRITICAL_SECTION() {__irq_status = __get_PRIMASK(); __set_PRIMASK(1);} /* Disable all interrupts */
      #define MPT_CFG_EXIT_CRITICAL_SECTION() {__set_PRIMASK(__irq_status);}
      ```
  - `TRACE_MALLOC` and `TRACE_FREE`

    When `MPT_CFG_STATIC_ALLOCATION == 0`, tracing resources will be allocated from the heap.
    These are defined by default to use the porting layer in `modality_probe_port.h`.
    ```c
    #ifndef TRACE_MALLOC
    #define TRACE_MALLOC(size) pvPortMalloc(size)
    #endif

    #ifndef TRACE_FREE
    #define TRACE_FREE(ptr) vPortFree(ptr)
    #endif
    ```

* Modality Probe and Trace Settings
  - `MPT_CFG_STATIC_ALLOCATION`

    When `MPT_CFG_STATIC_ALLOCATION = 0` Modality probe and tracing resources are allocated
    statically.
    Otherwise resources are allocated using `TRACE_MALLOC()`, and free'd using `TRACE_FREE()
  - `MPT_CFG_MAX_PROBES`

    The maximun number of Modality probes available to the system.
    Unless excluded with `vTraceExcludeTask()`, each task will be allocated a Modality probe.
  - `MPT_CFG_TASK_PROBE_SIZE`

    The size, in bytes, of each probe's log storage buffer.
    When `MPT_CFG_STATIC_ALLOCATION == 1`, probe buffers are allocated from
    a single global byte array, see `MPT_CFG_STORAGE_SIZE`.
    Otherwise buffers are allocated using `TRACE_MALLOC()`, and free'd using `TRACE_FREE()`.
  - `MPT_CFG_STORAGE_SIZE`

    The size, in bytes, of the global probe log storage buffer.
    Only used when `MPT_CFG_STATIC_ALLOCATION == 1`.
  - `MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG`

    When `MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG == 1`, each task's probe pointer will be placed
    in the TCB's application tag for use by the application.
    ```c
    /* MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG == 1 */
    /* MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS == 1 */
    static void task_fn(void* params)
    {
        /* ... */

        modality_probe* probe = (modality_probe*) xTaskGetApplicationTaskTag(xTaskGetCurrentTaskHandle());

        /* ... */

        size_t err = MODALITY_PROBE_RECORD(probe, SOME_APPLICATION_EVENT);
        configASSERT(err == 0);
    }
    ```
  - `MPT_CFG_MAX_EXCLUDED_TASKS`

    The maximun number of entries that can be stored in the task exclude list for
    task-level initialization filtering.
    See `vTraceExcludeTask()`.
  - `MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS`
    When `MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS == 1`, all of the Modality probe recording
    macros normally defined in `modality/probe.h` will get redefined to use a task critical
    section.
    This is useful when using the probes at the task lever for application events, see `MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG`.
    ```c
    /* MPT_CFG_PLACE_PROBE_IN_TCB_APP_TAG == 1 */
    /* MPT_CFG_INCLUDE_USER_LEVEL_PROBE_MACROS == 1 */
    static void task_fn(void* params)
    {
        /* ... */

        modality_probe* probe = (modality_probe*) xTaskGetApplicationTaskTag(xTaskGetCurrentTaskHandle());

        /* ... */

        size_t err = MODALITY_PROBE_RECORD(probe, SOME_APPLICATION_EVENT);
        configASSERT(err == 0);
    }
    ```

* Constants Affecting the IO Task Execution Behavior
  - `MPT_CFG_INCLUDE_IO_TASK`

    When `MPT_CFG_INCLUDE_IO_TASK == 1`, an internal task will be created to manage
    the IO for the probes. The IO task is responsible for getting reports and mutator announcements
    sent out to a Modality collector (or file for later processing) via `TRACE_IO_WRITE()`, and
    optionally getting control plane messages in via `TRACE_IO_READ()`.

    When IO task is enabled, users must define `TRACE_IO_READ()`, `TRACE_IO_WRITE()`, and optionally `TRACE_IO_INIT()`
    inside a header file name `modality_probe_io.h`.
  - `MPT_CFG_IO_TASK_NAME`

    The name of the internal IO task.
    Used when `MPT_CFG_INCLUDE_IO_TASK == 1`.
  - `MPT_CFG_IO_TASK_PRIORITY`

    The priority of the internal IO task.
    Used when `MPT_CFG_INCLUDE_IO_TASK == 1`.
  - `MPT_CFG_IO_TASK_STACK_SIZE`

    The stack size of the internal IO task.
    Used when `MPT_CFG_INCLUDE_IO_TASK == 1`.
  - `MPT_CFG_IO_TASK_DELAY`

    The number of ticks used to `vTaskDelay()` in the IO task loop.
    Used when `MPT_CFG_INCLUDE_IO_TASK == 1`.
  - `MPT_CFG_IO_TASK_STARTUP_DELAY`

    When defined, the number of ticks used to `vTaskDelay()` in the IO task loop before it begins its loop.
    Used when `MPT_CFG_INCLUDE_IO_TASK == 1`.
  - `MPT_CFG_IO_TASK_ITERS_PER_MUTATOR_ANNOUNCEMENT`

    The number of iterations in the IO task before reporting mutator announcements.
    Mutator announcements only need to be reported once, but doing it occasionally is recommended
    if the IO transport is lossy.
    Used when `MPT_CFG_INCLUDE_IO_TASK == 1`.
  - `MPT_CFG_IO_TASK_BUFFER_SIZE`

    The size, in bytes, of the internal buffer used by the IO task for
    reports, mutator announcements, and control plane buffering.
    Used when `MPT_CFG_INCLUDE_IO_TASK == 1`.

* Control Plane Buffer Settings
  - `MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER`

    When `MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1`, each task will be allocated a message buffer
    to store control plane messages destined to that task's probe (e.g. for mutations).

    Tasks may get the `MessageBufferHandle_t` by calling `pvTraceGetControlPlaneBuffer()` for
    manually processing or `vTraceProcessControlPlaneMessages()` to automatically process
    control messages.
  - `MPT_CFG_CONTROL_PLANE_BUFFER_SIZE`

    The size, in bytes, of each task's message buffer.
    Used when `MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1`.
  - `MPT_CFG_CONTROL_PLANE_TASK_BUFFER_SIZE`

    The size, in bytes, of each task's local control plane buffer to receive message buffer bytes in for
    processing.
    Used when `MPT_CFG_INCLUDE_CONTROL_PLANE_BUFFER == 1`.
