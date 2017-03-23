/*
 * serial.c
 *
 *  Created on: Mar 22, 2017
 *      Author: faebsn
 */

#include <DAVE.h>
#include "daisy_wrapper.h"
#include "global_definition.h"

static uint8_t daisy_tx_buf[0xff];
static daisy_command_t command = {.command = CMD_UNDEFINDED,
		.payload = 0,
		.sender_id = MY_ID
};

void min_frame_received(uint8_t buf[], uint8_t control, uint8_t id) {
	memcpy(&command,buf,control);
	if(id == MY_ID){
		switch (command.command) {
		case CMD_READY:
			daisy_ready_received(command.sender_id);
			break;
		case CMD_START:
			daisy_start_received();
			break;
		}
	} else {
		min_tx_frame(id,buf,control);
	}
}

void daisy_transmit_buffer(uint8_t id, uint8_t buf[], uint8_t control) {
	memcpy(daisy_tx_buf,buf,control);
	min_tx_frame(id,daisy_tx_buf,control);
}

// should be called in a loop
void daisy_rx_polling() {
	if(!UART_IsRXFIFOEmpty(&DAISY)) {
		uint8_t byte = XMC_UART_CH_GetReceivedData(DAISY.channel);
		min_rx_byte(byte);
	}
}

// always returns 0xff for now, buffer should be sufficient
uint8_t min_tx_space(void) {
	return 0xff;
}

void daisy_send_ready() {
	command.command = CMD_READY;
	min_tx_frame(ID_MASTER,(uint8_t*)&command,sizeof(command));
}

void daisy_send_start(uint8_t id) {
	command.command = CMD_START;
	command.sender_id = MY_ID;
	min_tx_frame(id,(uint8_t*)&command,sizeof(command));
}

void min_tx_byte(uint8_t byte) {
	UART_Transmit(&DAISY,&byte,1);
}

