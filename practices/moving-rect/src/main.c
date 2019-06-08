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

void rough_redraw_move(unsigned short *mem);
void partial_redraw_move(unsigned short *mem);


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

	disp_draw_rect(mem, 0, 0, 80, 80, PIXEL(255, 128, 0));

	rough_redraw_move(mem);
	usleep(1000000); /* 1 sec */
	partial_redraw_move(mem);

	disp_unmap(mem);
	close(dp_fd);

	return 0;
}


void rough_redraw_move(unsigned short *mem) {
}

void partial_redraw_move(unsigned short *mem) {
}


