/*
 * application_layer.c
 *
 *  Created on: Apr 3, 2017
 *      Author: Fabio Pungg
 */


#include "application_layer.h"
#include "daisy_wrapper.h"

static daisy_command_t command = {.command = CMD_UNDEFINED,
		.payload = 0xdeadbeef,
		.sender_id = MY_ID
};

static daisy_set_callback set_callback = NULL;
static daisy_get_callback get_callback = NULL;
static daisy_start_callback start_callback = NULL;
/*
 * callback function from daisy_wrapper, used when a payload
 * is addressed to this device
 */
void daisy_payload_received(uint8_t* payload, uint8_t length) {
	if(length<sizeof(daisy_command_t)) {
		return; // fail silently here
	}

	daisy_command_t *cmd_p = (daisy_command_t*)payload;
	switch(cmd_p->command) {
	case CMD_UNDEFINED:
		break;
	case CMD_START:
		if(start_callback != NULL) {
			(*start_callback)(cmd_p->param);
		}
		break;
	case CMD_READY:
		break;
	case CMD_BUSY:
		break;
	case CMD_SET_PARAM:
		if(set_callback!=NULL) {
			(*set_callback)(cmd_p->param,cmd_p->payload);
		}
		break;
	case CMD_GET_PARAM:
		if(get_callback!=NULL) {
			(*get_callback)(cmd_p->param);
		}
		break;
	default:
		break;
	}
}

void application_layer_init(void) {
	daisy_min_init();
}

void application_worker(void) {
	daisy_rx_polling();
}

void set_cmd_start_callback(daisy_start_callback function) {
	start_callback = function;
}

void set_cmd_set_callback(daisy_set_callback function) {
	set_callback = function;
}

void set_cmd_get_callback(daisy_get_callback function) {
	get_callback = function;
}

#ifdef MASTER_DEVICE
void signal_start(uint8_t id,uint8_t param) {
	command.command = CMD_START;
	command.sender_id = MY_ID;
	command.param = param;
	daisy_send_payload(id,(uint8_t*)&command,sizeof(daisy_command_t));
}

void signal_set(uint8_t id, uint8_t param, uint32_t value) {
	command.command = CMD_SET_PARAM;
	command.sender_id = MY_ID;
	command.param = param;
	command.payload = value;
	daisy_send_payload(id,(uint8_t*)&command,sizeof(command));
}
#elif defined SLAVE_DEVICE
void signal_ready(void) {
	command.command = CMD_READY;
	command.sender_id = MY_ID;
	daisy_send_payload(ID_MASTER,(uint8_t*)&command,sizeof(command));
}

void signal_busy(void) {
	command.command = CMD_BUSY;
	command.sender_id = MY_ID;
	daisy_send_payload(ID_MASTER,(uint8_t*)&command,sizeof(command));
}

void signal_status(void) {
	command.command = CMD_STATUS;
	command.sender_id = MY_ID;
	// TODO: add status data here and send
}
#endif

void daisy_ping_received() {
	// kick the dog here or call the dog kicking callback here
}
