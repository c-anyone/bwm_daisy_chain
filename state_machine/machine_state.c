/*
 * machine_state.c
 *
 *  Created on: Mar 24, 2017
 *      Author: Fabio Pungg
 */

#include "machine_state.h"
#include "..\daisy_chain\daisy_wrapper.h"


/**
 * init might have to be split up into multiple steps because
 * ball intake has to be lowered and sled has to be in waiting position
 * BEFORE magazine starts rotating
 */

static machine_status_t slave_status[MACHINE_COUNT] = {
		{false,false},
		{false,false},
		{false,false},
		{false,false}
};

void machine_state_init_done(uint8_t id) {
	if(id<MACHINE_COUNT) {
		slave_status[id].init = true;
	}
}

void machine_state_set_ready(uint8_t id) {
	if(id<MACHINE_COUNT) {
		slave_status[id].ready = true;
	}
}

void machine_state_set_busy(uint8_t id) {
	if(id<MACHINE_COUNT) {
		slave_status[id].ready = false;
	}
}

static inline bool all_boards_init() {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(!slave_status[i].init) {
			return false;
		}
	}
	return true;
}

static inline bool all_boards_ready() {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(!slave_status[i].ready) {
			return false;
		}
	}
	return true;
}

/**
 *
 */
static const state_t machine_state[NUM_STATES] = {
		{INIT, WAITING, &init, &init_to_waiting},
		{WAITING, INTAKE_POS, &waiting, &waiting_to_intake_pos},
		{INTAKE_POS, INTAKE_READY, &intake_pos, &intake_pos_to_ready},
		{INTAKE_READY, SHOT_READY, &intake_ready, &intake_ready_to_shot_ready},
		{SHOT_READY, SHOT_FIRED, &shot_ready, &shot_ready_to_fired},
		{SHOT_FIRED, SHOT_READY, &shot_fired, &shot_fired_to_waiting}
};

/**
 *	on function entry executes state entry function once
 *	afterwards it waits for transition function to return true
 *	before transitioning to the next state
 */
machine_state_t state_machine(machine_state_t machine_current_state) {
	static bool entered = false;
	if(entered == false) {
		entered = true;
		machine_state[machine_current_state].entryFun();
	}
	bool ret = machine_state[machine_current_state].transFun();
	if(ret == true) {
		entered = false;
		return machine_state[machine_current_state].next_state;
	}
	return machine_current_state;
}

void init() {
	// send out init signal to everyone

}

bool init_to_waiting() {
	// check if all init conditions are set
	// daisy_received
	return false;
}

void waiting() {
	// move sled to intake position
}

bool waiting_to_intake_pos() {
	// check if sled is in position
	return false;
}

void intake_pos() {
	// raise intake
}

bool intake_pos_to_ready() {
	// check if intake done raising
	return false;
}

void intake_ready() {
	// begin moving sled to shot ready position
}

bool intake_ready_to_shot_ready(){
	// check if sled is done repositioning
	return false;
}

void  shot_ready() {
	// wait for external shoot signal
}

bool shot_ready_to_fired() {
	// check if sled is in fired position
	return false;
}

void shot_fired() {
	// move sled back to waiting position
	// rotate magazine
	// lower intake
}

bool shot_fired_to_waiting() {
	// sled in position
	// magazine in position
	// intake lowered
	return false;
}
