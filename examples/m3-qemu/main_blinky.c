/*
 * FreeRTOS V202107.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <stdio.h>

/* Generated application event definitions */
#include "generated_component_definitions.h"

static void prvQueueReceiveTask( void * pvParameters );
static void prvQueueSendTask( void * pvParameters );
static void prvQueueSendTimerCallback( TimerHandle_t xTimerHdl );

#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_LENGTH                   ( 1 )
#define mainQUEUE_SEND_FREQUENCY_MS        ( 200 / portTICK_PERIOD_MS )
#define mainTIMER_SEND_FREQUENCY_MS        ( 200 / portTICK_PERIOD_MS )

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

static TimerHandle_t xTimer = NULL;

void main_blinky( void )
{
    const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS;

    /* Create the queue. */
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

    if( xQueue != NULL )
    {
        /* Start the two tasks as described in the comments at the top of this
         * file. */
        xTaskCreate( prvQueueReceiveTask,             /* The function that implements the task. */
                     "Rx",                            /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                     configMINIMAL_STACK_SIZE,        /* The size of the stack to allocate to the task. */
                     NULL,                            /* The parameter passed to the task - not used in this case. */
                     mainQUEUE_RECEIVE_TASK_PRIORITY, /* The priority assigned to the task. */
                     NULL );                          /* The task handle is not required, so NULL is passed. */

        xTaskCreate( prvQueueSendTask,
                     "TX",
                     configMINIMAL_STACK_SIZE,
                     NULL,
                     mainQUEUE_SEND_TASK_PRIORITY,
                     NULL );

        xTimer = xTimerCreate( "Timer",
                               xTimerPeriod,                /* The period of the software timer in ticks. */
                               pdTRUE,                      /* xAutoReload is set to pdTRUE. */
                               NULL,                        /* The timer's ID is not used. */
                               prvQueueSendTimerCallback ); /* The function executed when the timer expires. */

        if( xTimer != NULL )
        {
            xTimerStart( xTimer, 0 );
        }

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the Idle and/or
     * timer tasks to be created.  See the memory management section on the
     * FreeRTOS web site for more details on the FreeRTOS heap
     * http://www.freertos.org/a00111.html. */
    for( ; ; )
    {
    }
}

static void prvQueueSendTask( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const uint32_t ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ; ; )
    {
        /* Place this task in the blocked state until it is time to run again. */
        vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

        /* Send to the queue - causing the queue receive task to unblock and
         * toggle the LED.  0 is used as the block time so the sending operation
         * will not block - it shouldn't need to block as the queue should always
         * be empty at this point in the code. */
        xQueueSend( xQueue, &ulValueToSend, 0U );
    }
}
/*-----------------------------------------------------------*/

volatile uint32_t ulRxEvents = 0;
static void prvQueueReceiveTask( void * pvParameters )
{
    size_t err;
    uint32_t ulReceivedValue;
    const uint32_t ulExpectedValue = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    modality_probe* probe = (modality_probe*) xTaskGetApplicationTaskTag(xTaskGetCurrentTaskHandle());
    configASSERT(probe != NULL);

    for( ; ; )
    {
        /* Wait until something arrives in the queue - this task will block
         * indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
         * FreeRTOSConfig.h. */
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

        err = MODALITY_PROBE_EXPECT(
                probe,
                RX_MSG,
                (ulReceivedValue == ulExpectedValue),
                MODALITY_SEVERITY(10),
                MODALITY_TAGS("app", "ipc"),
                "Received message");
        configASSERT(err == MODALITY_PROBE_ERROR_OK);

        /*  To get here something must have been received from the queue, but
         * is it the expected value?  If it is, toggle the LED. */
        if( ulReceivedValue == ulExpectedValue )
        {
            printf( "%s\n", "blinking" );
            vTaskDelay( 1000 );
            ulReceivedValue = 0U;
            ulRxEvents++;

            err = MODALITY_PROBE_RECORD_W_U32(
                    probe,
                    BLINKING,
                    ulRxEvents,
                    MODALITY_TAGS("app", "ipc"),
                    "Blinking");
            configASSERT(err == MODALITY_PROBE_ERROR_OK);
        }
    }
}
/*-----------------------------------------------------------*/

static void prvQueueSendTimerCallback( TimerHandle_t xTimerHdl )
{
    const uint32_t ulValueToSend = 1;

    /* Avoid compiler warnings resulting from the unused parameter. */
    ( void ) xTimerHdl;

    /* Send to the queue - causing the queue receive task to unblock and
     * write out a message.  This function is called from the timer/daemon task, so
     * must not block.  Hence the block time is set to 0. */
    xQueueSend( xQueue, &ulValueToSend, 0U );
}
/*-----------------------------------------------------------*/
