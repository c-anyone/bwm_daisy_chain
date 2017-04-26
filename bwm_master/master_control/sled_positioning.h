/*
 * sled_positioning.h
 *
 *  Created on: Mar 27, 2017
 *      Author: Fabio Pungg
 */

#ifndef SLED_POSITIONING_H_
#define SLED_POSITIONING_H_

void sled_init(void);
void sled_move_pos0(void);
void sled_move_shot_ready(void);
void sled_move_waiting(void);
void sled_move_shoot(void);

extern void sled_position_reached(void);
extern void sled_limit_switch(void);

#endif /* SLED_POSITIONING_H_ */
