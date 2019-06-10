#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "touch.h"
#include "display.h"
#include "metric.h"

#define ABS(X) ((X) < 0 ? -(X) : (X))

int main(int argc, const char * argv[]) {
	puts("Practice: free-draw");

	int ts_fd;
	int dp_fd;
	int ts_read;
	int touched;
	unsigned long line_num;
	unsigned short *mem;
	struct touch_event te;
	struct touch_correction correction;

	correction.x_min = 310;
	correction.x_max = 3900;

	correction.y_min = 150;
	correction.y_max = 3900;
	
	touched = 0;
	line_num = 0;

	ts_fd = open(EVENTPATH, O_RDONLY | O_NONBLOCK);
	if (ts_fd == -1) {
		perror("ts_fd open failed");
		return 1;
	}

	dp_fd = open(FDPATH, O_RDWR);
	if (dp_fd == -1) {
		perror("dp_fd open failed");
		return 1;
	}


	mem = disp_map(dp_fd);

	disp_draw_rect(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT, 0);
	disp_draw_done(mem);

	int last_x;
	int last_y;

	while (1) {
		
		ts_read = touch_read(ts_fd, &te, &correction);

		if (ts_read != 0) {
			if (ts_read == -1) {
				printf("error while reading!\n");
				return 1;
			}
			usleep(10000);	
			continue;
		}
		
		if (te.touch_state == STATE_TOUCH_DOWN) {
			last_x = te.x;
			last_y = te.y;

			touched = 1;
		}
		else if (te.touch_state == STATE_TOUCH_UP) {
			touched = 0;
		}
	
		if (te.touch_state == STATE_TOUCH_DOWN) printf("\n============================= TOUCH START =============================\n");
		printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
		if (te.touch_state == STATE_TOUCH_UP) printf("============================= TOUCH FINISH =============================\n\n");
		
		/*
		disp_draw_point(mem, te.x, te.y, PIXEL(255, 255, 255));
		disp_partial_done(mem, te.x, te.y, 1, 1);
		*/
	
		if (ABS(te.x - last_x) > 15 || ABS(te.y - last_y) > 15) {
			printf("BOOM!\n");
			continue;
		}

		disp_draw_line(mem, last_x, last_y, te.x, te.y, PIXEL(255, 255, 255));
		disp_draw_done(mem);

		last_x = te.x;
		last_y = te.y;
	}


	/* NOREACH */

	return 0;
}

