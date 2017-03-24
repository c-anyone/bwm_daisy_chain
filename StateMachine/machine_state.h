/*
 * machine_state.h
 *
 *  Created on: Mar 24, 2017
 *      Author: faebsn
 */

#ifndef MACHINE_STATE_H_
#define MACHINE_STATE_H_

typedef struct MachineState* MachineStatePtr;

typedef void (*EventStartFunc)(MachineStatePtr);
typedef void (*EventStopFunc)(MachineStatePtr);

void defaultImplementation(MachineStatePtr);

struct MachineState {
	EventStartFunc start;
	EventStopFunc stop;
};

enum StateMachineEnum {
	STATE_1,
	STATE_2
};

#endif /* MACHINE_STATE_H_ */
