#include "touch.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef __APPLE__

///////////////////// DELETE HERE ON RELEASE ENV //////////////////////
struct input_event {
    int type, code, value;
};
#define EV_SYN          1
#define EV_KEY          2
#define EV_ABS          3
#define ABS_X           4
#define ABS_Y           5
#define ABS_PRESSURE    6
#define BTN_TOUCH       7
///////////////////// DELETE HERE ON RELEASE ENV //////////////////////

#else
#include <linux/input.h>
#include <linux/input-event-codes.h>
#endif


#include <errno.h>
#include <time.h>
#include "machine_specific.h"

/**
  * Constructor
  */
touch::touch(const char *event_fd_path):
correction_(touch_correction(TS_X_MIN, TS_X_MAX, TS_Y_MIN, TS_Y_MAX)) {
	fd_ = open(event_fd_path, O_RDONLY | O_NONBLOCK);
	if (fd_ == -1) {
		perror("open failed");
		exit(1); // TODO: change it to throw
	}
}

/**
  * Private
  */
int touch::_touch_read(int fd, touch_event *event, touch_correction *correction) {
	if (event == NULL) return -1;

	struct input_event 	ie;
	ssize_t	    		nread;
	int 		    	x;
	int 			    y;

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
		nread = read(fd, &ie, (size_t)sizeof(struct input_event));
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
        event->x = (x - correction->x_min()) * 320 / (correction->x_max() - correction->x_min());
    }
    if (y != -1) {
        event->y = (y - correction->y_min()) * 240 / (correction->y_max() - correction->y_min());
    }
    
	return 0;
}

/**
  * Public
  */
int touch::touch_read(touch_event *event) {
    return _touch_read(fd_, event, &correction_);
}


