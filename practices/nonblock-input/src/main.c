/**
  * Practice: touch-input
  *
  * Receive touch event forever.
  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

#include "touch.h"

int main(int argc, const char * argv[]) {
	puts("Practice: nonblock-input");
	
	int 			fd = 0;
	int 			read = 0;
	unsigned long 		line_num = 0;
	struct touch_event 	te;
	struct pollfd 		p;
	int			pread;

	fd = open(EVENTPATH, O_RDONLY);
	if (fd == -1) {	
		perror("open failed.\n");
		exit(1);
	}

	p.fd = fd;
	p.events = POLLIN;

	while (1) {
		
		pread = poll(&p, 1, 0);

		if (pread == -1) {
			perror("error while poll()");
			exit(1);
		}
		else if (pread == 0) {
			printf("pass\n");
			continue;
		}

		if (p.revents & POLLIN) {
				/**
				  * Poll successful + fd ready + has event
				  */
		
			read = touch_read(fd, &te, NULL); /* read with fd, to te, no correction. */
			if (read == 0) {
				/**
				  * Successfull read.
				  */
				printf("[%ld] X: %5d,\tY: %5d,\tPressure: %d\n", line_num++, te.x, te.y, te.pressure);
			}
			else {
				if (read == 1) {
					printf("nothing to read.\n");
				}
				else {
					fprintf(stdout, "touch_read failed.\n");
					exit(1);
				}
			}		
		}
		else {
			printf("whats this case?\n");	
		}
}


	/* NOREACH */

	return 0;
}
