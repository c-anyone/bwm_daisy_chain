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
//#include "state_machine/machine_state.h"
#include "master_control/sled_positioning.h"
#include "master_control/master_control.h"
#include "master_control/ball_intake.h"
//#include "master_control/sled_positioning.h"

// Master  ID = 0x01
// Slave 1 ID = 0x02
// Slave 2 ID = 0x03
// Slave 3 ID = 0x04

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

states_t cur_machine_state = I1_POWERED_UP;

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

		cur_machine_state = state_machine(cur_machine_state);

		WATCHDOG_Service();
		//  	current_machine_state = state_machine(current_machine_state);
	}
}


void test_command(uint8_t command) {
	switch(command) {
	case 0x10:
		//		master_control_get_ball_sequence();
		trigger_init_procedure();
		break;
	case 0x11:
		//		master_control_start_shot_sequence();
		trigger_shot_procedure();
		break;
	case 0x13:
		sled_move_waiting();
		break;
	case 0x14:
		sled_move_shot_ready();
		break;
	}
}

