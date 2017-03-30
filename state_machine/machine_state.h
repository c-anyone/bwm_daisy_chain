/*
 * machine_state.h
 *
 *  Created on: Mar 24, 2017
 *      Author: Fabio Pungg
 */

#ifndef MACHINE_STATE_H_
#define MACHINE_STATE_H_

#include <DAVE.h>
#include <stdbool.h>

typedef enum {
    INIT_PHASE_ONE=0,
    INIT_PHASE_TWO,
    ROTATE_MAGAZINE,
    BALL_INTAKE,
    SHOT_READY,
    SHOOTING,
    NUM_STATES
} machine_state_t;

typedef void(*StateEntryFun)();
typedef void(*StateFun)();
typedef bool(*TransitionFun)();

typedef struct {
	machine_state_t cur_state;
	machine_state_t next_state;
	StateEntryFun entryFun;
	StateFun	  stateFun;
	TransitionFun transFun;
}state_t;

/**
 *  function to be called in main loop,
 *  handles state transitions and state working functions
 */
machine_state_t state_machine(machine_state_t);


void machine_state_init_done(uint8_t id);
void machine_state_set_ready (uint8_t id);
void machine_state_set_busy (uint8_t id);
void machine_state_external_trigger(void);


#endif /* MACHINE_STATE_H_ */
