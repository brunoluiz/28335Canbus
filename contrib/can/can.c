#include "DSP28x_Project.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_ECan.h"
#include "settings.h"
#include "can.h"

int can_init(can_t *self) {
	InitECanGpio();
	InitECana();

    // Configure Mailboxes 0-15 as Tx, 16-31 as Rx
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanaRegs.CANMD.all = 0x2;

    // Enable all Mailboxes */
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanaRegs.CANME.all = 0x3;

    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    EALLOW;
    ECanaRegs.CANMIM.all = 0x3;
    EDIS;


	return 1;
}

void can_clear() {
	  ECanaRegs.CANME.all = 0;

		ECanaMboxes.MBOX0.MSGID.all = 0;
		ECanaMboxes.MBOX1.MSGID.all = 0;

		ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 0;
		ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 0;

		ECanaMboxes.MBOX0.MDL.all = 0;
	  ECanaMboxes.MBOX0.MDH.all = 0;
		ECanaMboxes.MBOX1.MDL.all = 0;
	  ECanaMboxes.MBOX1.MDH.all = 0;
}

int can_write(Uint32 id, Uint16 * data, unsigned short  size) {
	struct ECAN_REGS ECanaShadow;
	int i = 0;
	unsigned long content_h = 0;
	unsigned long content_l = 0;

	ECanaShadow.CANME.all = 0;
	ECanaShadow.CANME.bit.ME0 = 0;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	// SET: CAN arbitary ID
	// ECanaMboxes.MBOX0.MSGID.bit.IDE = 1;
	// ECanaMboxes.MBOX0.MSGID.all = id | ((Uint32)1 << 31);
	ECanaMboxes.MBOX0.MSGID.bit.STDMSGID = id | ((Uint32)1 << 8);

	ECanaShadow.CANME.all = 0;
	ECanaShadow.CANME.bit.ME0 = 1;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	// SET: CAN DLC/size
	ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = size;

	// SET: CAN content
	for(i = 0; i < size && i < 4; i++) {
		content_l = (content_l) | ((unsigned long long)data[i] << (3-i)*8);
	}
	for(i = 0; i < 4 && size > 4; i++) {
		content_h = (content_h) | ((unsigned long long)data[i+4] << (3-i)*8);
	}
	ECanaMboxes.MBOX0.MDL.all = content_l;
    ECanaMboxes.MBOX0.MDH.all = content_h;

    // Prepare to transmit
	ECanaShadow.CANTRS.all = 0;
	ECanaShadow.CANTRS.bit.TRS0 = 1;             // Set TRS for mailbox 0
	ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;

	// Transmit
	ECanaRegs.CANTRS.all = 0x00000001;  // Set TRS for all transmit mailboxes
	while(ECanaRegs.CANTA.all != 0x00000001 ) {}  // Wait for all TAn bits to be set..
    
    // Clear flags after transmit
	ECanaShadow.CANTA.all = 0;
	ECanaShadow.CANTA.bit.TA0 = 1;     	         // Clear TA0
	ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;

    return 1;
}

int can_listen(can_t *self) {
	struct ECAN_REGS ECanaShadow;

	ECanaShadow.CANME.all = 0;
	ECanaShadow.CANME.bit.ME1 = 0;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	// SET: CAN arbitary ID: extended ID
	ECanaMboxes.MBOX1.MSGID.bit.STDMSGID = NODE_ID;

	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;	
	ECanaShadow.CANMD.all = 0x02;
	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

	ECanaShadow.CANME.all = 0;
	ECanaShadow.CANME.bit.ME1 = 1;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	ECanaShadow.CANRMP.all = 0;
	ECanaShadow.CANRMP.bit.RMP1 = 1;     	         // Clear TA0
	ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;

	while(ECanaRegs.CANRMP.all != 0x00000002) { }
	
    // Clear flags after transmit
	ECanaShadow.CANRMP.all = 0;
	ECanaShadow.CANRMP.bit.RMP1 = 1;     	         // Clear TA0
	ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;

	self->request.addr = 0;

	// // TODO: Implement how to get those data from mailbox
	self->request.func = (ECanaMboxes.MBOX1.MDL.byte.BYTE0);
	self->request.addr = ((unsigned int)ECanaMboxes.MBOX1.MDL.byte.BYTE1 << 8) | ECanaMboxes.MBOX1.MDL.byte.BYTE2;
	self->request.content[0] = ECanaMboxes.MBOX1.MDL.byte.BYTE3;
	self->request.content[1] = ECanaMboxes.MBOX1.MDH.byte.BYTE4;
	self->request.size = ECanaMboxes.MBOX1.MSGCTRL.bit.DLC;

	return 1;
}

int can_process(can_t *self) {
	Uint16 * memAddr;
	char * registersPtr;

	registersPtr = (char *)&(self->data);

	if (self->request.addr >= sizeof(self->data)) {
		self->response.id = NODE_ID;
		self->response.func = 0x80 | self->request.func;

		self->response.size    = self->request.size;

		// TODO: Implement READ
		self->response.content[0] = 0x02;
		self->response.size    = 2;
		
		return 1;
	}

	// READ REGISTERS
	if (self->request.func == MB_FUNC_READ_HOLDINGREGISTERS) {
		unsigned short readData;

		self->response.id = NODE_ID;
		self->response.addr = self->request.addr;
		self->response.func = self->request.func;

		self->response.size    = self->request.size;

		// TODO: Implement READ
		readData = *(registersPtr + self->request.addr);
		self->response.content[0] = (readData & 0xFF00) >> 8;
		self->response.content[1] = (readData & 0x00FF);
		self->response.size    = 5;
		
		return 1;
	}
	// WRITE TO REGISTERS
	else if ( (self->request.func == MB_FUNC_WRITE_NREGISTERS) || (self->request.func == MB_FUNC_WRITE_HOLDINGREGISTER) ){
		Uint16 dataToWrite = (self->request.content[0] << 8) | self->request.content[1];

		self->response.id = NODE_ID;
		self->response.addr = self->request.addr;
		self->response.func = self->request.func;
		self->response.content[0] = self->request.content[0];
		self->response.content[1] = self->request.content[1];

		memAddr = (Uint16 *) (registersPtr + self->request.addr);
		*(memAddr) = (Uint16)(dataToWrite);
		self->response.size    = self->request.size;

		return 1;
	}

	return -1;
}

int can_send(can_t *self) {
	Uint16 formatedResponse[8];

	if ((self->response.func & 0xF0) == 0x80) {
		formatedResponse[0] = self->response.func;
		formatedResponse[1] = self->response.content[0];
		return can_write(self->response.id, formatedResponse, 2);
	}

	formatedResponse[0] = self->response.func;
	formatedResponse[1] = (self->response.addr & 0xFF00) >> 8;
	formatedResponse[2] = (self->response.addr & 0x00FF);
	formatedResponse[3] = self->response.content[0];
	formatedResponse[4] = self->response.content[1];

	return can_write(self->response.id, formatedResponse, 5);
}


can_t construct_can(){
	can_t can;

	can.baudrate = 0;
	can.data = construct_data();

	// can_content_t request;
	// can_content_t response;

	can.init = can_init;
	can.clear = can_clear;

	can.write = can_write;
	
	can.listen = can_listen;
	can.process = can_process;
	can.send = can_send;

	return can;
}
