#include "DSP28x_Project.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_ECan.h"
#include "settings.h"
#include "can.h"

int can_init(can_t *self) {
	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();

	// Step 2. Initalize GPIO:
	// This example function is found in the DSP2833x_Gpio.c file and
	// illustrates how to set the GPIO to it's default state.
	// InitGpio();  // Skipped for this example

	// For this example, configure CAN pins using GPIO regs here
	// This function is found in DSP2833x_ECan.c
	InitECanGpio();

	// Step 3. Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	DINT;

	// Initialize PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	// This function is found in the DSP2833x_PieCtrl.c file.
	InitPieCtrl();

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
	// This function is found in DSP2833x_PieVect.c.
	InitPieVectTable();

	InitECana();

	// User inits
	// Configure Mailbox 0 as a Transmit mailbox and Mailbox 1 as Receive mailbox
	// ECanaShadow.CANMD.all = 0xFFFFFFFE;
	// ECanaShadow.CANMD.all = 0x00000000;

	// // Enable Mailbox 0 and 1 
	// ECanaShadow.CANME.all = 0x00000003;

	// // Enable interruptions
	// EALLOW;
 //    ECanaRegs.CANMIM.all = 0xFFFFFFFF;
 //    EDIS;

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

int can_write(Uint32 id, unsigned short * data, unsigned short  size) {
	struct ECAN_REGS ECanaShadow;
	int i = 0;
	unsigned long content_h = 0;
	unsigned long content_l = 0;

	ECanaShadow.CANME.all = 0;
	ECanaShadow.CANME.bit.ME0 = 0;             // Set TRS for mailbox 0
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	// SET: CAN arbitary ID
	ECanaMboxes.MBOX0.MSGID.bit.IDE = 1;
	ECanaMboxes.MBOX0.MSGID.all = id | ((Uint32)1 << 31);

	ECanaShadow.CANME.all = 0;
	ECanaShadow.CANME.bit.ME0 = 1;             // Set TRS for mailbox 0
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
//	volatile struct MBOX *Mailbox;
//	Mailbox = &ECanaMboxes.MBOX0;
//	mdl = Mailbox->MDL.all; // = 0x9555AAAn (n is the MBX number)
//	mdh = Mailbox->MDH.all; // = 0x89ABCDEF (a constant)
//	id = Mailbox->MSGID.all;// = 0x9555AAAn (n is the MBX number)
//	size = Mailbox->MSGID.all;// = 0x9555AAAn (n is the MBX number)

	self->request.id = 0;
	if ( (self->request.id != NODE_ID) || (self->request.id != BROADCAST_ID) ) 
		return -1;

	// TODO: Implement how to get those data from mailbox
	self->request.addr = 0;
	self->request.op_code = 0;
	self->request.content[0] = 0;
	self->request.size = 0;

	return 1;
}

int can_process(can_t *self) {
	Uint16 * memAddr;
	char * registersPtr;

	registersPtr = (char *)&(self->data);

	// READ REGISTERS
	if (self->request.op_code == OP_READ_REG) {
		self->response.addr    = self->request.addr;
		self->response.id      = self->request.id;
		self->response.op_code = OP_WRITE_REG;

		// TODO: Implement READ
//		self->response.content = *(registersPtr + self->request.addr);
		self->response.size    = self->request.size;
		
		return 1;
	}
	// WRITE TO REGISTERS
	else if (self->request.op_code == OP_WRITE_REG) {

		self->response.addr    = self->request.addr;
		self->response.id      = self->request.id;
		self->response.op_code = OP_WRITE_REG;

		// TODO: Implement WRITE
		memAddr = (Uint16 *) (registersPtr + self->request.addr);
		*(memAddr) = (Uint16)(self->request.content);
		self->response.size    = self->request.size;
		return 1;
	}

	return -1;
}

int can_send(can_t *self) {
	int size;
	Uint64 id = 0;

	id |= (self->response.addr);
	id |= ((Uint64)self->response.id << 16);
	id |= ((Uint64)self->response.op_code << 24);

	return can_write((Uint32)id, self->response.content, self->response.size);
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
