#include "can.h"
#include "DSP28x_Project.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_GlobalPrototypes.h"

can_t can;

int main(){
	// InitSysCtrl();

	can = construct_can();
	can.init(&can);

	can.data.dummy1 = 1;
	can.data.dummy3 = 1;

	while(1) {
		// Uint32 id = 0x0101;
		// unsigned short data[] = {0,1,2,3,4,5,6,7};

		// can.write(id, data, 6);
		// can.clear();

		can.response.id = 1;
		can.response.addr = 255;
		can.response.op_code = 1;

		can.response.content[0] = 1;
		can.response.content[1] = 0xFF;
		can.response.size = 2;

		can.send(&can);
	}
//	while(1) {
//		can.clear();
//		can.listen(&can);
//		can.process(&can);
//		can.send(&can);
//	}
}
