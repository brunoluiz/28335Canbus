/*
 * ModbusVarMap.h
 *
 *  Created on: 08/10/2014
 *      Author: bds
 */

#ifndef DATA_H_
#define DATA_H_

#include "types.h"

typedef struct data_t data_t;

struct data_t {
	float32 dummy0;
	float32 dummy1;
	float32 dummy2;
	float32 dummy3;
	float32 dummy4;
	float32 dummy5;
	float32 dummy6;
	float32 dummy7;
	float32 dummy8;
	float32 dummy9;
	float32 dummy10;
};

data_t construct_data();

#endif /* MODBUSVARMAP_H_ */
