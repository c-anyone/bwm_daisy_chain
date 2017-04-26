/*
 * master_control.h
 *
 *  Created on: Mar 27, 2017
 *      Author: Fabio Pungg
 */

#ifndef MASTER_CONTROL_H_
#define MASTER_CONTROL_H_

typedef enum {
	MASTER_INIT_ONE = 0,		// lower ball intake
	MASTER_INIT_TWO,			// move sled back to pos0
	MASTER_INIT_THREE,			// move sled to waiting
	MASTER_WAITING,			// wait for take ball sequence trigger
	MASTER_TAKE_BALL_SEQUENCE,	// move sled to pos0
	MASTER_SLED_INTAKE_POS,		// raise ball intake
	MASTER_BALL_TAKEN,			// move sled to shot ready position
	MASTER_SHOT_READY,		// shot ready reached, indicate shot ready
	MASTER_SHOT_SEQUENCE,		// move sled (quick) to end pos
	MASTER_SHOOTING,			// lower ball intake
	MASTER_SHOT_DONE,			// indicate shot done, take ball sequence
	NUMBER_OF_STATES
} master_states_t;

void master_control_init(void);
void master_control_get_ball_sequence(void);
void master_control_start_shot_sequence(void);
master_states_t get_master_state(void);

void sled_state_machine(void);

extern void master_control_waiting();
extern void master_control_shot_ready();
extern void master_control_shot_done(); //never called?

#endif /* MASTER_CONTROL_H_ */
