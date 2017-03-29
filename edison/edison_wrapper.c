/*
 * wlan_handler.c
 *
 *  Created on: Mar 23, 2017
 *      Author: Fabio Pungg
 */

#include <DAVE.h>
#include "edison_wrapper.h"

/* replaced with a communication test function */
//void set_shoot(void){
//	linear_pos_start();
//	uint32_t wait_during_shoot = 0xFFF;
//	delay(wait_during_shoot);
//	// Ballaufnahme implementieren
//	linear_pos_wait();
//}

void set_shoot(void) {
}



void set_coordinate(uint8_t x_low, uint8_t x_high, uint8_t y_low, uint8_t y_high, uint8_t hang){
//	uint16_t x_coordinate = (x_low | x_high << 8);
//	uint16_t y_coordinate = (y_low | y_high << 8);
//	float x = (float)x_coordinate / 10;
//	float y = (float)y_coordinate / 10;
}

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
	test_ball_intake(id);
	if (id == 0x11){
//		set_shoot();
	}
	else if (id == 0x12){

//		uint8_t x_low = buf[0];
//		uint8_t x_high = buf[1];
//		uint8_t y_low = buf[2];
//		uint8_t y_high = buf[3];
//		uint8_t hang = buf[4];
//		set_coordinate(x_low, x_high, y_low, y_high, hang);
	}
}
