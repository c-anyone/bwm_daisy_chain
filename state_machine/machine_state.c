/*
 * machine_state.c
 *
 *  Created on: Mar 24, 2017
 *      Author: Fabio Pungg
 */

#include "machine_state.h"
#include "..\daisy_chain\application_layer.h"
#include "master_control\master_control.h"

/**
 * init might have to be split up into multiple steps because
 * ball intake has to be lowered and sled has to be in waiting position
 * BEFORE magazine starts rotating
 */
static bool daisy_status = false;
static bool external_trigger = false;

static void init_phase_one();
static void init_phase_two();
static void rotate_magazine(void);
static void ball_intake(void);
static void shot_sequence_start(void);

static bool init_phase_one_to_two();
static bool init_two_to_sled_ready();
static bool rotate_magazine_done(void);
static bool ball_intake_done(void);
static bool trigger_shot_sequence(void);
static bool shot_sequence_end(void);


void machine_state_set_ready(uint8_t);
void machine_state_set_busy(uint8_t);

static const state_t machine_state[NUM_STATES+1] = {
		{INIT_PHASE_ONE, INIT_PHASE_TWO,   init_phase_one, NULL,  init_phase_one_to_two},
		{INIT_PHASE_TWO, ROTATE_MAGAZINE,  init_phase_two,NULL,  init_two_to_sled_ready},
		{ROTATE_MAGAZINE, BALL_INTAKE,     rotate_magazine, NULL,  rotate_magazine_done},
		{BALL_INTAKE, SHOT_READY,          ball_intake, NULL,  ball_intake_done},
		{SHOT_READY, SHOOTING, 			   NULL,NULL,trigger_shot_sequence},
		{SHOOTING, ROTATE_MAGAZINE, 	   shot_sequence_start, NULL, shot_sequence_end}
};

typedef struct {
	bool init;
	bool ready;
}machine_status_t;

static machine_status_t slave_status[MACHINE_COUNT] = {
		{false,false},
		{false,false},
		{false,false},
		{false,false}
};

static void status_callback(uint8_t sender_id, uint32_t status) {

}

void machine_state_init(void) {
	// TODO: setup the callbacks for daisy chain
	// TODO: check the daisy connection
	set_cmd_status_callback(status_callback);
	// wait for sequence start

}

void machine_state_init_done(uint8_t id) {
	if(id<MACHINE_COUNT) {
		slave_status[id].init = true;
	}
}

void machine_state_set_ready(uint8_t id) {
	if(id<MACHINE_COUNT+1) {
		slave_status[id - 1].ready = true;
	}
}

void machine_state_set_busy(uint8_t id) {
	if(id<MACHINE_COUNT+1) {
		slave_status[id - 1].ready = false;
	}
}

static inline void set_all_ready(void) {
	for(int i=0;i<MACHINE_COUNT;++i) {
		slave_status[i].ready = true;
	}
}

static inline bool all_boards_init() {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(slave_status[i].init == false) {
			return false;
		}
	}
	return true;
}

static inline bool all_boards_ready() {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(slave_status[i].ready == false) {
			return false;
		}
	}
	return true;
}

void machine_state_external_trigger(void) {
	external_trigger = true;
}

/**
 *	on function entry executes state entry function once,
 *	afterwards it waits for transition function to return true
 *	before transitioning to the next state
 *	an optional state function will be executed every time
 *	if no transition function is set for the current state, it
 *	will immediately transition to the next state
 */
machine_state_t old_state_machine(machine_state_t machine_current_state) {
	static bool entered = false;
	if(entered == false) {
		entered = true;
		if(machine_state[machine_current_state].entryFun != NULL) {
			machine_state[machine_current_state].entryFun();
		}
	}

	if(machine_state[machine_current_state].stateFun != NULL) {
		machine_state[machine_current_state].stateFun();
	}

	if(machine_state[machine_current_state].transFun != NULL) {
		bool ret = machine_state[machine_current_state].transFun();
		if(ret == true) {
			entered = false;
			return machine_state[machine_current_state].next_state;
		}
	} else {
		return machine_state[machine_current_state].next_state;
	}
	return machine_current_state;
}

// check daisy chain connection status
static void init_phase_one() {
//	daisy_ping();
}

static bool init_phase_one_to_two() {
	// check if all init conditions are set
	// daisy_ping_received
	if(daisy_status) {
		set_all_ready();
	}
	return daisy_status;
}

static void init_phase_two() {
	// daisy is working, now init the sled
	machine_state_set_busy(ID_MASTER);
	master_control_init();
}

static bool init_two_to_sled_ready() {
	//	return get_master_state == MASTER_WAITING;
	return all_boards_ready();
}

static void rotate_magazine(void) {
	machine_state_set_busy(ID_SLAVE_1);
	signal_start(ID_SLAVE_1,0);
}

static bool rotate_magazine_done(void) {
	return all_boards_ready();
}

static void ball_intake(void) {
	machine_state_set_busy(ID_MASTER);
	master_control_get_ball_sequence();
}

static bool ball_intake_done(void) {
	return all_boards_ready();
}

static bool trigger_shot_sequence(void) {
	if(external_trigger == true) {
		external_trigger = false;
		return true;
	}
	return false;
}

static void shot_sequence_start(void) {
	machine_state_set_busy(ID_MASTER);
	master_control_start_shot_sequence();
}

static bool shot_sequence_end(void) {
	if(all_boards_ready()) {
		return true;
	}
	return false;
}

