

#include <DAVE.h>
#include <stdbool.h>
#include "sled_positioning.h"
#define TIMER_5MS	(5000u)
#define TIMER_100MS	(100000u)

/**
 *  Bit settings on control pins
 *  0.11 | 0.2
 *  POS1 | POS0 || distance covered
 *   0   |  0   || intake to shot ready
 *   0   |  1   || waiting position
 *   1   |  0   || shot ready to end
 *   1   |  1   || to position zero
 *
 */
static void trigger_sequence(void);

static void sled_reactivate_servo(void);
static void pos_reached_reactivate(void) {
	//	NVIC_ClearPendingIRQ(SLED_POSITION_INTERRUPT.IRQn);
	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
}

static void limit_switch_reactivate(void) {
	NVIC_ClearPendingIRQ(SLED_LIMIT_SWITCH_INTERRUPT.IRQn);
	PIN_INTERRUPT_Enable(&SLED_LIMIT_SWITCH_INTERRUPT);
}
static void sled_limit_switch_cb(void) {
	//	PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
	sled_limit_switch();
}

static uint32_t position_reached_counter = 0;
static uint32_t pos_interrupt_count = 0;
static uint32_t home_pos_count = 0;

static uint32_t timer_trigger_5ms;
static uint32_t timer_servo_50ms;
static uint32_t timer_home_pos_5ms;
static uint32_t timer_pos_interrupt;
static uint32_t timer_limit_interrupt;

// sets the servo to on
void sled_init(void){
	position_reached_counter = 0;
	pos_interrupt_count = 0;
	DIGITAL_IO_SetOutputHigh(&SLED_SON);
	timer_trigger_5ms = SYSTIMER_CreateTimer(TIMER_5MS,SYSTIMER_MODE_ONE_SHOT,(void*)trigger_sequence,NULL);
	timer_home_pos_5ms = SYSTIMER_CreateTimer(TIMER_5MS,SYSTIMER_MODE_ONE_SHOT,(void*)sled_limit_switch_cb,NULL);
	timer_servo_50ms = SYSTIMER_CreateTimer(TIMER_100MS,SYSTIMER_MODE_ONE_SHOT,(void*)sled_reactivate_servo,NULL);
	timer_pos_interrupt = SYSTIMER_CreateTimer(TIMER_100MS,SYSTIMER_MODE_ONE_SHOT,(void*)pos_reached_reactivate,NULL);
	timer_limit_interrupt = SYSTIMER_CreateTimer(TIMER_5MS*4,SYSTIMER_MODE_ONE_SHOT,(void*)limit_switch_reactivate,NULL);
}

void sled_move_pos0(void){
	if(PIN_INTERRUPT_GetPinValue(&SLED_LIMIT_SWITCH_INTERRUPT) == 0) {
		sled_limit_switch();
	} else {
		// moves the sled from end to start

		PIN_INTERRUPT_Disable(&SLED_POSITION_INTERRUPT);

		NVIC_ClearPendingIRQ(SLED_LIMIT_SWITCH_INTERRUPT.IRQn);
		PIN_INTERRUPT_Enable(&SLED_LIMIT_SWITCH_INTERRUPT);

		DIGITAL_IO_SetOutputHigh(&SLED_POS0);
		DIGITAL_IO_SetOutputHigh(&SLED_POS1);

		if(DIGITAL_IO_GetInput(&SLED_POS0) == 1
				&& DIGITAL_IO_GetInput(&SLED_POS1)) {
			trigger_sequence();
		} else {
			// handle the error (hardware fault)
		}
	}
}

void sled_move_shot_ready(void){
	// moves from pickup position to shot ready
	PIN_INTERRUPT_Disable(&SLED_POSITION_INTERRUPT);

	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputLow(&SLED_POS1);



	if(DIGITAL_IO_GetInput(&SLED_POS0) == 0
			&& DIGITAL_IO_GetInput(&SLED_POS1) == 0) {
		trigger_sequence();
		SYSTIMER_StartTimer(timer_pos_interrupt);
	} else {
		// wrong bits sets, handle error (hardware fault)
	}
}

void sled_move_waiting(void) {
	// moves from pickup position into waiting position to safely rotate the magazine
	PIN_INTERRUPT_Disable(&SLED_POSITION_INTERRUPT);
	DIGITAL_IO_SetOutputHigh(&SLED_POS0);
	DIGITAL_IO_SetOutputLow(&SLED_POS1);

	SYSTIMER_StartTimer(timer_pos_interrupt);

	if(DIGITAL_IO_GetInput(&SLED_POS0) == 1
			&& DIGITAL_IO_GetInput(&SLED_POS1) == 0) {
		trigger_sequence();
		SYSTIMER_StartTimer(timer_pos_interrupt);
	} else {
		// wrong pins set, handle this error (hardware fault)
	}
}

void sled_move_shoot(void){
	// moves the sled with high speed to the flywheels
	DIGITAL_IO_SetOutputLow(&SLED_POS0);
	DIGITAL_IO_SetOutputHigh(&SLED_POS1);
	if(DIGITAL_IO_GetInput(&SLED_POS0) == 0
			&& DIGITAL_IO_GetInput(&SLED_POS1) == 1) {
		SYSTIMER_StartTimer(timer_pos_interrupt);
		trigger_sequence();
	} else {
		// wrong pins set, handle this error (hardware fault)
	}
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
//	if(PIN_INTERRUPT_GetPinValue(&SLED_LIMIT_SWITCH_INTERRUPT) == 0) {
		home_pos_count++;
		DIGITAL_IO_SetOutputLow(&SLED_SON);
		DIGITAL_IO_SetOutputLow(&SLED_POS0);
		DIGITAL_IO_SetOutputLow(&SLED_POS1);
		PIN_INTERRUPT_Disable(&SLED_LIMIT_SWITCH_INTERRUPT);
		SYSTIMER_StartTimer(timer_servo_50ms);
//	}
}

static void sled_reactivate_servo(void) {
	static bool entered=false;
	if(!entered) {
		DIGITAL_IO_SetOutputHigh(&SLED_SON);
		SYSTIMER_StartTimer(timer_servo_50ms);
		entered = true;
	} else {
		entered = false;
		//		PIN_INTERRUPT_Enable(&SLED_POSITION_INTERRUPT);
		SYSTIMER_StartTimer(timer_home_pos_5ms);
	}
}

void sled_position_reachedIRQ(void){
	// notify the state machine / controller that the position is reached
	uint32_t limit_switch = PIN_INTERRUPT_GetPinValue(&SLED_LIMIT_SWITCH_INTERRUPT);
	pos_interrupt_count++;
	if( limit_switch == 1 && PIN_INTERRUPT_GetPinValue(&SLED_POSITION_INTERRUPT) == 0) {
		PIN_INTERRUPT_Disable(&SLED_POSITION_INTERRUPT);
		position_reached_counter++;
		sled_position_reached();
	}
}


