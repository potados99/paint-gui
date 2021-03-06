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
	int			nread;
	int 			x;
	int 			y;

	/**
	 * Clean touch_event struct.
	 * Leave other values but touch_state.
	 */	
	event->touch_state = STATE_NONE;

	/**
	 * Set 'Not set' flags.
	 */
	x = -1;
	y = -1;

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

	/* No correction. */
	if (correction == NULL) {
		if (x != -1) event->x = x;
		if (y != -1) event->y = y;
		
		return 0;
	}

	/* Do touch correction. */ 
	if (x != -1) {
		event->x = (x - correction->x_min) * 320 / (correction->x_max - correction->x_min);
	}
	if (y != -1) {
		event->y = (y - correction->y_min) * 240 / (correction->y_max - correction->y_min);
	}

	return 0;
}

