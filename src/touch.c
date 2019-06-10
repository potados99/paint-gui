#include "touch.h"
#include "machine_specific.h"
#include "debug.h"
#include "macros.h"

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

int touch_read(int fd, struct touch_event *event) {
#ifndef UNSAFE
    ASSERTDO(event != NULL, print_error("touch_read(): event cannot be null.\n"); return -1);
#endif
    
    /**
     * 리눅스니까 리눅스가 제공해주시는 input_event를 씁니다.
     */
    struct input_event ie;
    
    /**
     * 입력값을 그대로 쓰지는 않을겁니다. 왜냐면 입력이 튀는 경우가 있거든요.
     * 그래서 입력 후에 처리(필터링)하기 위해 잠시 담아둘 변수입니다.
     * 간혹 입력이 없는 경우가 있습니다. 그런 경우를 위해 이전 값을 담아둡니다.
     */
    short               raw_x = event->x;
    short               raw_y = event->y;
    
    /**
     * 현재 들어온 좌표가 예측 범위에서 벗어나는 이상한 값인지 판단할 때에 쓰입니다.
     */
    unsigned short      distance = 0;
    
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
        if (read(fd, &ie, sizeof(struct input_event) == -1)) {
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
                goto process_input;
                
            case EV_KEY:
                /**
                 * 이 디스플레이가 지원하는 EV_KEY는 사실 BTN_TOUCH밖에 없습니다.
                 * if 비교 안해도 돼요..
                 */
                // if (ie.code == BTN_TOUCH)
                if (ie.value == 1) {
                    event->touch_state = STATE_TOUCH_DOWN;
                }
                else {
                    event->touch_state = STATE_TOUCH_UP;
                    /**
                     * 이전 터치 정보를 싹 지워줍니다.
                     * unsigned라서 제일 큰 값이 들어갑니다.
                     */
                    event->last_distance = -1;
                }
                break;
                
            case EV_ABS:
                /**
                 * 절대좌표가(Absolute) 들어왔군요...
                 * ABS_X인지 ABS_Y인지 ABS_PRESSURE인지 확인해줍니다.
                 */
                switch (ie.code) {
                    case ABS_X:
                        raw_x = ((TS_WDITH - ie.value) - TS_X_MIN) * DP_WIDTH / (TS_X_MAX - TS_X_MIN);
                        break;
                        
                    case ABS_Y:
                        raw_y = (ie.value - TS_Y_MIN) * DP_HEIGHT / (TS_Y_MAX - TS_Y_MIN);
                        break;
                        
                    case ABS_PRESSURE:
                        event->pressure = ie.value;
                        break;
                }
                break;
                
        } /* switch 끝 */
        
    } /* while 끝 */
    
process_input:
    
    /**
     * 터치가 지속중인 상태에만 검사를 해야 합니다!
     *
     * 만약 터치중이라면, 이 입력값이 돌연변이 돌아이인지 판단을 해야 합니다.
     * 먼저 지난 입력값을 알아야 하는데, 그건 event에 고대로 있습니다!
     *
     * 이전 두 점간의 거리와, 현재와 마지막 점간의 거리를 비교합니다.
     * 현재와 마지막 점간의 거리가 갑자기 늘어났다면? 현재 점은 무효!!
     *
     * 거리를 구할 때에는 피타고라스 정리같이 비싼 것은 쓰지 않습니다.
     * 그저 x변화량과 y변화량을 더할 뿐입니다... 이것도 충분해요..ㅎㅎ
     */
    if (event->touch_state == STATE_NONE) {
        
        distance = ABS(raw_x - event->x) + ABS(raw_y - event->y);
        
        if (distance > event->last_distance + TS_JUMP_TOLERANCE) {
            /**
             * 확실합니다. 이넘은 돌연변이입니다.
             * 한번 터치중에 점푸하는것도 한계가 있지 얘는 얼마나 가는거야...
             */
            
            print_info("Long jump during touch detected!\n");
            
        }
        else {
            /**
             * 튀지 않는 온화한 자만이 event의 x와 y에 대입될 자격이 있습니다.
             */
            event->x = raw_x;
            event->y = raw_y;
            
            event->last_distance = distance;
        }
    }
    
    return 0;
}


