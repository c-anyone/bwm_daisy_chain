/*
 * new_state_machine.c
 *
 *  Created on: Apr 4, 2017
 *      Author: Fabio Pungg
 */


#include "new_state_machine.h"
#include "../daisy_chain/application_layer.h"
#include "../master_control/master_control.h"

#define TIMER_1SEC	(1000000u)

static volatile bool init_trigger = false;
static volatile bool shot_trigger = false;
static volatile bool sled_shot_done = false;
static volatile bool sled_is_waiting = false;
static volatile bool sled_shot_ready = false;

static bool check_shot_done(void) {
	if(sled_shot_done) {
		sled_shot_done = false;
		return true;
	}
	return false;
}

static bool check_sled_waiting(void) {
	if(sled_is_waiting) {
		sled_is_waiting = false;
		return true;
	}
	return false;
}

static bool check_shot_ready(void) {
	if(sled_shot_ready) {
		sled_shot_ready = false;
		return true;
	}
	return false;
}

struct machine_state {
	uint8_t id;
	bool ready; // true if the board is ready, false if it's busy
	bool ack;	// signals if the last command sent to the machine has been acked
	uint8_t last_command;	// save the last command sent to the device
	uint32_t last_parameter;  // together with the last parameters
	uint32_t ack_timer; //has to be started after every sent command, to be reset on ack
};

static struct machine_state cur_machine_state[MACHINE_COUNT] = {
		{0,true,true, 0,0,0}, // as long as master is local it never needs ack/is ready from start
		{1,true,true,0,0,0},
		{2,true,true,0,0,0},
		{3,true,true,0,0,0}
};

static bool all_ready(void) {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(!cur_machine_state[i].ready) {
			return false;
		}
	}
	return true;
}

typedef void(*StateEntryFun)();
typedef void(*StateFun)();
typedef bool(*TransitionFun)();

struct state_functions{
	states_t nextState;
	StateEntryFun entryFun;
	StateFun	  stateFun;
	TransitionFun transFun;
};
static void entry_i1(void); static bool transition_i1(void);
static bool i2_check_init_trigger(void);
static void i3_init_sled(void);
static void i4_rotate_magazine(void);
static void s1_get_ball_sequence(void);
static bool s2_check_shot_trigger(void);
static void s3_start_shot_sequence(void);
static void s4_shot_done_prepare_next(void); static bool s4_to_shot_ready(void);

static struct state_functions state_functions_table[MAX_STATE] = {
		{I2_COMM_CHECKED,entry_i1,NULL,transition_i1},
		{I3_SLED_INIT, NULL   ,NULL,i2_check_init_trigger},
		{I4_MAGAZINE_INIT, i3_init_sled,sled_state_machine,check_sled_waiting},
		{S1_PICKUP_BALL, i4_rotate_magazine,NULL,all_ready},
		{S2_SHOT_READY, s1_get_ball_sequence,sled_state_machine,check_shot_ready},
		{S3_SHOT_SEQUENCE, NULL    ,NULL,s2_check_shot_trigger},
		{S4_SHOT_DONE,s3_start_shot_sequence,sled_state_machine,check_shot_done},
		{S2_SHOT_READY,s4_shot_done_prepare_next,sled_state_machine,s4_to_shot_ready}
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

static inline bool all_acknowledged(void) {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(!cur_machine_state[i].ack) {
			return false;
		}
	}
	return true;
}

void state_machine_init(void) {
	// TODO: setup callbacks for application layer
	// TODO: start a communication check
	// TODO: request a state from every board
	for(int i=0;i<MACHINE_COUNT;++i) {
		cur_machine_state[i].id = i;
	}
	set_cmd_ready_callback(set_ready);
	set_cmd_busy_callback(set_busy);
	set_cmd_ack_callback(set_ack);
}
static bool entered = false;

states_t state_machine(states_t state) {

	if(entered == false) {
		entered = true;
		if(state_functions_table[state].entryFun != NULL) {
			state_functions_table[state].entryFun();
		}
	}
	if(state_functions_table[state].stateFun != NULL) {
		state_functions_table[state].stateFun();
	}

	if(state_functions_table[state].transFun != NULL) {
		bool ret = state_functions_table[state].transFun();
		if(ret == true) {
			entered = false;
			return state_functions_table[state].nextState;
		}
	} else {
		entered = false;
		return state_functions_table[state].nextState;
	}
	return state;
}

void entry_i1(){
	signal_get_status(ID_SLAVE_1);
//	signal_get_status(ID_SLAVE_2);
	//	signal_get_status(ID_SLAVE_3); for now this one will not be used
}

bool transition_i1(){
	return all_acknowledged() && all_ready();
}

bool i2_check_init_trigger(){
	if(init_trigger) {
		init_trigger = false;
		return true;
	}
	return false;
}


void i3_init_sled(){
//	set_busy(ID_MASTER);
	master_control_init();
//	set_busy(ID_SLAVE_2);
//	signal_start(ID_SLAVE_2,0); // fu only started on speed > 0
}


void i4_rotate_magazine(){
	set_busy(ID_SLAVE_1);
	signal_start(ID_SLAVE_1,1);
	//	set_busy(ID_SLAVE_2);
	//	signal_start(ID_SLAVE_2,0);
	//	start_ack_timer(ID_SLAVE_1);
	//	start_ack_timer(ID_SLAVE_2);
}
void s1_get_ball_sequence(){
	set_busy(ID_MASTER);
	master_control_get_ball_sequence();
}
void s3_start_shot_sequence(){
	set_busy(ID_MASTER);
	sled_shot_done = false;
	master_control_start_shot_sequence();
}

bool s2_check_shot_trigger() {
	if(shot_trigger) {
		shot_trigger = false;
		return true;
	}
	return false;
}

void s4_shot_done_prepare_next(void) {
	master_control_get_ball_sequence();
	set_busy(ID_SLAVE_1);
	set_busy(ID_MASTER);
	signal_start(ID_SLAVE_1,1);
}

bool s4_to_shot_ready() {
	return all_ready();
}

void master_control_waiting() {
//	set_ready(ID_MASTER);
	sled_is_waiting = true;
}

void master_control_shot_ready() {
	// ready to shoot
	set_ready(ID_MASTER);
	sled_shot_ready = true;
}

void master_control_shot_done() {
	// right time to trigger magazine
	set_ready(ID_MASTER);
	sled_shot_done = true;
}


void trigger_init_procedure(void) {
	init_trigger = true;
	entered = false;
}

void trigger_shot_procedure(void) {
	shot_trigger = true;
}
