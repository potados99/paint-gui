#include "input.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <errno.h>
#include <time.h>


/**
  * Constructor
  */
input::input(const char *event_fd_path) {
	fd_ = open(event_fd_path, O_RDONLY | O_NONBLOCK);
	if (fd_ == -1) {
		perror("open failed");
		exit(1); // TODO: change it to throw
	}

	correction_ = touch_correction();
}

/**
  * Private
  */
int input::touch_read(int fd, touch_event *event, touch_correction *correction) {
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
		event->x = (correction->xd_coef_x * x) + (correction->xd_coef_y * y) + correction->xd_coef_1;
	}
	if (y != -1) {
		event->y = (correction->yd_coef_x * x) + (correction->yd_coef_y * y) + correction->yd_coef_1;
	}

	return 0;
}

/**
  * Public
  */
void input::start_loop() {
	// TODO: replace this test code.

	int 		read = 0;
	int 		line_num = 0;
	touch_event 	te;
	
	bool 		touched = 0;
	time_t		last_time = 0;

	while (1) {
		
		/**
		  * Poll successful + fd ready + has event
		  */
		
		read = touch_read(fd, &te, NULL); /* read with fd, to te, no correction. */
	
		if (read == 0) {
			/**
			  * Successfull read.
			  */

			if (te.touch_state == STATE_TOUCH_DOWN) {
				touched = 1;
			}
			else if (te.touch_state == STATE_TOUCH_UP) {
				touched = 0;
			}
			
			if (te.touch_state == STATE_TOUCH_DOWN) printf("\n============================= TOUCH START =============================\n");
			printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
			if (te.touch_state == STATE_TOUCH_UP) printf("============================= TOUCH FINISH =============================\n\n");
		}
		else {
			if (read == 1) {
				if (touched == 0) {
					/**
					 * Only when touch is finished.
					 */
					time_t cur_time = time(NULL);
					if (last_time < cur_time) {
						printf("waiting...\n");
						last_time = cur_time;
					}
				}
			}
			else {
				fprintf(stdout, "touch_read failed.\n");
				exit(1);
			}
		}		
	}

}

