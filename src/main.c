#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "touch.h"
#include "display.h"
#include "machine_specific.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
	printf("paint-c\n");
    
    int                 ts_fd;
    int                 dp_fd;
    int                 ts_read;
    int                 touched;
    int                 last_x = 0;
    int                 last_y = 0;
    struct touch_event  te;
 	te.x = 0;
	te.y = 0;
   
    unsigned long       line_num = 0;

    
    ts_fd = open(TS_FD_PATH, TS_OPEN_OPTION);
    ASSERTDO(ts_fd != -1, print_error("open() error with %s.\n", TS_FD_PATH); return -1);
    
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    disp_map(dp_fd);
    
	disp_draw_whole(COLOR(255, 255, 255));
	disp_commit();
	
    
    while (1) {
        
        ts_read = touch_read(ts_fd, &te); 
 
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
        
        disp_draw_line(last_x, last_y, te.x, te.y, COLOR(0, 0, 0));
        disp_commit();
        
        last_x = te.x;
        last_y = te.y;
    }
    
    
    
    
    

	disp_unmap();
	close(dp_fd);
    close(ts_fd);
    
	return 0;
}

