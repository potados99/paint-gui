#include "touch.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef __APPLE__
///////////////////// TEST ENVIRONMENT //////////////////////
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
///////////////////// TEST ENVIRONMENT //////////////////////
#else
#include <linux/input.h>
#include <linux/input-event-codes.h>
#endif

#include <errno.h>
#include <time.h>
#include "machine_specific.h"

#include "debug.h"


/******************************
 * Public
 ******************************/

touch::touch(const char *event_fd_path):
correction_(touch_correction(TS_X_MIN, TS_X_MAX, TS_Y_MIN, TS_Y_MAX)) {
    /**
     * Open fd. nonblock.
     */
    ASSERTDO((fd_ = open(event_fd_path, O_RDONLY | O_NONBLOCK)) != -1,
             print_error("touch::touch(): open() failed with %s.\n", event_fd_path);
             exit(1));
}

touch::~touch() {
    close(fd_);
}

int touch::touch_read(touch_event *event) {
    ASSERTDO(event != NULL, print_error("touch::touch_read(touch_event): event cannot be null.\n"); return -1);
    
    return _touch_read(fd_, event, &correction_);
}


/******************************
 * Private
 ******************************/

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
    event->state = touch_state::STATE_NONE;

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
                print_error("touch::_touch_read(): read() failed due to unknown error.\n");
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
                if (ie.value == 1) {
                    print_error("touch::_touch_read(): touch down.\n");
                    event->state = touch_state::STATE_TOUCH_DOWN;
                }
                else if (ie.value == 0) {
                    print_error("touch::_touch_read(): touch up.\n");
                    event->state = touch_state::STATE_TOUCH_UP;
                }
                else {
                    /* NOT POSSIBLE */
                    print_error("touch::_touch_read(): IMPOSSIBLE AREA.\n");
                }
			}
		}
		else if (ie.type == EV_ABS) {
			switch (ie.code) {
				case ABS_X:
					x = (TS_WDITH - ie.value);
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
    if (event->state == touch_state::STATE_TOUCH_UP) {
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
        event->x = (x - correction->x_min()) * DP_WIDTH / (correction->x_max() - correction->x_min());
    }
    if (y != -1) {
        event->y = (y - correction->y_min()) * DP_HEIGHT / (correction->y_max() - correction->y_min());
    }
    
	return 0;
}


