/*
 * new_state_machine.h
 *
 *  Created on: Apr 4, 2017
 *      Author: faebsn
 */

#ifndef NEW_STATE_MACHINE_H_
#define NEW_STATE_MACHINE_H_


typedef enum {
	I1_POWERED_UP = 0,
	I2_COMM_CHECKED,
	I3_SLED_INIT,
	I4_MAGAZINE_INIT,
	S1_PICKUP_BALL,
	S2_SHOT_READY,
	S2A_CHANGE_ELEVATION,
	S2B_CHANGE_SPEED,
	S3_SHOT_SEQUENCE,
	SR_ERROR,
	MAX_STATE
} states_t;

typedef enum {
	E_ALL_READY=0,
	E_WORKING,
	E_EXTERNAL,
	MAX_EVENT
} events_t;

struct state {
	states_t state;
	events_t event;

};

void state_machine_init(void);
states_t new_state_machine(events_t event);

#endif /* NEW_STATE_MACHINE_H_ */
