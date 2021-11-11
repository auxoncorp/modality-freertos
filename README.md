# Modality probe FreeRTOS tracing backend

## Getting Started

1. Install Modality (the examples assume the tarball was extracted to `/usr/local/modality`)
2. Update build script
  ```make
  INCLUDE_DIRS    += -I/path/to/modality-freertos/source/include

  SOURCE_FILES    += /path/to/modality-freertos/source/modality_probe_trace.c
  ```
3. Update `FreeRTOSConfig.h`
  ```c
  #define configUSE_TRACE_FACILITY 1

  /* Placed at the bottom, after all other configuration definitions */
  #include "modality_probe_trace.h"
  ```
4. Create a `modality_probe_config.h` configuration file, see below for examples
5. Define an IO interface
  ```c
  /* modality_probe_io.h */

  /* Optional */
  #define TRACE_IO_INIT() my_trace_io_init()

  /* Optional: required for control-plane data in */
  #define TRACE_IO_READ(_data, _size, _bytes_read) my_trace_io_read(_data, _size, _bytes_read)

  #define TRACE_IO_WRITE(_data, _size, _bytes_written) my_trace_io_write(_data, _size, _bytes_written)
```
6. Initialize tracing before starting the scheduler
  ```c
  int main(void)
  {
      vTraceEnable();

      /* Optionally filter out tasks from being traced */
      vTraceExcludeTask("IDLE");

      /* ... application setup ... */

      vTaskStartScheduler();
  }
  ```

## Example

The example assume the Modality tar package has been installed in `/usr/local/modality`,
see the CMake option `MODALITY_PROBE_ROOT`.

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

1. Run the networking setup script in a separate terminal to proxy networking from qemu.

    ```bash
    # Temporarily adds qemu-net interface, with IP address 192.0.2.2
    cd examples/m3-qemu
    sudo ./setup-networking.sh
    ```

2. Create the example SUT and open a [session](https://docs.auxon.io/modality/reference/glossary.html#session).

    ```bash
    cd examples/m3-qemu
    ./m3-qemu-sut-up.sh
    ```

3. Build the modality-freertos example.

    ```bash
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
    make
    ```

4. Run the example.

    ```bash
    # Press CTRL+C to quit
    make simulate
    ```

    ```text
    Creating ModalityProbeIo task
    0x20005580->prvMiscInitialisation 187: Seed for randomiser: 32
    0x20005580->prvMiscInitialisation 189: Random numbers: 0000094A 00003B00 00005631 000007FE
    0x20005580->main 112: FreeRTOS_IPInit
    Registering probe ID 64172 for task 'IP-task'
    Registering probe ID 35405 for task 'Rx'
    Registering probe ID 1161 for task 'TX'
    Registering probe ID 34950 for task 'IDLE'
    Registering probe ID 23595 for task 'Tmr Svc'
    0x2000f4e8->prvIPTask 402: prvIPTask started
    Registering probe ID 20415 for task 'EMAC'
    0x20005580->init_recvr 77: Created Modality trace IO receiver socket
    blinking
    blinking
    0x2000f4e8->vApplicationIPNetworkEventHook 227:

    IP Address: 192.0.2.80
    0x2000f4e8->vApplicationIPNetworkEventHook 230: Subnet Mask: 255.255.255.0
    0x2000f4e8->vApplicationIPNetworkEventHook 233: Gateway Address: 192.0.2.254
    0x2000f4e8->vApplicationIPNetworkEventHook 236: DNS Server Address: 192.0.2.254


    0x20005580->init_sender 41: Created Modality trace IO sender socket
    blinking
    blinking
    blinking
    blinking
    blinking
    ```

5. View the trace log.

    ```bash
    modality log
    ```

    ```text
    ║  *  ║  ║  ║  ║  ║  (1161:1:0:1, TASK_CREATE @ TX, payload=1)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  *  ║  ║  ║  ║  (20415:1:0:1, TASK_CREATE @ EMAC, payload=7)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:1:0:1, TASK_CREATE @ TMR_SVC, payload=9)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  *  ║  ║  (34950:1:0:1, TASK_CREATE @ IDLE, payload=0)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  *  ║  (35405:1:0:1, TASK_CREATE @ RX, payload=2)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  ║  *  (64172:1:0:1, TASK_CREATE @ IP_TASK, payload=8)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:1:0:2, TASK_SWITCHED_IN @ TMR_SVC, payload=9)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:1:0:3, TASK_SWITCHED_OUT @ TMR_SVC)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:2:0:1, TASK_SWITCHED_IN @ TMR_SVC, payload=9)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:2:0:2, QUEUE_RECEIVE @ TMR_SVC, payload=1)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:2:0:3, QUEUE_RECEIVE_FAILED @ TMR_SVC, outcome=FAIL)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:2:0:4, TASK_DELAY_UNTIL @ TMR_SVC, payload=200)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  *  ║  ║  ║  (23595:2:0:5, TASK_SWITCHED_OUT @ TMR_SVC)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ╚═══════»╗  TMR_SVC interacted with IP_TASK
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  ║  *  (64172:2:0:3, TASK_SWITCHED_IN @ IP_TASK, payload=8)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  ║  *  (64172:2:0:4, QUEUE_SEND @ IP_TASK, payload=0)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  ║  *  (64172:2:0:5, QUEUE_RECEIVE @ IP_TASK, payload=1)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  ║  *  (64172:2:0:6, TASK_DELAY @ IP_TASK, payload=1)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  ║  *  (64172:2:0:7, TASK_SWITCHED_OUT @ IP_TASK)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ╔«══════════╝  IP_TASK interacted with EMAC
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  *  ║  ║  ║  ║  (20415:2:0:3, TASK_SWITCHED_IN @ EMAC, payload=7)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  *  ║  ║  ║  ║  (20415:2:0:4, TASK_SWITCHED_OUT @ EMAC)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ╚═══════»╗  ║  EMAC interacted with RX
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  *  ║  (35405:2:0:3, TASK_SWITCHED_IN @ RX, payload=2)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  *  ║  (35405:2:0:4, QUEUE_RECEIVE_BLOCKED @ RX, payload=0)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  *  ║  (35405:2:0:5, TASK_SWITCHED_OUT @ RX)
    ║  ║  ║  ║  ║  ║  ║
    ║  ║  ║  ║  ║  ╚═»╗  RX interacted with IP_TASK
    .  .  .  .  .  .  .
    .  .  .  .  .  .  .
    .  .  .  .  .  .  .
    ```

## Updating Modality Component Manifests

Run the [tools/update-manifest.py](tools/update-manifest.py) script to
add the FreeRTOS tracing event and task probe definitions to a Modality component.

Probe IDs are generated from a hash of their associated task name.

```bash
./tools/update-manifest.py \
    --component examples/m3-qemu/modality-component \
    --task-names "IP-task" "Rx" "TX" "IDLE" "Tmr Svc" "EMAC"
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
    When an assert is triggered, `vTraceDisable()` is called and the error message can be retrieved by calling `pcTraceGetError()`.
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
    Otherwise resources are allocated using `TRACE_MALLOC()`, and free'd using `TRACE_FREE()`
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

## LICENSE

See [LICENSE](./LICENSE) for more details.

Copyright 2020 [Auxon Corporation](https://auxon.io)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
