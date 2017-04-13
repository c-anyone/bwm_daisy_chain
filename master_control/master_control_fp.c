/*
 * master_control_fp.c
 *
 *  Created on: Apr 4, 2017
 *      Author: Fabio Pungg
 *
 *	New sled control state machine, based on function pointers
 *	a real table based approach is not too useful here, as
 *	the state transitions are mostly straightforward.
 *	For improved integration into the higher level state machine,
 *	calls to get the current state could be added.
 *
 *	TODO:
 *	Where the state is kept is still open, could be in the higher level
 *	state machine and fed into this via function parameters, but could also
 *	be made static and only manipulated via setter/getter functions (yeah, java...)
 *
 *	Pro for static/local:
 *		machine_state/new_state_machine will be kept clean, won't have to be
 *		modified, as old master_control keeps it's state local as well
 *		via indirect init, reset functions it triggers the usual states
 *
 *	Contra:
 *		you shouldn't do stuff like that
 */
#ifndef OLD_MASTER_CONTROL

#include <DAVE.h>
#include "master_control.h"
#include "ball_intake.h"
#include "sled_positioning.h"

typedef void (*StateEntryFun)();
typedef bool (*TransitionFun)();

struct state_functions{
	master_states_t nextState;
	StateEntryFun onEntry;
	TransitionFun onTransition;
};

static master_states_t sled_state = MASTER_INIT_ONE;
static volatile bool position_reached = false;
static volatile bool ball_sequence_trigger = false;
static volatile bool ball_intake_moved = false;
static volatile bool shot_sequence_trigger = false;
static volatile bool home_position_reached = false;

static bool entered = false;

static bool check_position_reached(void) {
	if(position_reached) {
		position_reached = false;
		return true;
	}
	return false;
}

static bool external_ball_sequence_trigger(void) {
	if(ball_sequence_trigger) {
		ball_sequence_trigger = false;
		return true;
	}
	return false;
}

static bool check_ball_intake_position(void) {
	if(ball_intake_moved) {
		ball_intake_moved = false;
		return true;
	}
	return false;
}

static bool external_shot_sequence_trigger(void) {
	if(shot_sequence_trigger) {
		shot_sequence_trigger = false;
		return true;
	}
	return false;
}

static bool check_home_position_reached(void) {
	if(home_position_reached) {
		home_position_reached = false;
		position_reached = false;
		return true;
	}
	return false;
}

// TODO: evaluate possible callback functions set into state struct
// to indicate finished sequence
static struct state_functions function_table[NUMBER_OF_STATES] = {
		{MASTER_INIT_TWO, ball_intake_lower,	check_ball_intake_position},
		{MASTER_INIT_THREE, sled_move_pos0,		check_home_position_reached},
		{MASTER_WAITING, sled_move_waiting,		check_position_reached},
		{MASTER_TAKE_BALL_SEQUENCE, master_control_waiting,	external_ball_sequence_trigger},
		{MASTER_SLED_INTAKE_POS, sled_move_pos0,check_home_position_reached},
		{MASTER_BALL_TAKEN, ball_intake_raise,	check_ball_intake_position},
		{MASTER_SHOT_READY, sled_move_shot_ready,	check_position_reached},
		{MASTER_SHOT_SEQUENCE, master_control_shot_ready, external_shot_sequence_trigger},
		{MASTER_SHOOTING, sled_move_shoot,	check_position_reached},
		{MASTER_SHOT_DONE,master_control_shot_done,	NULL},
		{MASTER_WAITING,ball_intake_lower,			check_ball_intake_position},
};

void sled_state_machine(void) {

	if(!entered) {
		entered = true;
		if(function_table[sled_state].onEntry != NULL) {
			function_table[sled_state].onEntry();
		}
	}
	if(function_table[sled_state].onTransition == NULL) {
		// transition here, return next state
		entered = false;
		sled_state = function_table[sled_state].nextState;
	} else {
		if(function_table[sled_state].onTransition()) {
			entered = false;
			sled_state = function_table[sled_state].nextState;
		}
	}
}

void master_control_init(void) {
	sled_state = MASTER_INIT_ONE;
	entered = false;
	ball_intake_init();
	sled_init();
}

void master_control_get_ball_sequence(void) {
//	if(sled_state == MASTER_WAITING) {
		ball_sequence_trigger = true;
//	}
}

void master_control_start_shot_sequence(void) {
	if(sled_state == MASTER_SHOT_READY) {
		shot_sequence_trigger = true;
	}
}

void ball_intake_ready(void) {
	ball_intake_moved = true;
}

void sled_limit_switch(void){
	home_position_reached = true;
}

void sled_position_reached(void) {
		position_reached = true;
}
#endif
