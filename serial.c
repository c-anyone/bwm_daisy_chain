/*
 * serial.c
 *
 *  Created on: Mar 22, 2017
 *      Author: faebsn
 */

#include <DAVE.h>
#include "serial.h"

void min_frame_received(uint8_t buf[], uint8_t control, uint8_t id) {
	daisy_received_buffer(buf, control, id);
}

void daisy_transmit_buffer(uint8_t id, uint8_t buf[], uint8_t control) {
	min_tx_frame(id,buf,control);
}

void daisy_rx_polling() {
	if(!UART_IsRXFIFOEmpty(&UART)) {
		   uint8_t byte = XMC_UART_CH_GetReceivedData(UART.channel);
		   min_rx_byte(byte);
	}
}

uint8_t min_tx_space(void) {
	return 0xff;
}

void min_tx_byte(uint8_t byte) {
	UART_Transmit(&UART,&byte,1);
}

