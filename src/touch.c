#include "touch.h"
#include "machine_specific.h"
#include "debug.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


#ifdef __APPLE__
///////////////////// TEST ENVIRONMENT //////////////////////
struct input_event {
    int type, code, value;
};
#define EV_SYN          1
#define EV_KEY          2
#define EV_ABS          3
#define ABS_X           4
#define ABS_Y           5
#define ABS_PRESSURE    6
#define BTN_TOUCH       7
///////////////////// TEST ENVIRONMENT //////////////////////
#else
#include <linux/input.h>
#include <linux/input-event-codes.h>
#endif

int touch_read(int fd, struct touch_event *event) {
#ifndef UNSAFE
    ASSERTDO(event != NULL, print_error("touch_read(): event cannot be null.\n"); return -1);
#endif
    
    struct input_event ie;

    /**
     * Clean touch_event struct.
     * Leave other values but touch_state.
     */
    event->touch_state = STATE_NONE;

    while (1) {
        
        /**
         * Single read
         */
        if (read(fd, &ie, sizeof(struct input_event) == -1)) {
#ifdef NONBLOCK_READ
            /**
             * With non-block read, EAGAIN is not an error.
             */
            if (errno == EAGAIN)
                return 1;
            else {
                print_error("touch_read(): error other than EAGAIN occured while read().\n");
                return -1;
            }
#else
            /**
             * With blocking read, every error is a problem.
             */
            print_error("touch_read(): error while read().\n");
            return -1;
#endif
        }
        
        /**
         * Process read values
         */
        switch (ie.type) {
            case EV_SYN:
                return 0;
                
            case EV_KEY:
                if (ie.code == BTN_TOUCH)
                    event->touch_state = (ie.value ? STATE_TOUCH_DOWN : STATE_TOUCH_UP);
                break;
                
            case EV_ABS:
                switch (ie.code) {
                    case ABS_X:
                        event->x = ((TS_WDITH - ie.value) - TS_X_MIN) * DP_WIDTH / (TS_X_MAX - TS_X_MIN);
                        break;
                        
                    case ABS_Y:
                        event->y = (ie.value - TS_Y_MIN) * DP_HEIGHT / (TS_Y_MAX - TS_Y_MIN);
                        break;
                        
                    case ABS_PRESSURE:
                        event->pressure = ie.value;
                        break;
                }
                break;
                
        } /* end of switch */
    } /* end of while */
    
    return 0;
}


