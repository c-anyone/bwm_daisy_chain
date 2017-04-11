/*
 * global_definitions.h
 *
 *  Created on: Apr 11, 2017
 *      Author: faebsn
 */

#ifndef GLOBAL_DEFINITIONS_H_
#define GLOBAL_DEFINITIONS_H_

#define EDISON_INIT			(0x10u)
#define EDISON_SHOOT		(0x11u)
#define EDISON_ELEVATION 	(0x13)
#define EDISON_SPEED		(0x15u)

#define ID_SLAVE_SPEED		(0x02u)
#define ID_SLAVE_ELEVATION	(0x01u)
#define ID_SLAVE_MAGAZINE	(0x01u)

#define PARAM_INIT		(0xffu)
#define PARAM_SPEED		(0x00u)
#define PARAM_ELEVATION	(0x00u)
#define PARAM_MAGAZINE	(0x01u)
#define PARAM_START_FLY	(0x00u)
#define PARAM_STOP_FLY	(0x01u)
#define PARAM_PLAY_WAV	(0x02u)


#endif /* GLOBAL_DEFINITIONS_H_ */
