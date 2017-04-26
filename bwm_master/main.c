/*
 * main.c
 *
 *  Created on: 2017 Mar 22 14:22:12
 *  Author: Fabio Pungg
 */

#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

#include <stdbool.h>

#include "daisy_chain/application_layer.h"
#include "edison/edison_wrapper.h"
#include "state_machine/new_state_machine.h"
#include "master_control/sled_positioning.h"
#include "master_control/master_control.h"
#include "master_control/ball_intake.h"
#include "daisy_chain/global_definitions.h"

// Master  ID = 0x00
// Slave 1 ID = 0x01
// Slave 2 ID = 0x02
// Slave 3 ID = 0x03

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */
daisy_command_t com = {
		.command = CMD_UNDEFINED,
		.sender_id = ID_MASTER
};

//machine_state_t current_machine_state = INIT_PHASE_ONE;
bool motorboard_ready_status[MACHINE_COUNT] = {
		false,false,false,false
};

states_t current_state = I1_POWERED_UP;
uint32_t shot_trigger_timer = 0;

int main(void)
{
	DAVE_STATUS_t status;

	status = DAVE_Init();           /* Initialization of DAVE APPs  */

	for(int i=0;i<0xFFFF;++i) {
		__NOP();
	}

	if(status != DAVE_STATUS_SUCCESS)
	{
		/* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
		XMC_DEBUG("DAVE APPs initialization failed\n");

		while(1U)
		{
		}
	}

	XMC_USIC_CH_RXFIFO_Flush(DAISY.channel);
	XMC_USIC_CH_RXFIFO_Flush(EDISON.channel);

	if(shot_trigger_timer == 0)
		shot_trigger_timer = SYSTIMER_CreateTimer(3000000,SYSTIMER_MODE_ONE_SHOT,(void*)trigger_shot_procedure,NULL);

	application_layer_init();
	state_machine_init();

	WATCHDOG_Start();
	/* Placeholder for user application code. The while loop below can be replaced with user application code. */
	while(1U)
	{
		edison_rx_polling();
		application_worker();

		// could be moved to corresponding state
		ball_intake_worker();

		current_state = state_machine(current_state);

		WATCHDOG_Service();
	}
}

void test_command(uint8_t command, uint32_t payload) {
	switch(command) {
	case EDISON_INIT:
		if(current_state == I2_COMM_CHECKED) {
			trigger_init_procedure();
			signal_set(ID_SLAVE_2,PARAM_SPEED,0);

		} else {
			signal_start(ID_SLAVE_1,0xff);
			signal_set(ID_SLAVE_2,PARAM_SPEED,0);
			//			signal_start(ID_SLAVE_2,0xff); // enable of fu slave now only set on speed > 0
			current_state = I2_COMM_CHECKED;
			trigger_init_procedure();
		}
		break;
	case EDISON_SHOOT:
		// payload will be time in ms
		if(current_state == S2_SHOT_READY) {
			if(payload > 0) {
				SYSTIMER_RestartTimer(shot_trigger_timer,payload*1000);
			} else {
				trigger_shot_procedure();
			}
			signal_start(ID_SLAVE_2,2);
		}
		break;
	case EDISON_ELEVATION:
		if (current_state != S3_SHOT_SEQUENCE) {
			signal_set(ID_SLAVE_1,PARAM_ELEVATION,payload);
		}
		break;
	case EDISON_AZIMUT:
		break;

	case EDISON_SPEED:
		if (current_state != S3_SHOT_SEQUENCE) {
			if(payload == 0) {
				// send a stop signal
				//				signal_start(ID_SLAVE_2,PARAM_STOP_FLY);
				signal_set(ID_SLAVE_2,PARAM_SPEED,0);
				// set speed to 0
			} else {
				uint32_t speed = (payload*40) & 0x0000ffff;
				signal_set(ID_SLAVE_2,PARAM_SPEED,speed);
			}
		}
		break;
	}
}

