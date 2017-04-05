/*
 * new_state_machine.h
 *
 *  Created on: Apr 4, 2017
 *      Author: Fabio Pungg
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
//	S2A_CHANGE_ELEVATION,
//	S2B_CHANGE_SPEED,
	S3_SHOT_SEQUENCE,
//	SR_ERROR,
	MAX_STATE
} states_t;

typedef enum {
	E_NO_EVENT=0,
	E_ALL_READY,
	E_WORKING,
	E_EXTERNAL,
	MAX_EVENT
} events_t;

struct state {
	states_t state;
	events_t event;

};

void state_machine_init(void);
void trigger_init_procedure(void);
void trigger_shot_procedure(void);
states_t state_machine(states_t);

#endif /* NEW_STATE_MACHINE_H_ */
