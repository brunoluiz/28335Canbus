#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "types.h"
#include "definitions.h"
#include "can.h"

#define CPU_FRQ_150MHZ 1

// CAN settings ========================================
#define BROADCAST_ID	0x00
#define NODE_ID			  0x01
#define DATA_MAP		  "data.h"

// Serial settings ========================================
#define CAN_BAUDRATE 	38400

typedef enum {
	MB_FUNC_READ_COIL = 1,
	MB_FUNC_READ_INPUT,
	MB_FUNC_READ_HOLDINGREGISTERS,
	MB_FUNC_READ_INPUTREGISTERS,
	MB_FUNC_FORCE_COIL,
	MB_FUNC_WRITE_HOLDINGREGISTER,
	MB_FUNC_FORCE_NCOILS = 15,
	MB_FUNC_WRITE_NREGISTERS = 16
} ModbusFunctionCode;

#endif
