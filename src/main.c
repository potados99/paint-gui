#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "display.h"
#include "machine_specific.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
	printf("back to c\n");
    
    int             dp_fd;
    
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    disp_map(dp_fd);
    
	disp_draw_whole(PIXEL(0, 128, 255));
	disp_commit();
	
	int start = POINT(0, DP_HEIGHT - 1);

	//usleep(1000000);
/*
    disp_set_direct(true);
	for (int i = 0; i < 319; ++i) {
		usleep(1000);
		disp_draw_line(start, POINT(i, 0), PIXEL(0, 128, 255));
		disp_draw_line(start, POINT(i + 1, 0), PIXEL(255, 255, 255));
		//disp_commit();
	} 
*/
	
	disp_set_direct(false);

	disp_draw_line(POINT(0, 239), POINT(319, 0), PIXEL(255, 255, 255));
	disp_commit();

	for (int i = 0; i < 160; ++i) {
		usleep(20000);
		disp_draw_rect(POINT(i, i), SIZE(80, 80), PIXEL(0, 128, 255));
		disp_draw_line(start, POINT(319, 0), PIXEL(255, 255, 255));		
		disp_draw_rect(POINT(i+1, i+1), SIZE(80, 80), PIXEL(0, 255, 0));
		disp_commit_partial(POINT(i, i), SIZE(81, 81));
		disp_cancel();
	}

	disp_unmap();
	close(dp_fd);
  
	return 0;
}

