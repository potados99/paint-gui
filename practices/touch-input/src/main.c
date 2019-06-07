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
	puts("Practice: touch-input");
	
	int 			fd = 0;
	int 			read = 0;
	unsigned long 		line_num = 0;
	struct touch_event 	te;

	fd = open(EVENTPATH, O_RDONLY); /* no 'O_NONBLOCK' for 'blocking read */
	if (fd == -1) {	
		perror("open failed.\n");
		exit(1);
	}

	while (1) {
		/**
		  * Blocking read in loop.
		  */
		
		read = touch_read(fd, &te, NULL); /* read with fd, to te, no correction. */
		if (read == 0) {
			/**
			  * Successfull read.
			  */
			if (te.touch_state == STATE_TOUCH_DOWN) printf("============================= TOUCH START =============================\n");
			printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
			if (te.touch_state == STATE_TOUCH_UP) printf("============================= TOUCH FINISH =============================\n\n");
		}
		else {
			fprintf(stdout, "touch_read failed.\n");
			exit(1);
		}
	}


	/* NOREACH */

	return 0;
}
