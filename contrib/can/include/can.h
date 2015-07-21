#ifndef CAN_H_
#define CAN_H_

#include "types.h"
#include "data.h"

typedef struct {
	Uint16 id;
	Uint16 addr;
	Uint16 func;

	Uint16 content[8];
	Uint16 size;
} can_content_t;

typedef struct can_t can_t;

struct can_t {
	Uint32 baudrate;
	data_t data;

	can_content_t request;
	can_content_t response;

	int  (*init)(can_t *self);
	void (*clear)();

	int  (*write)(Uint32 id, Uint16 * data, unsigned short  size);
	
	int  (*listen)(can_t *self);
	int  (*process)(can_t *self);
	int  (*send)(can_t *self);
	
	
};

inline int can_init(can_t *self);
inline void can_clear();

inline int can_write(Uint32 id, Uint16 * data, unsigned short  size);

inline int can_listen(can_t *self);
inline int can_process(can_t *self);
inline int can_send(can_t *self);

can_t construct_can();

#endif
