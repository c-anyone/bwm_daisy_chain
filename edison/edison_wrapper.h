/*
 * wlan_handler.h
 *
 *  Created on: Mar 23, 2017
 *      Author: Markus
 */

#include "edison_min.h"

#ifndef EDISON_WRAPPER_H_
#define EDISON_WRAPPER_H_

void set_shoot(void);

void set_coordinate(uint8_t x_low, uint8_t x_high, uint8_t y_low, uint8_t y_high, uint8_t hang);

void edison_rx_polling();

extern void test_communication(void);
extern void test_command(uint8_t,uint32_t);


#endif /* EDISON_WRAPPER_H_ */
