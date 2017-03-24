/*
 * machine_state.c
 *
 *  Created on: Mar 24, 2017
 *      Author: faebsn
 */

#include "machine_state.h"

static void defaultStop(MachineStatePtr state) {

}

static void defaultStart(MachineStatePtr state) {

}

void defaultImplementation(MachineStatePtr state) {
	state->start = defaultStart;
	state->stop = defaultStop;
}
