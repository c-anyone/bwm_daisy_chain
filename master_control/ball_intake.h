/*
 * ball_intake.h
 *
 *  Created on: Mar 23, 2017
 *      Author: Fabio Pungg
 */

#ifndef BALL_INTAKE_H_
#define BALL_INTAKE_H_

// init function for ball intake
void ball_intake_init(void);

// lower or raise the intake
void ball_intake_lower(void);
void ball_intake_raise(void);

// worker function, has to be called in main loop
void ball_intake_worker(void);

extern void ball_intake_ready();

#endif /* BALL_INTAKE_H_ */
