/*
 * main.c
 *
 *  Created on: 2017 Mar 22 14:22:12
 *  Author: faebsn
 */




#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)
#include "daisy_wrapper.h"
#include "global_definition.h"
#define MY_ID	(0x01)

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
daisy_command_t com;

static void daisy_command_handling();

int main(void)
{
  DAVE_STATUS_t status;

  status = DAVE_Init();           /* Initialization of DAVE APPs  */

  XMC_USIC_CH_RXFIFO_Flush(DAISY.channel);

  if(status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");

    while(1U)
    {

    }
  }

  /* Placeholder for user application code. The while loop below can be replaced with user application code. */
  while(1U)
  {
  	daisy_rx_polling();
  }
}

void daisy_received_buffer(uint8_t buf[], uint8_t control, uint8_t id) {
	if(id == MY_ID) {
//		do the what we need to
		memcpy(&com,buf,control);
		daisy_command_handling();
	} else {
//		retransmit
		daisy_transmit_buffer(id,buf,control);
	}
}

static void daisy_command_handling() {
	// do something with com
}

