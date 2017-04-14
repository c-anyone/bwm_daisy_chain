/*
 * new_state_machine.h
 *
 *  Created on: Apr 4, 2017
 *      Author: Fabio Pungg
 */

#ifndef NEW_STATE_MACHINE_H_
#define NEW_STATE_MACHINE_H_


typedef enum {
	I1_POWERED_UP = 0,		//powered up, check communication
	I2_COMM_CHECKED,		// comm
	I3_SLED_INIT,
	I4_MAGAZINE_INIT,
	S1_PICKUP_BALL,
	S2_SHOT_READY,
	S3_SHOT_SEQUENCE,
	S4_SHOT_DONE,
	MAX_STATE
} states_t;

void state_machine_init(void);
void trigger_init_procedure(void);
void trigger_shot_procedure(void);
states_t state_machine(states_t);

#endif /* NEW_STATE_MACHINE_H_ */
