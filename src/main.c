#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "display.h"
#include "machine_specific.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
	printf("back to c\n");
    
    int             dp_fd;
    unsigned short  *dp_mem;
    
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    dp_mem = disp_map(dp_fd);
    
	disp_draw_whole(dp_mem, PIXEL(0, 128, 255));
	disp_commit(dp_mem);
	
	int start = POINT(0, DP_HEIGHT - 1);
	int end;

	usleep(1000000);

	disp_draw_line(dp_mem, start, POINT(320, 0), PIXEL(255, 255, 255));
	disp_commit(dp_mem);

	usleep(1000000);

	for (int i = 0; i < 100; ++i) {
		disp_draw_line(dp_mem, start, POINT(i, 0), PIXEL(0, 128, 255));		
		disp_draw_line(dp_mem, start, POINT(i + 1, 0), PIXEL(255, 255, 255));
		disp_commit(dp_mem);		
	} 




  
	return 0;
}

