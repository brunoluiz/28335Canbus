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
		can.clear();
		can.listen(&can);
		can.process(&can);
		can.send(&can);
	}
}
