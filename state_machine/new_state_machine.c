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

void entry_i1();
void entry_i2();
void entry_i3();
void entry_i4();
void entry_s1();
void entry_s2();
void entry_s2a();
void entry_s2b();
void entry_s3();
void entry_sr();

void state_i1();
void state_i2();
void state_i3();
void state_i4();
void state_s1();
void state_s2();
void state_s2a();
void state_s2b();
void state_s3();
void state_sr();

bool transition_i1();
bool transition_i2();
bool transition_i3();
bool transition_i4();
bool transition_s1();
bool transition_s2();
bool transition_s2a();
bool transition_s2b();
bool transition_s3();
bool transition_sr();

static bool init_trigger = false;
static bool shot_trigger = false;

struct machine_state {
	uint8_t id;
	bool ready; // true if the board is ready, false if it's busy
	bool ack;	// signals if the last command sent to the machine has been acked
	uint8_t last_command;	// save the last command sent to the device
	uint32_t last_parameter;  // together with the last parameters
	uint32_t ack_timer; //has to be started after every sent command, to be reset on ack
};

	typedef void(*StateEntryFun)();
	typedef void(*StateFun)();
	typedef bool(*TransitionFun)();

struct state_functions{
		states_t nextState;
	StateEntryFun entryFun;
	StateFun	  stateFun;
	TransitionFun transFun;
};
//
//states_t next_state[MAX_STATE] = {
//		I2_COMM_CHECKED,
//		I3_SLED_INIT,
//		I4_MAGAZINE_INIT,
//		S1_PICKUP_BALL,
//		S2_SHOT_READY,
//		S3_SHOT_SEQUENCE,
//		I4_MAGAZINE_INIT
//};

static struct state_functions state_functions_table[MAX_STATE] = {
		{I2_COMM_CHECKED,entry_i1,NULL,transition_i1},
		{I3_SLED_INIT, NULL   ,NULL,transition_i2},
		{I4_MAGAZINE_INIT, entry_i3,sled_state_machine,transition_i3},
		{S1_PICKUP_BALL, entry_i4,NULL,transition_i4},
		{S2_SHOT_READY, entry_s1,sled_state_machine,transition_s1},
		{S3_SHOT_SEQUENCE, NULL    ,NULL,transition_s2},
		{I4_MAGAZINE_INIT,entry_s3,sled_state_machine,transition_s3}

};

static struct machine_state cur_machine_state[MACHINE_COUNT] = {
		{0,true,true, 0,0,0}, // as long as master is local it never needs ack/is ready from start
		{1,true,true,0,0,0},
		{2,true,true,0,0,0},
		{3,true,true,0,0,0}
};

void trigger_init_procedure(void) {
	init_trigger = true;
}

void trigger_shot_procedure(void) {
	shot_trigger = true;
}

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
		if(!cur_machine_state[i].ready) {
			return false;
		}
	}
	return true;
}

static inline bool all_acknowledged(void) {
	for(int i=0;i<MACHINE_COUNT;++i) {
		if(!cur_machine_state[i].ack) {
			return false;
		}
	}
	return true;
}

static void missing_ack_callback(void *args) {
	if(args != NULL) {
//		uint8_t id = *((uint8_t*)args);
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
//		cur_machine_state[i].ack_timer = SYSTIMER_CreateTimer(TIMER_1SEC*2,
//				SYSTIMER_MODE_ONE_SHOT,missing_ack_callback,&(cur_machine_state[i].id));
	}
	set_cmd_ready_callback(set_ready);
	set_cmd_busy_callback(set_busy);
	set_cmd_ack_callback(set_ack);
}

states_t state_machine(states_t state) {
	static bool entered = false;
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
	signal_get_status(ID_SLAVE_2);
//	signal_get_status(ID_SLAVE_3); for now this one will not be used
}
void entry_i2(){}
void entry_i3(){
	set_busy(ID_MASTER);
	master_control_init();
}

void entry_i4(){
	set_busy(ID_SLAVE_1);
	signal_start(ID_SLAVE_1,1);
//	set_busy(ID_SLAVE_2);
//	signal_start(ID_SLAVE_2,0);
	//	start_ack_timer(ID_SLAVE_1);
	//	start_ack_timer(ID_SLAVE_2);
}
void entry_s1(){
	set_busy(ID_MASTER);
	master_control_get_ball_sequence();
}
void entry_s2(){}
void entry_s2a(){}
void entry_s2b(){}
void entry_s3(){
	master_control_start_shot_sequence();
}
void entry_sr(){}

//void state_i3() {
//	sled_state_machine();
//}
//
//void state_s1() {
//	sled_state_machine();
//}

bool transition_i1(){
	return all_acknowledged() && all_ready();
}

bool transition_i2(){
	return init_trigger;
}

bool transition_i3(){
	return all_ready();
}
bool transition_i4(){ return all_ready(); }
bool transition_s1(){ return all_ready(); }
bool transition_s2() {
	if(shot_trigger) {
		shot_trigger = false;
		return true;
	}
	return false;
}
bool transition_s2a(){ return false; }
bool transition_s2b(){ return false; }
bool transition_s3(){ return all_ready(); }
bool transition_sr(){ return false; }

void master_control_waiting() {
	set_ready(ID_MASTER);
}

void master_control_shot_ready() {
	// ready to shoot
	set_ready(ID_MASTER);
}

void master_control_shot_done() {
	// right time to trigger magazine
	set_ready(ID_MASTER);
}
