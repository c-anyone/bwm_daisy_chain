/*
 * master_control.c
 *
 *  Created on: Mar 27, 2017
 *      Author: Fabio Pungg
 */

#ifdef OLD_MASTER_CONTROL
#include <DAVE.h>
#include "master_control.h"
#include "ball_intake.h"
#include "sled_positioning.h"

static master_states_t cur_master_state = MASTER_INIT_ONE;
static uint32_t interrupt_count = 0;

master_states_t get_master_state(void) {
	return cur_master_state;
}

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
		cur_master_state = MASTER_WAITING_TWO;
		master_control_waiting();
		break;
	case MASTER_WAITING_ONE:
		// signal sled init done
		cur_master_state = MASTER_WAITING_TWO;
		break;
	case MASTER_WAITING_TWO:
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
		break;
	case MASTER_SHOOTING:
		cur_master_state = MASTER_SHOT_DONE;
		ball_intake_lower();
		master_control_waiting();
		break;
	case MASTER_SHOT_DONE:
		cur_master_state = MASTER_TAKE_BALL_SEQUENCE;
		sled_move_pos0();
		break;
	default:
		// error
		break;
	}
}

void master_control_get_ball_sequence(void) {
	if(cur_master_state!= MASTER_WAITING_TWO) {
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
	interrupt_count++;
	// FIXME: remove interrupt count, only for debugging!
}
#endif
