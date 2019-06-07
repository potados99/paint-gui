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

	int			nread = 0;
	int 			x = 0;
	int 			y = 0;

	/**
	 * Clean touch_event struct
	 */	
	event->touch_state = STATE_NONE; /* default none. */

	while (1) {

		/**
		 * Single read
		 */
		nread = read(fd, &ie, sizeof(struct input_event));
	        if (nread == -1){
			if (errno == EAGAIN) {
				return 1;
			}
			else {
				perror("read error");
				return -1;
			}
		}

		/**
		 * Process read values
		 */
		if (ie.type == EV_SYN) {
			break; /* time to return! */
		}
		else if (ie.type == EV_KEY) {
			if (ie.code == BTN_TOUCH) {
				/**
				 * Of cource BTN_TOUCH.
				 */
				event->touch_state = (ie.value ? STATE_TOUCH_DOWN : STATE_TOUCH_UP);
			}
		}
		else if (ie.type == EV_ABS) {
			switch (ie.code) {
				case ABS_X:
					x = (TS_SIZE - ie.value);
					break;

				case ABS_Y:
					y = ie.value;
					break;

				case ABS_PRESSURE:
					event->pressure = ie.value;
					break;
			
			} /* end of switch */
	
		} /* end of if */
	
	} /* end of while */


	/**
	 * Nothing to do with coordinates.
	 */
	if (event->touch_state == STATE_TOUCH_UP) {
		return 0;
	}

	/**
	 * Restore lost pieces.
	 */
	if (x == 0) {
		x = event->x;
	}
	if (y == 0) {
		y = event->y;
	}

	/* No correction. */
	if (correction == NULL) {
		event->x = x;
		event->y = y;
		
		return 0;
	}

	/* Do touch correction. */ 
	event->x = (correction->xd_coef_x * x) + (correction->xd_coef_y * y) + correction->xd_coef_1;
	event->y = (correction->yd_coef_x * x) + (correction->yd_coef_y * y) + correction->yd_coef_1;

	return 0;
}

