/*
 * daisy_wrapper.c
 *
 *  Created on: Mar 22, 2017
 *      Author: Fabio Pungg
 *
 * Handles addressing and other daisy chain related stuff
 * uses callback daisy_payload_received when data is received
 */

#include <DAVE.h>
#include "daisy_wrapper.h"

static uint8_t daisy_tx_buf[0xff];


static struct min_context context;

void daisy_min_init(void) {
	min_init_context(&context,DAISY_PORT);
}

void min_application_handler(uint8_t id, uint8_t buf[], uint8_t length, uint8_t port) {
	if(id == ID_PING) {
		daisy_ping_received();
		if(ID_MASTER != MY_ID) {
			min_send_frame(&context,id,buf,length);
		}
	}
	else if(id == MY_ID){
		daisy_payload_received(buf,length);
		// FIXME: memcpy data to a safe buffer, or do this in upper layer
	} else if(MY_ID != ID_MASTER) {
		min_send_frame(&context,id,buf,length);
	}
}

// should be called in main loop
void daisy_rx_polling() {
	while(!UART_IsRXFIFOEmpty(&DAISY)) {
		uint8_t byte = XMC_UART_CH_GetReceivedData(DAISY.channel);
		min_poll(&context,&byte,sizeof(byte));
	}
}

// always returns 0xff for now, buffer should be sufficient
uint16_t min_tx_space(uint8_t port) {
	return 0xff;
}

void daisy_ping() {
//	min_send_frame(&context,ID_PING,(uint8_t*)&command,0);
}

void daisy_send_payload(uint8_t id, uint8_t* payload, uint8_t length) {
	if(length<MAX_PAYLOAD) {
		memcpy(daisy_tx_buf,payload,length);
		min_send_frame(&context,id,daisy_tx_buf,length);
	}
}
#ifdef MASTER_DEVICE

#elif defined SLAVE_DEVICE



#endif
void min_tx_byte(uint8_t port,uint8_t byte) {
	UART_Transmit(&DAISY,&byte,sizeof(uint8_t));
}

