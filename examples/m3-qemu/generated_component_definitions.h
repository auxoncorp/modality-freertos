/*
 * GENERATED CODE, DO NOT EDIT
 *
 * Component:
 *   Name: modality-component
 *   ID: c555a326-11b0-427f-879e-175788120410
 *   Code hash: 8f9f27e393c38b6c795b42048777ba9415cb293ead1d4c94251b8b6c5a037d8c
 *   Instrumentation hash: 8a56e56709477e7ad436d0d4f91ef4fc7cc473e5cab0585024b45d1e4f9b3664
 */

#ifndef MODALITY_PROBE_GENERATED_IDENTIFIERS_H
#define MODALITY_PROBE_GENERATED_IDENTIFIERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Probes (sha3-256 1149f4f66038d87051052644dee0f5f8a65dca559e739669c6772ea5df98946d)
 */

/*
 * Name: IP_TASK
 * Description: FreeRTOS task 'IP-task'
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Location: :
 */
#define IP_TASK (64172UL)

/*
 * Name: RX
 * Description: FreeRTOS task 'Rx'
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Location: :
 */
#define RX (35405UL)

/*
 * Name: TX
 * Description: FreeRTOS task 'TX'
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Location: :
 */
#define TX (1161UL)

/*
 * Name: IDLE
 * Description: FreeRTOS task 'IDLE'
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Location: :
 */
#define IDLE (34950UL)

/*
 * Name: TMR_SVC
 * Description: FreeRTOS task 'Tmr Svc'
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Location: :
 */
#define TMR_SVC (23595UL)

/*
 * Name: EMAC
 * Description: FreeRTOS task 'EMAC'
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Location: :
 */
#define EMAC (20415UL)

/*
 * Events (sha3-256 b718303a936198f50a46d08bfcfaf5cfe8c0d00796cdcb3a70f57f0ae7670cb5)
 */

/*
 * Name: TASK_CREATE
 * Description: FreeRTOS task create
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Payload type: u32
 * Location: :
 */
#define TASK_CREATE (65534UL)

/*
 * Name: TASK_SWITCHED_IN
 * Description: FreeRTOS task switched in
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Payload type: u32
 * Location: :
 */
#define TASK_SWITCHED_IN (65533UL)

/*
 * Name: TASK_SWITCHED_OUT
 * Description: FreeRTOS task switched out
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Payload type:
 * Location: :
 */
#define TASK_SWITCHED_OUT (65532UL)

/*
 * Name: QUEUE_SEND
 * Description: FreeRTOS queue send
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND (65531UL)

/*
 * Name: QUEUE_SEND_BACK_FAILED
 * Description: FreeRTOS queue send back failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_BACK_FAILED (65530UL)

/*
 * Name: QUEUE_SEND_FRONT_FAILED
 * Description: FreeRTOS queue send front failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FRONT_FAILED (65529UL)

/*
 * Name: QUEUE_RECEIVE
 * Description: FreeRTOS queue receive
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_RECEIVE (65528UL)

/*
 * Name: QUEUE_RECEIVE_FAILED
 * Description: FreeRTOS queue receive failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_RECEIVE_FAILED (65527UL)

/*
 * Name: NETWORK_UP
 * Description: Network up
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: app;network
 * Payload type:
 * Location: m3-qemu/main.c:238
 */
#define NETWORK_UP (65536UL)

/*
 * Name: NETWORK_DOWN
 * Description: Network down
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;app;network
 * Payload type:
 * Location: m3-qemu/main.c:249
 */
#define NETWORK_DOWN (65537UL)

/*
 * Name: RX_MSG
 * Description: Received message
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: EXPECTATION;SEVERITY_10;app;ipc
 * Payload type: u32
 * Location: m3-qemu/main_blinky.c:147
 */
#define RX_MSG (65538UL)

/*
 * Name: BLINKING
 * Description: Blinking
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: app;ipc
 * Payload type: u32
 * Location: m3-qemu/main_blinky.c:165
 */
#define BLINKING (65539UL)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODALITY_PROBE_GENERATED_IDENTIFIERS_H */
