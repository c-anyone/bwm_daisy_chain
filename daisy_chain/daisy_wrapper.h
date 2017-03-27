/*
 * daisy_wrapper.h
 *
 *  Created on: Mar 22, 2017
 *      Author: Fabio Pungg
 */

#include <DAVE.h>
#include <stdbool.h>
#include "min.h"

#ifndef DAISY_WRAPPER_H_
#define DAISY_WRAPPER_H_

#define ID_MASTER		(0x01u)
#define ID_SLAVE_1		(0x02u)
#define ID_SLAVE_2		(0x03u)
#define ID_SLAVE_3		(0x04u)
#define MACHINE_COUNT	(0x04u)

// id definition for this device
// might be moved to another file (main.c)
#define MY_ID ID_MASTER

// commands to communicate with daisy devices
#define CMD_UNDEFINDED (0x00u)
#define CMD_START	   (0x01u)
#define CMD_READY	   (0x02u)
#define CMD_BUSY	   (0x03u)

// type for communication, payload is a placeholder, might be needed
// for setting motors to specific speeds et al
typedef struct {
	uint8_t command;
	uint8_t sender_id;
	uint16_t payload;
} daisy_command_t;

typedef struct {
	bool init;
	bool ready;

} machine_status_t;

uint8_t daisy_rx_buf[0xff];

// in case still undefined data has to be sent
void daisy_transmit_buffer(uint8_t id, uint8_t buf[], uint8_t control);

// should be called in a loop to receive data from UART
void daisy_rx_polling(void);

void daisy_received_cmd(uint8_t cmd);

// send ready always addressed to master, as the master keeps global state
void daisy_send_ready(void);
// send start should only be used from master, as it keeps global state
void daisy_send_start(uint8_t id);

// to be implemented in main or state machine handler
extern void daisy_ready_received(uint8_t id);
extern void daisy_busy_received(uint8_t id);
extern void daisy_undefined_command(daisy_command_t);

extern void daisy_start_received(void);

#endif /* DAISY_WRAPPER_H_ */
