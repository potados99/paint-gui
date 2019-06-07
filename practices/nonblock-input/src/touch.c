#include "touch.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h> /* struct input_event */
#include <linux/input-event-codes.h> /* EV_ABS, ABS_X ... */
#include <errno.h>

int touch_read(int fd, struct touch_event *event, struct touch_correction *correction) {
	if (event == NULL) return -1;

	struct input_event 	ie;
	int 			done = 0; /* initialization necessary! */

	int 			x = 0;
	int 			y = 0;
	int 			p = 0;

	while (done != (READ_X | READ_Y | READ_P)) {

		if (read(fd, &ie, sizeof(struct input_event)) < 0) {
			if (errno == EAGAIN) {
				return 1;
			}
			perror("read error");
			return -1;
		}

		if (ie.type == EV_ABS) {
			switch (ie.code) {
				case ABS_X:
					x = (TS_SIZE - ie.value);
					done |= READ_X;
					break;

				case ABS_Y:
					y = ie.value;
					done |= READ_Y;
					break;

				case ABS_PRESSURE:
					p = ie.value;
					done |= READ_P;
					break;
			
			} /* end of switch */
		} /* end of if */
	} /* end of while */

	/* No correction. */
	if (correction == NULL) {
		event->x = x;
		event->y = y;
		event->pressure = p;
		
		return 0;
	}

	/* Do touch correction. */ 
	event->x = (correction->xd_coef_x * x) + (correction->xd_coef_y * y) + correction->xd_coef_1;
	event->y = (correction->yd_coef_x * x) + (correction->yd_coef_y * y) + correction->yd_coef_1;
	event->pressure= p;

	return 0;
}

