/*
 * machine_state.c
 *
 *  Created on: Mar 24, 2017
 *      Author: fapungg
 */

#include "machine_state.h"

machine_state_t state_machine(machine_state_t current_state) {
//	states[current_state]->stateFun)();
	states[current_state].stateFun();
	bool ret = states[current_state].transFun();
	if(ret) {
		return states[current_state].next_state;
	}
	return current_state;
}

void init() {
	// send out init signal to everyone
}

bool init_to_waiting() {
	// check if all init conditions are set
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
	// check if sled is done positioning
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
