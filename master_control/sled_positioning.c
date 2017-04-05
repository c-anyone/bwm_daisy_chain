

#include <DAVE.h>
#include <stdbool.h>
#include "sled_positioning.h"
#define TIMER_5MS	(5000u)
#define TIMER_50MS	(50000u)

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

static void sled_reactivate_servo(void);

static uint32_t timer_trigger_5ms;
static uint32_t timer_servo_50ms;
static uint32_t timer_home_pos_5ms;

// sets the servo to on
void sled_init(void){
	DIGITAL_IO_SetOutputHigh(&SLED_SON);
	timer_trigger_5ms = SYSTIMER_CreateTimer(TIMER_5MS,SYSTIMER_MODE_ONE_SHOT,(void*)trigger_sequence,NULL);
	timer_home_pos_5ms = SYSTIMER_CreateTimer(TIMER_5MS,SYSTIMER_MODE_ONE_SHOT,(void*)sled_limit_switch,NULL);
	timer_servo_50ms = SYSTIMER_CreateTimer(TIMER_50MS,SYSTIMER_MODE_ONE_SHOT,(void*)sled_reactivate_servo,NULL);
}

void sled_move_pos0(void){
	if(PIN_INTERRUPT_GetPinValue(&SLED_LIMIT_SWITCH_INTERRUPT) == 0) {
		sled_limit_switch();
	} else {
		// moves the sled from end to start

		//		PIN_INTERRUPT_Disable(&SLED_POSITION_INTERRUPT);

		DIGITAL_IO_SetOutputHigh(&SLED_POS0);
		DIGITAL_IO_SetOutputHigh(&SLED_POS1);

		trigger_sequence();
	}
}

void sled_move_shot_ready(void){
	// moves from pickup position to shot ready
	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputLow(&SLED_POS1);

	trigger_sequence();
	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);

}

void sled_move_waiting(void) {
	// moves from pickup position into waiting position to safely rotate the magazine
	DIGITAL_IO_SetOutputLow(&SLED_POS1);
	DIGITAL_IO_SetOutputHigh(&SLED_POS0);

	trigger_sequence();
	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
}

void sled_move_shoot(void){
	// moves the sled with high speed to the flywheels
	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputHigh(&SLED_POS1);

	trigger_sequence();
	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
}

typedef enum {
	INPUT_SET=0,
	INPUT_TRIGGER_LOW,
	INPUT_TRIGGER_HIGH
} trigger_state_t;

static void trigger_sequence(void) {
	static trigger_state_t trigger_state = INPUT_SET;

	switch(trigger_state) {
	case INPUT_SET:
		trigger_state = INPUT_TRIGGER_LOW;
		SYSTIMER_StartTimer(timer_trigger_5ms);
		break;
	case INPUT_TRIGGER_LOW:
		trigger_state = INPUT_TRIGGER_HIGH;
		DIGITAL_IO_SetOutputHigh(&SLED_CTRG);
		SYSTIMER_StartTimer(timer_trigger_5ms);
		break;
	case INPUT_TRIGGER_HIGH:
		trigger_state = INPUT_SET;
		DIGITAL_IO_SetOutputLow(&SLED_CTRG);
		break;
	default:
		break;
	}
}

void sled_home_positionIRQ(void){
	DIGITAL_IO_SetOutputLow(&SLED_SON);
	SYSTIMER_StartTimer(timer_servo_50ms);
}

static void sled_reactivate_servo(void) {
	DIGITAL_IO_SetOutputHigh(&SLED_SON);
	SYSTIMER_StartTimer(timer_home_pos_5ms);
}


void sled_position_reachedIRQ(void){
	// notify the state machine / controller that the position is reached
	//	PIN_INTERRUPT_Disable(&SLED_POSITION_INTERRUPT);
//	if(PIN_INTERRUPT_GetPinValue(&SLED_LIMIT_SWITCH_INTERRUPT) != 0) {
		sled_position_reached();
//	}
}


