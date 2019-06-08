#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "touch.h"
#include "display.h"
#include "metric.h"


void calibrate(unsigned short *dp_mem, int ts_fd, struct touch_correction *correction);

int main(int argc, const char * argv[]) {
	
	int ts_fd;
	int dp_fd;
	int ts_read;
	int touched;
	unsigned long line_num;
	unsigned short *mem;
	struct touch_event te;
	struct touch_correction correction;

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

	disp_draw_rect(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT, PIXEL(0, 0, 0));
	disp_draw_done(mem);

	calibrate(mem, ts_fd, &correction);

	disp_draw_rect(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT, PIXEL(0, 0, 0));
	disp_draw_done(mem);

	printf("\ncalibration done.\n");
	printf("XD = (%dX + %dY + %d) / %d\n", correction.xd_coef_x, correction.xd_coef_y, correction.xd_coef_1, correction.k);
	printf("YD = (%dX + %dY + %d) / %d\n\n", correction.yd_coef_x, correction.yd_coef_y, correction.yd_coef_1, correction.k);

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
			touched = 1;
		}
		else if (te.touch_state == STATE_TOUCH_UP) {
			touched = 0;
		}
	
		if (te.touch_state == STATE_TOUCH_DOWN) printf("\n============================= TOUCH START =============================\n");
		printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
		if (te.touch_state == STATE_TOUCH_UP) printf("============================= TOUCH FINISH =============================\n\n");
		
		disp_draw_point(mem, te.x, te.y, PIXEL(255, 255, 255));
		disp_partial_done(mem, te.x, te.y, 1, 1);
	
	}


	/* NOREACH */

	return 0;
}





void calibrate(unsigned short *dp_mem, int ts_fd, struct touch_correction *correction) {
	
	struct point lcd_points[3] = {
		{160, 80}, {80, 160}, {240, 160}
	};

	struct point ts_points[3];
	struct touch_event te;
	int read_result = 0;

	int pressed = 0;

	int index = 0;

	while (index < 3) {
		disp_draw_rect(dp_mem, lcd_points[index].x - 1, lcd_points[index].y - 1, 3, 3, PIXEL(255, 255, 255));
		disp_draw_done(dp_mem);

		read_result = touch_read(ts_fd, &te, NULL);

		if (read_result == 0) {
			if (te.touch_state == STATE_TOUCH_DOWN) {
				ts_points[index].x = te.x;
				ts_points[index].y = te.y;

				printf("got coordinate %d!\n", index);		
				
				index++;	

				if (index < 3) {
					disp_draw_rect(dp_mem, lcd_points[index].x - 1, lcd_points[index].y - 1, 3, 3, PIXEL(255, 255, 255));
					disp_draw_done(dp_mem);
				}
			}
		}
	}
	
	int k = ((ts_points[0].x - ts_points[2].x) * (ts_points[1].y - ts_points[2].y)) - 
		((ts_points[1].x - ts_points[2].x) * (ts_points[0].y - ts_points[2].y));

	correction->k = k;
		
	correction->xd_coef_x = (((lcd_points[0].x - lcd_points[2].x) * (ts_points[1].y - ts_points[2].y)) - 
			((lcd_points[1].x - lcd_points[2].x) * (ts_points[0].y - ts_points[2].y)));
	
	correction->xd_coef_y = (((ts_points[0].x - ts_points[2].x) * (lcd_points[1].x - lcd_points[2].x)) - 
			((lcd_points[0].x - lcd_points[2].x) * (ts_points[1].x - ts_points[2].x)));
 
	correction->xd_coef_1 = ((ts_points[0].y * ((ts_points[2].x * lcd_points[1].x) - (ts_points[1].x * lcd_points[2].x))) + 
			(ts_points[1].y * ((ts_points[0].x * lcd_points[2].x) - (ts_points[2].x * lcd_points[0].x))) + 
			(ts_points[2].y * ((ts_points[1].x * lcd_points[0].x) - (ts_points[0].x * lcd_points[1].x))));
	

	correction->yd_coef_x = (((lcd_points[0].y - lcd_points[2].y) * (ts_points[1].y - ts_points[2].y)) - 
			((lcd_points[1].y - lcd_points[2].y) * (ts_points[0].y - ts_points[2].y)));
	
	correction->yd_coef_y = (((ts_points[0].x - ts_points[2].x) * (lcd_points[1].y - lcd_points[2].y)) - 
			((lcd_points[0].y - lcd_points[2].y) * (ts_points[1].x - ts_points[2].x)));
	
	correction->yd_coef_1 = ((ts_points[0].y * ((ts_points[2].x * lcd_points[1].y) - (ts_points[1].x * lcd_points[2].y))) + 
			(ts_points[1].y * ((ts_points[0].x * lcd_points[2].y) - (ts_points[2].x * lcd_points[0].y))) + 
			(ts_points[2].y * ((ts_points[1].x * lcd_points[0].y) - (ts_points[0].x * lcd_points[1].y))));
}
