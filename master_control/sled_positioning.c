

#include <DAVE.h>
#include "sled_positioning.h"

/**
 *  Bit settings on control pins
 *  POS1 | POS0 || distance covered
 *   0   |  0   || intake to shot ready
 *   0   |  1   || waiting position
 *   1   |  0   || shot ready to end
 *   1   |  1   || to position zero
 *
 */
static void trigger_sequence(void);

// delay for 5ms, might be optimized out
static void delay() {
	for(int i=0; i<0xFFFF;++i) {
	}
}

//static void delay_long() {
//	for(int i=0; i<0x2FF;++i){
//		delay();
//	}
//}

// sets the servo to on
void sled_init(void){
	DIGITAL_IO_SetOutputHigh(&SLED_SON);
}

void sled_move_pos0(void){
	if(PIN_INTERRUPT_GetPinValue(&SLED_LIMIT_SWITCH_INTERRUPT) == 0) {
		sled_home_position();
	} else {
		// moves the sled from end to start
		DIGITAL_IO_SetOutputHigh(&SLED_POS0);
		DIGITAL_IO_SetOutputHigh(&SLED_POS1);

		trigger_sequence();
	}
}

void sled_move_shot_ready(void){
	// moves from pickup position to shot ready
	// setup control pins
	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputLow(&SLED_POS1);

	trigger_sequence();
	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
}

void sled_move_waiting(void) {
	DIGITAL_IO_SetOutputLow(&SLED_POS1);
	DIGITAL_IO_SetOutputHigh(&SLED_POS0);

	trigger_sequence();
	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
}

void sled_move_shoot(void){
	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputHigh(&SLED_POS1);

	trigger_sequence();
	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
}

static void trigger_sequence(void) {
	delay();
	DIGITAL_IO_SetOutputHigh(&SLED_CTRG);
	delay();
	DIGITAL_IO_SetOutputLow(&SLED_CTRG);

}

void sled_home_positionIRQ(void){
	DIGITAL_IO_SetOutputLow(&SLED_SON);

	delay(); delay(); delay(); delay();
	delay(); delay(); delay(); delay();
	delay(); delay(); delay(); delay();

	DIGITAL_IO_SetOutputHigh(&SLED_SON);
	delay();
	sled_home_position();
}

void sled_position_reachedIRQ(void){
	// notify the state machine / controller that the position is reached
	PIN_INTERRUPT_Disable(&SLED_POSITION_INTERRUPT);
	sled_position_reached();
}


