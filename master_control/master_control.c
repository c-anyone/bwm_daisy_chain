/*
 * master_control.c
 *
 *  Created on: Mar 27, 2017
 *      Author: faebsn
 */

#include "master_control.h"
#include "ball_intake.h"
#include "sled_positioning.h"


typedef enum {
	MASTER_INIT_ONE = 0,	// lower ball intake
	MASTER_INIT_TWO,		// move sled back to pos0
	MASTER_INIT_THREE,		// move sled to waiting
	MASTER_WAITING,
	MASTER_TAKE_BALL_SEQUENCE,
	MASTER_SLED_INTAKE_POS,
	MASTER_BALL_TAKEN,
	MASTER_SHOT_READY_ONE,
	MASTER_SHOT_READY_TWO,
	MASTER_SHOT_SEQUENCE,
	MASTER_SHOOTING,
	MASTER_SHOT_DONE
} master_states_t;

master_states_t cur_master_state = MASTER_INIT_ONE;
static void master_state_machine(void) {
	switch(cur_master_state) {
	case MASTER_INIT_ONE:
		cur_master_state = MASTER_INIT_TWO;
		sled_move_pos0();
		break;
	case MASTER_INIT_TWO:
		cur_master_state = MASTER_INIT_THREE;
		sled_move_waiting();
		break;
	case MASTER_INIT_THREE:
		cur_master_state = MASTER_WAITING;
		master_control_waiting();
		// signal sled init done
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
		cur_master_state = MASTER_SHOT_READY_TWO;
		master_control_shot_ready();
		break;
	case MASTER_SHOT_READY_TWO:
		// do nothing till the shooting sequence is triggered
		break;
	case MASTER_SHOT_SEQUENCE:
		cur_master_state = MASTER_SHOOTING;
		sled_move_shoot();
	case MASTER_SHOOTING:
		cur_master_state = MASTER_SHOT_DONE;
		ball_intake_lower();
	case MASTER_SHOT_DONE:
		cur_master_state = MASTER_TAKE_BALL_SEQUENCE;
		master_control_shot_done();
		sled_move_pos0();
		break;
	default:
		// error
		break;
	}
}

void master_control_get_ball_sequence(void) {
	if(cur_master_state!= MASTER_WAITING) {
//
	}
	else {
		cur_master_state = MASTER_TAKE_BALL_SEQUENCE;
		master_state_machine();
	}
}

void master_control_start_shot_sequence(void) {
	if(cur_master_state!= MASTER_SHOT_READY_ONE) {
		// should not do anything, sled is not ready
	} else {
		cur_master_state = MASTER_SHOT_SEQUENCE;
		master_state_machine();
	}
}

void master_control_init() {
	cur_master_state = MASTER_INIT_ONE;
	ball_intake_init();
	//	sled_init();
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
void sled_home_position(void) {
	master_state_machine();
}

/**
 * triggered by servo controller
 */
void sled_position_reached(void) {
	master_state_machine();
}
