/*
 * global_definition.h
 *
 *  Created on: Mar 22, 2017
 *      Author: faebsn
 */

#ifndef GLOBAL_DEFINITION_H_
#define GLOBAL_DEFINITION_H_

// id definitions for different daisy devices
// might be moved to daisy_wrapper.h
#define ID_MASTER  (0x01u)
#define ID_SLAVE_1 (0x02u)
#define ID_SLAVE_2 (0x03u)
#define ID_SLAVE_3 (0x04u)

// id definition for this device
// might be moved to daisy_wrapper.h
#define MY_ID ID_MASTER

// commands to communicate with daisy devices
// might be moved to daisy_wrapper.h
#define CMD_UNDEFINDED (0x00u)
#define CMD_START	   (0x01u)
#define CMD_READY	   (0x02u)

// type for communication, payload is a placeholder, might be needed
// for setting motors to specific speeds etc.
typedef struct {
	uint8_t command;
	uint8_t sender_id;
	uint16_t payload;
} daisy_command_t;




#endif /* GLOBAL_DEFINITION_H_ */
