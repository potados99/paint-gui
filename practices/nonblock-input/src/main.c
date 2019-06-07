/**
  * Practice: touch-input
  *
  * Receive touch event forever.
  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "touch.h"

int main(int argc, const char * argv[]) {
	puts("Practice: nonblock-input");
	
	int 			fd = 0;
	int 			read = 0;
	unsigned long 		line_num = 0;
	struct touch_event 	te;
	int 			touched = 0;


	fd = open(EVENTPATH, O_RDONLY | O_NONBLOCK);
	if (fd == -1) {	
		perror("open failed.\n");
		exit(1);
	}


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

			printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %2d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
		}
		else {
			if (read == 1) {
				if (touched == 0) {
					/**
					 * Only when touch is finished.
					 */
					printf("nothing to read.\n");
				}
			}
			else {
				fprintf(stdout, "touch_read failed.\n");
				exit(1);
			}
		}		
	}

	/* NOREACH */

	return 0;
}
