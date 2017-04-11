/*
 * ball_intake.c
 *
 *  Created on: Mar 23, 2017
 *      Author: Fabio Pungg
 */

#include <DAVE.h>
#include "ball_intake.h"

volatile uint32_t endschalter_oben = 3500;
volatile uint32_t endschalter_unten = 2960;

volatile XMC_VADC_RESULT_SIZE_t result;
volatile XMC_VADC_RESULT_SIZE_t filter =0;

volatile uint32_t pin_status_raise =1;
volatile uint32_t pin_status_lower =1;

void ball_intake_init(void) {
	ADC_MEASUREMENT_StartConversion(&ADC_MEASUREMENT_0);
	DIGITAL_IO_SetOutputLow(&TLE_DC_disable);
}

void ball_intake_raise(void)
{
	if ((pin_status_raise ==1 && pin_status_lower ==1))
	{
		pin_status_raise=0;
	}
}

void ball_intake_lower(void)
{
	if ((pin_status_lower ==1 && pin_status_raise ==1))
	{
		pin_status_lower=0;
	}
}

void ball_intake_worker(void) {
		result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_A_handle);
		filter -= (filter-result)*0.05;

		if ((pin_status_raise ==0 && pin_status_lower ==1))
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
				// callback, intake moving done
				pin_status_raise=1;
				ball_intake_ready();
			}
		}
		if (pin_status_lower ==0 && pin_status_raise ==1)
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
				// callback, intake moving done
				pin_status_lower=1;
				ball_intake_ready();
			}
		}
}
