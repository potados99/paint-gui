/**
  * Practice: moving-rect
  *
  * To test display performance and duplicated draw behavior.
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "display.h"

void rough_redraw_move(unsigned short *mem);
void partial_redraw_move(unsigned short *mem);
void smooth_redraw_move(unsigned short *mem);

int main(int argc, const char * argv[]) {
	puts("Practice: moving rect\n");
	
	int		dp_fd;
	unsigned short 	*mem;


	dp_fd = open(FDPATH, O_RDWR);
	if (dp_fd == -1) {
		perror("open error");
		return -1;
	}

	mem = disp_map(dp_fd);

	disp_draw_rect(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT, PIXEL(0, 0, 0)); /* clear background */

	disp_draw_rect(mem, 0, 0, 80, 80, PIXEL(0, 128, 255));

	rough_redraw_move(mem);
	
	usleep(1000000); /* 1 sec */
	disp_draw_rect(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT, PIXEL(0, 0, 0)); /* clear background */
	
	partial_redraw_move(mem);
	
	usleep(1000000); /* 1 sec */
	disp_draw_rect(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT, PIXEL(0, 0, 0)); /* clear background */
	
	smooth_redraw_move(mem);


	disp_unmap(mem);
	close(dp_fd);

	return 0;
}


void rough_redraw_move(unsigned short *mem) {
	/**
	 * Conclusion: THIS IS A TOTAL SHIT.
	 */
	for (int i = 0; i < 160; ++i) {
		usleep(5000);
		disp_draw_rect(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT, PIXEL(0, 0, 0)); /* clear background */
		disp_draw_rect(mem, i, i, 80, 80, PIXEL(0, 128, 255));
	}
}

void partial_redraw_move(unsigned short *mem) {
	for (int i = 0; i < 159; ++i) {
		usleep(5000);
		disp_draw_rect(mem, i, i, 80, 80, PIXEL(0, 0, 0));
		disp_draw_rect(mem, i + 1, i + 1, 80, 80, PIXEL(0, 128, 255));
	}
}


void smooth_redraw_move(unsigned short *mem) {
	for (int i = 0; i < 160; ++i) {
		for (int j = i; j < 80; ++j) {
			disp_draw_point(mem, j, i, PIXEL(0, 0, 0));
			disp_draw_point(mem, i, j, PIXEL(0, 0, 0));
		}

		for (int j = i + 80; j > 0; --j) {
			disp_draw_point(mem, j, i + 80, PIXEL(0, 128, 255));
			disp_draw_point(mem, i + 80, j, PIXEL(0, 128, 255));
		}
	}
}

