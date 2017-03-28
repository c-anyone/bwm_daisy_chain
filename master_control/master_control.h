/*
 * master_control.h
 *
 *  Created on: Mar 27, 2017
 *      Author: faebsn
 */

#ifndef MASTER_CONTROL_H_
#define MASTER_CONTROL_H_

void master_control_init(void);
void master_control_get_ball_sequence(void);
void master_control_start_shot_sequence(void);

extern void master_control_waiting();
extern void master_control_shot_ready();
extern void master_control_shot_done();

#endif /* MASTER_CONTROL_H_ */
