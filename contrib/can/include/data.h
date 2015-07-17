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
};

data_t construct_data();

#endif /* MODBUSVARMAP_H_ */
