#include "touch.h"
#include "machine_specific.h"
#include "debug.h"
#include "macros.h"
#include "metric.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


#ifdef __APPLE__
///////////////////// TEST ENVIRONMENT //////////////////////
//
// 이 부분은 맥북에서 빌드하기 위해 만들어놓은 것입니다.
// 지워버려도 돼요!

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

#define TRANSFORM_X(VAL) (((TS_WDITH - VAL) - TS_X_MIN) * DP_WIDTH / (TS_X_MAX - TS_X_MIN))
#define TRANSFORM_Y(VAL) ((VAL - TS_Y_MIN) * DP_HEIGHT / (TS_Y_MAX - TS_Y_MIN))

int touch_read(int fd, struct touch_event *event) {
#ifndef UNSAFE
    ASSERTDO(event != NULL, print_error("touch_read(): event cannot be null.\n"); return -1);
#endif
   
    /**
     * 리눅스니까 리눅스가 제공해주시는 input_event를 씁니다.
     */
    struct input_event 	ie;

    /**
     * 별다른 일이 있기 전까지는 터치 상태는 기본 STATE_NONE입니다.
     */
    event->touch_state = STATE_NONE;

    /**
     * 리눅스의 터치 이벤트는 여러 정보를 전달합니다.
     * 한 번의 터치도 여러 이벤트로 나누어 전달되는데,
     * EV_SYN를 통해 하나의 이벤트가 종료되었음을 알립니다.
     */
    while (1) {

        /**
         * 읽기
         */
		if (read(fd, &ie, sizeof(struct input_event)) == -1)  {
#ifdef NONBLOCK_READ
            /**
             * non-block 읽기에서, EAGAIN은 에러가 아닙니다.
             * 1을 리턴하여 읽을 것이 없음을 알립니다.
             */
            if (errno == EAGAIN)
                return 1;
            else {
                print_error("touch_read(): error other than EAGAIN occured while read().\n");
                return -1;
            }
#else
            /**
             * blocking 읽기에서, 에러는 그냥 에러입니다...ㅎㅎ
             */
            print_error("touch_read(): error while read().\n");
            return -1;
#endif
        }
        
        /**
         * 처리하기
         */
        switch (ie.type) {
            case EV_SYN:
                /**
                 * 끝입니다. 이제 처리합시다.
                 * 브레이크를 두번 할수도 없고 해서 고투를 썼어요..
                 */
               	return 0;
               
            case EV_KEY:
                /**
                 * 이 디스플레이가 지원하는 EV_KEY는 사실 BTN_TOUCH밖에 없습니다.
                 * if 비교 안해도 돼요..
                 */
                // if (ie.code == BTN_TOUCH)
                event->touch_state = (ie.value ? STATE_TOUCH_DOWN : STATE_TOUCH_UP);
                break;
                
            case EV_ABS:
                /**
                 * 절대좌표가(Absolute) 들어왔군요...
                 * ABS_X인지 ABS_Y인지 ABS_PRESSURE인지 확인해줍니다.
                 */
                switch (ie.code) {
                    case ABS_X:
                        event->x = TRANSFORM_X(ie.value);
                        break;
                        
                    case ABS_Y:
                        event->y = TRANSFORM_Y(ie.value);
                        break;
                        
                    case ABS_PRESSURE:
                        event->pressure = ie.value;
                        break;
                }
                break;
                
        } /* switch 끝 */
        
    } /* while 끝 */
    
    return 0;
}


