#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "touch.h"
#include "display.h"
#include "machine_specific.h"
#include "debug.h"
#include "shape.h"

int main(int argc, const char *argv[]) {
	printf("paint-c\n");
    
    int                 ts_fd;
    int                 dp_fd;
    int                 ts_read;
    int                 touched;
    int                 last_x = 0;
    int                 last_y = 0;
    unsigned long       line_num = 0;
   
    TOUCH_EVENT(te); /* decl & init */

    ts_fd = open(TS_FD_PATH, TS_OPEN_OPTION);
    ASSERTDO(ts_fd != -1, print_error("open() error with %s.\n", TS_FD_PATH); return -1);
    
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    disp_map(dp_fd);
    
	disp_draw_whole(COLOR(255, 255, 255));
	disp_commit();
    

	struct shape sq0;
	sq0.type = ST_RECTP;
	sq0.value[0] = 50;
	sq0.value[1] = 50;
	sq0.value[2] = 150;
	sq0.value[3] = 150;
	sq0.color = 0;
	disp_draw_2d_shape(&sq0);
	disp_commit();
	usleep(400000);

	struct shape sq1;
	sq1.type = ST_RECTP;
	sq1.value[0] = 100;
	sq1.value[1] = 70;
	sq1.value[2] = 200;
	sq1.value[3] = 170;
	sq1.color = COLOR(255, 0, 0);
	disp_draw_2d_shape(&sq1);
	disp_commit();
	usleep(400000);

	struct shape ln0;
	ln0.type = ST_LINE;
	ln0.value[0] = 80;
	ln0.value[1] = 25;
	ln0.value[2] = 240;
	ln0.value[3] = 210;
	ln0.color = COLOR(0, 128, 0);
	disp_draw_2d_shape(&ln0);
	disp_commit();
	usleep(400000);

	struct shape sq2;
	sq2.type = ST_RECTP_FILL;
	sq2.value[0] = 20;
	sq2.value[1] = 120;
	sq2.value[2] = 250;
	sq2.value[3] = 130;
	sq2.color = COLOR(0, 128, 255);
	disp_draw_2d_shape(&sq2);
	disp_commit();
	usleep(400000);
 
    /**
     * Move + partial redraw.
     */
	for (int i = 0; i < 70; ++i) {
		usleep(100000);
		disp_draw_whole(COLOR(255, 255, 255));
        shape_move(&sq1, +1, +1);

		disp_draw_2d_shape(&sq0);
		disp_draw_2d_shape(&sq1);
		disp_draw_2d_shape(&ln0);
		disp_draw_2d_shape(&sq2);
		
		disp_commit_partialp(sq1.value[0], sq1.value[1] - 1, sq1.value[2], sq1.value[3]);
	}

    
    /**
     * Read + free draw
     */
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
        disp_commit_partialp(MIN(last_x, te.x) - 1, MIN(last_y, te.y) - 1, MAX(last_x, te.x) + 1, MAX(last_y, te.y) + 1);
        
        last_x = te.x;
        last_y = te.y;
    }
    
    
    
    
    

	disp_unmap();
	close(dp_fd);
    close(ts_fd);
    
	return 0;
}

