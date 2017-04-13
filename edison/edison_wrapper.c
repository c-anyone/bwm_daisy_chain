/*
 * wlan_handler.c
 *
 *  Created on: Mar 23, 2017
 *      Author: Fabio Pungg
 */

#include <DAVE.h>
#include "edison_wrapper.h"
#include "../daisy_chain/global_definitions.h"

uint8_t edison_min_tx_space(void){
	uint8_t  space = 0xFF;
	return space;
}

// should be called in a loop
void edison_rx_polling() {
	while(!UART_IsRXFIFOEmpty(&EDISON)) {
		uint8_t byte = XMC_UART_CH_GetReceivedData(EDISON.channel);
		edison_min_rx_byte(byte);
	}
}

void edison_min_tx_byte(uint8_t byte){
	UART_Transmit(&EDISON, &byte, 1);
}

void edison_min_frame_received(uint8_t buf[], uint8_t control, uint8_t id){
	edison_min_tx_frame(id, buf, control);

	if((id == EDISON_SHOOT || id == EDISON_ELEVATION|| id == EDISON_SPEED) && control == 2) {
		// elevation command, buf is a uint16_t, LSB first
		// same for speed command
		// and same for shoot, where payload is the timer in ms
		uint32_t payload = 0;

		payload = buf[0];
		payload |= (buf[1]<<8);
		test_command(id,payload);
		return;
	}
	test_command(id,0);
}
