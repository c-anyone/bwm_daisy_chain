

#include <DAVE.h>
#include "sled_positioning.h"

/**
 *  Bit settings on control pins
 *  POS1 | POS0 || distance covered
 *   0   |  0   || intake to shot ready
 *   0   |  1   || shot ready to intake
 *   1   |  0   || shot ready to end
 *   1   |  1   || end to intake
 *
 */
static void trigger_sequence(void);

// sets the servo to on
void sled_init(void){
	DIGITAL_IO_SetOutputHigh(&SLED_SON);
}

void move_pos0(void){
	// moves the sled from end to start

}

void move_shot_ready(void){
	// moves from pickup position to shot ready
	// setup control pins
	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputLow(&SLED_POS1);

	trigger_sequence();

}

void move_shoot(void){
	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputHigh(&SLED_POS1);

	trigger_sequence();
}

static void trigger_sequence(void) {
	DIGITAL_IO_SetOutputHigh(&SLED_CTRG);
	// delay min. 2ms
	DIGITAL_IO_SetOutputLow(&SLED_CTRG);
}

void sled_position_reachedIRQ(void){
	// notify the state machine / controller that the position is reached
}
