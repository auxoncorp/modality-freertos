/*
 * GENERATED CODE, DO NOT EDIT
 *
 * Component:
 *   Name: modality-component
 *   ID: c555a326-11b0-427f-879e-175788120410
 *   Code hash: 8f9f27e393c38b6c795b42048777ba9415cb293ead1d4c94251b8b6c5a037d8c
 *   Instrumentation hash: 4dea1f976dd8c18a4fe832025da22074958498346c2c57a5180c796c48b3f329
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
 * Events (sha3-256 ca03fdde64ad7f82b4229ff04de91c9d15b7f15f5161cc124cc99f96cadf9f3a)
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
 * Name: TASK_SUSPEND
 * Description: FreeRTOS task suspend
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Payload type: u32
 * Location: :
 */
#define TASK_SUSPEND (65531UL)

/*
 * Name: TASK_DELAY
 * Description: FreeRTOS task delay
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Payload type: u32
 * Location: :
 */
#define TASK_DELAY (65530UL)

/*
 * Name: TASK_DELAY_UNTIL
 * Description: FreeRTOS task delay until
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;task
 * Payload type: u32
 * Location: :
 */
#define TASK_DELAY_UNTIL (65529UL)

/*
 * Name: QUEUE_SEND
 * Description: FreeRTOS queue send
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND (65435UL)

/*
 * Name: QUEUE_SEND_BLOCKED
 * Description: FreeRTOS queue send blocked
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_BLOCKED (65434UL)

/*
 * Name: QUEUE_SEND_FAILED
 * Description: FreeRTOS queue send failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FAILED (65433UL)

/*
 * Name: QUEUE_SEND_FRONT
 * Description: FreeRTOS queue send front
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FRONT (65432UL)

/*
 * Name: QUEUE_SEND_FRONT_BLOCKED
 * Description: FreeRTOS queue send front blocked
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FRONT_BLOCKED (65431UL)

/*
 * Name: QUEUE_SEND_FRONT_FAILED
 * Description: FreeRTOS queue send front failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FRONT_FAILED (65430UL)

/*
 * Name: QUEUE_SEND_FROMISR
 * Description: FreeRTOS queue send fromisr
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue;ISR
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FROMISR (65429UL)

/*
 * Name: QUEUE_SEND_FROMISR_FAILED
 * Description: FreeRTOS queue send fromisr failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue;ISR
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FROMISR_FAILED (65428UL)

/*
 * Name: QUEUE_SEND_FRONT_FROMISR
 * Description: FreeRTOS queue send front fromisr
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue;ISR
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FRONT_FROMISR (65427UL)

/*
 * Name: QUEUE_SEND_FRONT_FROMISR_FAILED
 * Description: FreeRTOS queue send front fromisr failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue;ISR
 * Payload type: u32
 * Location: :
 */
#define QUEUE_SEND_FRONT_FROMISR_FAILED (65426UL)

/*
 * Name: QUEUE_RECEIVE
 * Description: FreeRTOS queue receive
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_RECEIVE (65335UL)

/*
 * Name: QUEUE_RECEIVE_BLOCKED
 * Description: FreeRTOS queue receive blocked
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_RECEIVE_BLOCKED (65334UL)

/*
 * Name: QUEUE_RECEIVE_FAILED
 * Description: FreeRTOS queue receive failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue
 * Payload type: u32
 * Location: :
 */
#define QUEUE_RECEIVE_FAILED (65333UL)

/*
 * Name: QUEUE_RECEIVE_FROMISR
 * Description: FreeRTOS queue receive fromisr
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;queue;ISR
 * Payload type: u32
 * Location: :
 */
#define QUEUE_RECEIVE_FROMISR (65332UL)

/*
 * Name: QUEUE_RECEIVE_FROMISR_FAILED
 * Description: FreeRTOS queue receive fromisr failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;queue;ISR
 * Payload type: u32
 * Location: :
 */
#define QUEUE_RECEIVE_FROMISR_FAILED (65331UL)

/*
 * Name: SEMAPHORE_GIVE
 * Description: FreeRTOS semaphore give
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;semaphore
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_GIVE (65235UL)

/*
 * Name: SEMAPHORE_GIVE_BLOCKED
 * Description: FreeRTOS semaphore give blocked
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;semaphore
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_GIVE_BLOCKED (65234UL)

/*
 * Name: SEMAPHORE_GIVE_FAILED
 * Description: FreeRTOS semaphore give failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;semaphore
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_GIVE_FAILED (65233UL)

/*
 * Name: SEMAPHORE_GIVE_FROMISR
 * Description: FreeRTOS semaphore give fromisr
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;semaphore;ISR
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_GIVE_FROMISR (65232UL)

/*
 * Name: SEMAPHORE_GIVE_FROMISR_FAILED
 * Description: FreeRTOS semaphore give fromisr failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;semaphore;ISR
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_GIVE_FROMISR_FAILED (65231UL)

/*
 * Name: SEMAPHORE_TAKE
 * Description: FreeRTOS semaphore take
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;semaphore
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_TAKE (65135UL)

/*
 * Name: SEMAPHORE_TAKE_BLOCKED
 * Description: FreeRTOS semaphore take blocked
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;semaphore
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_TAKE_BLOCKED (65134UL)

/*
 * Name: SEMAPHORE_TAKE_FAILED
 * Description: FreeRTOS semaphore take failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;semaphore
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_TAKE_FAILED (65133UL)

/*
 * Name: SEMAPHORE_TAKE_FROMISR
 * Description: FreeRTOS semaphore take fromisr
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;semaphore;ISR
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_TAKE_FROMISR (65132UL)

/*
 * Name: SEMAPHORE_TAKE_FROMISR_FAILED
 * Description: FreeRTOS semaphore take fromisr failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;semaphore;ISR
 * Payload type: u32
 * Location: :
 */
#define SEMAPHORE_TAKE_FROMISR_FAILED (65131UL)

/*
 * Name: MUTEX_GIVE
 * Description: FreeRTOS mutex give
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;mutex
 * Payload type: u32
 * Location: :
 */
#define MUTEX_GIVE (65035UL)

/*
 * Name: MUTEX_GIVE_BLOCKED
 * Description: FreeRTOS mutex give blocked
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;mutex
 * Payload type: u32
 * Location: :
 */
#define MUTEX_GIVE_BLOCKED (65034UL)

/*
 * Name: MUTEX_GIVE_FAILED
 * Description: FreeRTOS mutex give failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;mutex
 * Payload type: u32
 * Location: :
 */
#define MUTEX_GIVE_FAILED (65033UL)

/*
 * Name: MUTEX_TAKE
 * Description: FreeRTOS mutex take
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;mutex
 * Payload type: u32
 * Location: :
 */
#define MUTEX_TAKE (64935UL)

/*
 * Name: MUTEX_TAKE_BLOCKED
 * Description: FreeRTOS mutex take blocked
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FreeRTOS;mutex
 * Payload type: u32
 * Location: :
 */
#define MUTEX_TAKE_BLOCKED (64934UL)

/*
 * Name: MUTEX_TAKE_FAILED
 * Description: FreeRTOS mutex take failed
 * Component ID: c555a326-11b0-427f-879e-175788120410
 * Tags: FAILURE;FreeRTOS;mutex
 * Payload type: u32
 * Location: :
 */
#define MUTEX_TAKE_FAILED (64933UL)

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
