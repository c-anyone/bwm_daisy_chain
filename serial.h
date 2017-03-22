/*
 * serial.h
 *
 *  Created on: Mar 22, 2017
 *      Author: faebsn
 */

#include <DAVE.h>
#include "min.h"

#ifndef SERIAL_H_
#define SERIAL_H_

void daisy_transmit_buffer(uint8_t id, uint8_t buf[], uint8_t control);

void daisy_rx_polling(void);

// callback for received data
void daisy_received_buffer(uint8_t buf[], uint8_t control, uint8_t id);

#endif /* SERIAL_H_ */
