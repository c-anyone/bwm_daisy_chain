/*
 * new_state_machine.c
 *
 *  Created on: Apr 4, 2017
 *      Author: faebsn
 */


#include "new_state_machine.h"
#include "..\daisy_chain\application_layer.h"

#define TIMER_1SEC	(1000000u)

struct machine_state {
	uint8_t id;
	bool ready; // true if the board is ready, false if it's busy
	bool ack;	// signals if the last command sent to the machine has been acked
	uint8_t last_command;	// save the last command sent to the device
	uint32_t last_parameter;  // together with the last parameters
	uint32_t ack_timer; //has to be started after every sent command, to be reset on ack
};

static struct machine_state cur_machine_state[MACHINE_COUNT] = {
		{0,false,true,0,0,0}, // as long as master is local it never needs ack
		{1,false,false,0,0,0},
		{2,false,false,0,0,0},
		{3,false,false,0,0,0}
};

static bool ready_state[MACHINE_COUNT] = {
	false,false,false,false
};

static void set_ready(uint8_t id) {
	if(id<MACHINE_COUNT) {
		cur_machine_state[id].ready = true;
	}
}

static void set_busy(uint8_t id) {
	if(id<MACHINE_COUNT) {
		cur_machine_state[id].ready = false;
	}
}

static void set_ack(uint8_t id) {
	if(id<MACHINE_COUNT) {
		cur_machine_state[id].ack = true;
		SYSTIMER_StopTimer(cur_machine_state[id].ack_timer);
	}
}

static void start_ack_timer(uint8_t id) {
	if(id<MACHINE_COUNT) {
		cur_machine_state[id].ack = false;
		SYSTIMER_StartTimer(cur_machine_state[id].ack_timer);
	}
}

static inline bool all_ready(void) {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(!ready_state[i]) {
			return false;
		}
	}
	return true;
}

static void missing_ack_callback(void *args) {
	if(args != NULL) {
		uint8_t id = *args;
		// TODO: react to a missing ack from the specific machine
		// cur_machine_state[id].last_command....
	}
}

void state_machine_init(void) {
	// TODO: setup callbacks for application layer
	// TODO: start a communication check
	// TODO: request a state from every board
	for(int i=0;i<MACHINE_COUNT;++i) {
		cur_machine_state[i].id = i;
		cur_machine_state[i].ack_timer = SYSTIMER_CreateTimer(TIMER_1SEC*2,
				SYSTIMER_MODE_ONE_SHOT,missing_ack_callback,&(cur_machine_state[i].id));
	}
	set_cmd_ready_callback(set_ready);
	set_cmd_busy_callback(set_busy);
	set_cmd_ack_callback(set_ack);
}



states_t new_state_machine(events_t event) {
	if(event>=MAX_EVENT) {
		return SR_ERROR;
	}
	return I1_POWERED_UP;
}
