#ifndef touch_h
#define touch_h

#define STATE_NONE 		    0x00
#define STATE_TOUCH_DOWN 	0x01
#define STATE_TOUCH_UP 		0x02

// #define NONBLOCK_READ

struct touch_event {
	int x;
	int y;
	int pressure;
	int touch_state;
};

/**
 * QAUTION: NO EXCEPTION HANDLING WHEN UNSAFE MODE.
 * BE VERY CAREFUL WHEN USING THIS FUNCTION.
 */
int touch_read(int fd, struct touch_event *event);


#endif /* touch_h */
