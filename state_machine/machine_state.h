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
    INIT,
    WAITING,
    INTAKE_POS,
    INTAKE_READY,
    SHOT_READY,
    SHOT_FIRED,
    NUM_STATES
} machine_state_t;

typedef void(*StateEntryFun)();
typedef void(*StateEntryFun)();
typedef bool(*TransitionFun)();

typedef struct {
	machine_state_t cur_state;
	machine_state_t next_state;
	StateEntryFun entryFun;
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


/**
 * State Functions, might be moved to .c file
 * and made static
 */
void init();
void waiting();
void intake_pos();
void intake_ready();
void shot_ready();
void shot_fired();

/**
 * Transition Functions, might be moved to .c file
 * and made static
 */
bool init_to_waiting();
bool waiting_to_intake_pos();
bool intake_pos_to_ready();
bool intake_ready_to_shot_ready();
bool shot_ready_to_fired();
bool shot_fired_to_waiting();

#endif /* MACHINE_STATE_H_ */
