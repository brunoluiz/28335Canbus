#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "types.h"
#include "definitions.h"
#include "can.h"

#define CPU_FRQ_150MHZ 1

// CAN settings ========================================
#define BROADCAST_ID	0x00
#define NODE_ID			0x01
#define DATA_MAP		"data.h"

// Serial settings ========================================
#define CAN_BAUDRATE 	38400

#endif
