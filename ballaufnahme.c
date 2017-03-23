/*
 * ballaufnahme.c
 *
 *  Created on: Mar 23, 2017
 *      Author: faebsn
 */

#include <DAVE.h>
#include "ballaufnahme.h"

volatile uint32_t endschalter_oben = 800;
volatile uint32_t endschalter_unten = 100;

volatile XMC_VADC_RESULT_SIZE_t result;
volatile XMC_VADC_RESULT_SIZE_t filter =0;

volatile uint32_t pin_status_neg =1;
volatile uint32_t pin_status_pos =1;

void ballaufnahme_init(void) {
	ADC_MEASUREMENT_StartConversion(&ADC_MEASUREMENT_0);
}

void ballaufnahme_move_neg(void)
{
	if ((pin_status_neg ==1 && pin_status_pos ==1))
	{
		pin_status_neg=0;
	}
}

void ballaufnahme_move_pos(void)
{
	if ((pin_status_pos ==1 && pin_status_neg ==1))
	{
		pin_status_pos=0;
	}
}

void ballaufnahme_worker(void) {
		result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_A_handle);
		filter -= (filter-result)*0.05;

		//State machine ballauswurf
		/*
		 * 	ballhalter in unterste Stellung bringen
		 * 	Linearachse in Position 2
		 * 	revolver positionieren
		 * 	Linearachse in Position 1
		 * 	ballhalter in obere position
		 * 	Drehzahl check walzen (Analog Ausgang)
		 * 	Ball aus halterung nehmen / Linearache pos2
		 * 	Ball mit linearachse in walzen fördern
		 */
		//	pin_status_neg = DIGITAL_IO_GetInput(&DIGITAL_IO_move_neg);
		//	pin_status_pos = DIGITAL_IO_GetInput(&DIGITAL_IO_move_pos);

		if ((pin_status_neg ==0 && pin_status_pos ==1))
		{
			if (filter > endschalter_unten)
			{
				DIGITAL_IO_SetOutputLow(&TLE_DC_direction);
				DIGITAL_IO_SetOutputHigh(&TLE_DC_disable);
				DIGITAL_IO_SetOutputLow(&TLE_DC_inhibit);
			}
			else
			{
				DIGITAL_IO_SetOutputLow(&TLE_DC_disable);
				pin_status_neg=1;

			}
		}
		if (pin_status_pos ==0 && pin_status_neg ==1)
		{
			if (filter < endschalter_oben)
			{
				DIGITAL_IO_SetOutputHigh(&TLE_DC_direction);
				DIGITAL_IO_SetOutputHigh(&TLE_DC_disable);
				DIGITAL_IO_SetOutputLow(&TLE_DC_inhibit);
			}
			else
			{
				DIGITAL_IO_SetOutputLow(&TLE_DC_disable);
				pin_status_pos=1;

			}
		}
}
