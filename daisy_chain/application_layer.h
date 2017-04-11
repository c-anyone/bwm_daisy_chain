/*
 * application_layer.h
 *
 *  Created on: Apr 3, 2017
 *      Author: Fabio Pungg
 */

#ifndef DAISY_CHAIN_APPLICATION_LAYER_H_
#define DAISY_CHAIN_APPLICATION_LAYER_H_

#include <DAVE.h>
#include "daisy_wrapper.h"

#define ID_PING			(0xBBu)
#define ID_MASTER		(0x00u)
#define ID_SLAVE_1		(0x01u)
#define ID_SLAVE_2		(0x02u)
#define ID_SLAVE_3		(0x03u)
#define MACHINE_COUNT	(0x04u)

#define CMD_UNDEFINED		(0x00u)
#define CMD_START	   		(0x01u)
#define CMD_READY			(0x02u)
#define CMD_BUSY			(0x03u)
#define CMD_STATUS			(0x04u)
#define CMD_SET_PARAM		(0x05u)
#define CMD_GET_PARAM		(0x07u)
#define CMD_REQUEST_STATUS	(0x09u)
#define CMD_ACK				(0x0Au)

typedef void(*daisy_set_callback)(uint8_t param, uint32_t payload);
typedef void(*daisy_get_callback)(uint8_t param);
typedef void(*daisy_start_callback)(uint8_t param);
typedef void(*daisy_ack_callback)(uint8_t sender_id);
typedef void(*daisy_request_status_callback)(void);

typedef void(*daisy_busy_callback)(uint8_t sender_id);
typedef void(*daisy_ready_callback)(uint8_t sender_id);
typedef void(*daisy_status_callback)(uint8_t sender_id, uint32_t status);

// type for communication, payload is a placeholder, might be needed
// for setting motors to specific speeds et al
typedef struct {
	uint8_t command;
	uint8_t sender_id;
	uint8_t param;
	uint32_t payload;
} daisy_command_t;

// init this and every layer below
void application_layer_init(void);
//to be called in main loop, calls uart polling from daisy_wrapper
void application_worker(void);

// to be used on slave devices to react to commands
void set_cmd_start_callback(daisy_start_callback);
void set_cmd_set_callback(daisy_set_callback);
void set_cmd_get_callback(daisy_get_callback);
void set_cmd_request_status_callback(daisy_request_status_callback);

// to be used on master device to evaluate
void set_cmd_ack_callback(daisy_ack_callback);
void set_cmd_busy_callback(daisy_busy_callback);
void set_cmd_ready_callback(daisy_ready_callback);
void set_cmd_status_callback(daisy_status_callback);

void signal_set(uint8_t id, uint8_t param, uint32_t value);


/**
 * device role specific model
 */

#ifdef MASTER_DEVICE
// send start should only be used from master, as it keeps global state
void signal_start(uint8_t id, uint8_t param);
void signal_get(uint32_t id,uint32_t param);
void signal_get_status(uint8_t id);
#elif defined SLAVE_DEVICE
// send ready always addressed to master, as the master keeps global state
void signal_ready(void);
// send busy always addressed to master
void signal_busy(void);
void signal_status(void);
void signal_ack(void);
#endif

#endif /* DAISY_CHAIN_APPLICATION_LAYER_H_ */
