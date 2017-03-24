/*
 * machine_state.h
 *
 *  Created on: Mar 24, 2017
 *      Author: fapungg
 */

#ifndef MACHINE_STATE_H_
#define MACHINE_STATE_H_

typedef enum {
    INIT,
    WAITING,
    INTAKE_POS,
    INTAKE_READY,
    SHOT_READY,
    SHOT_FIRED,
    NUM_STATES
} machine_state_t;

typedef void(*StateEntryFun)();
typedef void(*StateFun)();
typedef machine_state_t(*TransitionFun)();


typedef struct {
	machine_state_t current_state;
	machine_state_t next_state;
	StateFun stateFun;
	TransitionFun transFun;
}state_t;



void state_machine(machine_state_t*);

/**
 * State Functions
 */
void init();
void waiting();
void intake_pos();
void intake_ready();
void shot_ready();
void shot_fired();

/**
 * Transition Functions
 */
void init_to_waiting();
void waiting_to_intake_pos();
void intake_pos_to_ready();
void intake_ready_to_shot_ready();
void shot_ready_to_fired();
void shot_fired_to_waiting();

state_t states[NUM_STATES] = {
		{INIT, WAITING, &init, &init_to_waiting},
		{WAITING, INTAKE_POS, &waiting, &waiting_to_intake_pos},
		{INTAKE_POS, INTAKE_READY, &intake_pos, &intake_pos_to_ready},
		{INTAKE_READY, SHOT_READY, &intake_ready, &intake_ready_to_shot_ready},
		{SHOT_READY, SHOT_FIRED, &shot_ready, &shot_ready_to_fired},
		{SHOT_FIRED, SHOT_READY, &shot_fired, &shot_fired_to_waiting}

};

machine_state_t current_state = INIT;

#endif /* MACHINE_STATE_H_ */
