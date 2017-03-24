/*
 * machine_state.c
 *
 *  Created on: Mar 24, 2017
 *      Author: fapungg
 */

#include "machine_state.h"

machine_state_t state_machine(machine_state current_state) {
	states[current_state]->stateFun();
	return states[current_state]->transFun();
}

void init() {
	// send out init signal to everyone
}

machine_state_t init_to_waiting() {
	// check if all init conditions are set
}

void waiting() {
	// move sled to intake position
}

machine_state_t waiting_to_intake_pos() {
	// check if sled is in position
}

void intake_pos() {
	// raise intake
}

machine_state_t intake_pos_to_ready() {
	// check if intake done raising
}

void intake_ready() {
	// begin moving sled to shot ready position
}

machine_state_t intake_ready_to_shot_ready(){
	// check if sled is done positioning
}

void  shot_ready() {
	// wait for external shoot signal
}

machine_state_t shot_ready_to_fired() {
	// check if sled is in fired position
}

void shot_fired() {
	// move sled back to waiting position
	// rotate magazine
	// lower intake
}

machine_state_t shot_fired_to_waiting() {
	// sled in position
	// magazine in position
	// intake lowered
}
