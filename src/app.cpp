//
//  app.cpp
//  paint-gui
//
//  Created by POTADOS on 09/06/2019.
//  Copyright © 2019 potados. All rights reserved.
//

#include <stdio.h>

#include "app.h"


void app::start_input_loop() {
    // TODO: replace this test code.
    
    int         read = 0;
    unsigned long     line_num = 0;
    touch_event     te;
    
    bool         touched = 0;
    time_t        last_time = 0;
    
    while (1) {
        
        /**
         * Poll successful + fd ready + has event
         */
        
        read = touch_.touch_read(&te); /* read with fd, to te, no correction. */
        
        if (read == 0) {
            /**
             * Successfull read.
             */
            
            if (te.touch_state == STATE_TOUCH_DOWN) {
                touched = 1;
            }
            else if (te.touch_state == STATE_TOUCH_UP) {
                touched = 0;
            }
            
            if (te.touch_state == STATE_TOUCH_DOWN) printf("\n============================= TOUCH START =============================\n");
            printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
            if (te.touch_state == STATE_TOUCH_UP) printf("============================= TOUCH FINISH =============================\n\n");
        }
        else {
            if (read == 1) {
                if (touched == 0) {
                    /**
                     * Only when touch is finished.
                     */
                    time_t cur_time = time(NULL);
                    if (last_time < cur_time) {
                        printf("waiting...\n");
                        last_time = cur_time;
                    }
                }
            }
            else {
                fprintf(stdout, "touch_read failed.\n");
                exit(1);
            }
        }
    }

}
