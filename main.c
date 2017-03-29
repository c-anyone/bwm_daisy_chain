/*
 * main.c
 *
 *  Created on: 2017 Mar 22 14:22:12
 *  Author: Fabio Pungg
 */

#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

#include <stdbool.h>

#include "daisy_chain/daisy_wrapper.h"
#include "edison/edison_wrapper.h"
//#include "state_machine/machine_state.h"
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
.command = CMD_UNDEFINDED,
.payload = 0xAFFE,
.sender_id = ID_MASTER
};

//machine_state_t current_machine_state = INIT_PHASE_ONE;
bool motorboard_ready_status[MACHINE_COUNT] = {
		false,false,false,false
};


int main(void)
{
  DAVE_STATUS_t status;

  status = DAVE_Init();           /* Initialization of DAVE APPs  */

  if(status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");

    while(1U)
    {
    }
  }

  master_control_init();

  XMC_USIC_CH_RXFIFO_Flush(DAISY.channel);
  XMC_USIC_CH_RXFIFO_Flush(EDISON.channel);

  /* Placeholder for user application code. The while loop below can be replaced with user application code. */
  while(1U)
  {
	edison_rx_polling();
  	daisy_rx_polling();

  	// could be moved to corresponding state
	ball_intake_worker();

//  	current_machine_state = state_machine(current_machine_state);
  }
}

void daisy_busy_received(uint8_t sender_id) {
	// memorize the working status somewhere, master only!
//	machine_state_set_busy(sender_id);
}

void daisy_ready_received(uint8_t sender_id){
	// handle the message from a slave in a state machine, master only!
//	machine_state_set_ready(sender_id);
}
void daisy_start_received(){
	// start the necessary transitions and signal to master with
	// daisy_send_ready();
}

void daisy_undefined_command(daisy_command_t cmd) {
	// evaluate the undefined command here and act accordingly
}

void test_communication(void) {
	daisy_transmit_buffer(ID_MASTER,(uint8_t*)&com,sizeof(com));
}

void test_ball_intake(uint8_t command) {
	switch(command) {
	case 0x11:
		master_control_start_shot_sequence();
		break;
	case 0x12:
		master_control_get_ball_sequence();
		break;
	}
}

void master_control_waiting() {
	// sled is done initializing, rotate magazine and trigger ball intake
}

void master_control_shot_ready() {
	// ready to shoot
}

void master_control_shot_done() {
	// right time to trigger magazine
}
