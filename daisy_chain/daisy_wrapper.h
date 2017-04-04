/*
 * daisy_wrapper.h
 *
 *  Created on: Mar 22, 2017
 *      Author: Fabio Pungg
 */


#ifndef DAISY_WRAPPER_H_
#define DAISY_WRAPPER_H_


#include <DAVE.h>
#include <stdbool.h>
#include "min.h"

#define DAISY_PORT		(0x00u)

#define ID_PING			(0x00u)
#define ID_MASTER		(0x01u)
#define ID_SLAVE_1		(0x02u)
#define ID_SLAVE_2		(0x03u)
#define ID_SLAVE_3		(0x04u)
#define MACHINE_COUNT	(0x04u)

// id definition for this device
// might be moved to another file (main.c)
#ifdef 	MASTER_DEVICE
#define MY_ID ID_MASTER
#elif defined SLAVE_DEVICE_1
#define SLAVE_DEVICE
#define MY_ID ID_SLAVE_1
#elif defined SLAVE_DEVICE_2
#define SLAVE_DEVICE
#define MY_ID ID_SLAVE_2
#elif defined SLAVE_DEVICE_3
#define SLAVE_DEVICE
#define MY_ID ID_SLAVE_3
#else
#error "no device type defined"
#endif
// commands to communicate with daisy devices

uint8_t daisy_rx_buf[0xff];

/**
 * common functions for both master and slave
 */
void daisy_min_init(void);
// should be called in a loop to receive data from UART
void daisy_rx_polling(void);
// ping to periodically verify working communication
void daisy_ping(void);

void daisy_send_payload(uint8_t id,uint8_t buf[], uint8_t length);

extern void daisy_ping_received(void);
extern void daisy_payload_received(uint8_t* payload, uint8_t length);



#endif /* DAISY_WRAPPER_H_ */
