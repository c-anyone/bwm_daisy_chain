/*
 * master_control.c
 *
 *  Created on: Mar 27, 2017
 *      Author: faebsn
 */

#include <DAVE.h>
#include "master_control.h"
#include "ball_intake.h"
#include "sled_positioning.h"


typedef enum {
	MASTER_INIT_ONE = 0,		// lower ball intake
	MASTER_INIT_TWO,			// move sled back to pos0
	MASTER_INIT_THREE,			// move sled to waiting
	MASTER_WAITING,				// wait for take ball sequence trigger
	MASTER_TAKE_BALL_SEQUENCE,	// move sled to pos0
	MASTER_SLED_INTAKE_POS,		// raise ball intake
	MASTER_BALL_TAKEN,			// move sled to shot ready position
	MASTER_SHOT_READY_ONE,		// shot ready reached, indicate shot ready
	MASTER_SHOT_READY_TWO,		// wait for shot ready trigger
	MASTER_SHOT_SEQUENCE,		// move sled (quick) to end pos
	MASTER_SHOOTING,			// lower ball intake
	MASTER_SHOT_DONE			// indicate shot done, take ball sequence
} master_states_t;

master_states_t cur_master_state = MASTER_INIT_ONE;
static void master_state_machine(void) {
	switch(cur_master_state) {
	case MASTER_INIT_ONE:
		cur_master_state = MASTER_INIT_TWO;
		sled_move_pos0();
		break;
	case MASTER_INIT_TWO:
		if(PIN_INTERRUPT_GetPinValue(&SLED_LIMIT_SWITCH_INTERRUPT) == 0) {
			cur_master_state = MASTER_INIT_THREE;
			sled_move_waiting();
		}
		break;
	case MASTER_INIT_THREE:
		if(PIN_INTERRUPT_GetPinValue(&SLED_POSITION_INTERRUPT) == 0) {
			cur_master_state = MASTER_WAITING;
			master_control_waiting();
			// signal sled init done
		}
		break;
	case MASTER_WAITING:
		// do nothing till the next sequence is triggered
		break;
	case MASTER_TAKE_BALL_SEQUENCE:
		cur_master_state = MASTER_SLED_INTAKE_POS;
		sled_move_pos0();
		break;
	case MASTER_SLED_INTAKE_POS:
		cur_master_state = MASTER_BALL_TAKEN;
		ball_intake_raise();
		break;
	case MASTER_BALL_TAKEN:
		cur_master_state = MASTER_SHOT_READY_ONE;
		sled_move_shot_ready();
	case MASTER_SHOT_READY_ONE:
		if(PIN_INTERRUPT_GetPinValue(&SLED_POSITION_INTERRUPT) == 0) {
			cur_master_state = MASTER_SHOT_READY_TWO;
			master_control_shot_ready();
		}
		break;
	case MASTER_SHOT_READY_TWO:
		// do nothing till the shooting sequence is triggered
		break;
	case MASTER_SHOT_SEQUENCE:
		cur_master_state = MASTER_SHOOTING;
		sled_move_shoot();
		break;
	case MASTER_SHOOTING:
		if(PIN_INTERRUPT_GetPinValue(&SLED_POSITION_INTERRUPT) == 0) {

			cur_master_state = MASTER_SHOT_DONE;
			ball_intake_lower();
		}
		break;
	case MASTER_SHOT_DONE:
		cur_master_state = MASTER_TAKE_BALL_SEQUENCE;
		if(PIN_INTERRUPT_GetPinValue(&SLED_POSITION_INTERRUPT) == 0) {
			master_control_shot_done();
			sled_move_pos0();
		}
		break;
	default:
		// error
		break;
	}
}

void master_control_get_ball_sequence(void) {
	if(cur_master_state!= MASTER_WAITING) {
		// do nothing, wrong state for triggering the get ball sequence
	}
	else {
		cur_master_state = MASTER_TAKE_BALL_SEQUENCE;
		master_state_machine();
	}
}

void master_control_start_shot_sequence(void) {
	if(cur_master_state!= MASTER_SHOT_READY_TWO) {
		// should not do anything, sled is not ready to shoot (wrong state)
	} else {
		cur_master_state = MASTER_SHOT_SEQUENCE;
		master_state_machine();
	}
}

void master_control_init() {
	cur_master_state = MASTER_INIT_ONE;
	ball_intake_init();
	sled_init();
	ball_intake_lower();
}

/**
 * triggered by ball intake
 */
void ball_intake_ready(void) {
	master_state_machine();
}

/**
 * triggered by limit switch
 */
void sled_limit_switch(void) {
	master_state_machine();
}

/**
 * triggered by servo controller
 */
void sled_position_reached(void) {

	master_state_machine();
}
