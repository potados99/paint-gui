#include "routine.h"

#include "touch.h"
#include "display.h"
#include "paint.h"
#include "machine_specific.h"

#include <fcntl.h>

void do_it(void) {
    int                 ts_fd;      /* 터치스크린 파일 기술자 */
    int                 dp_fd;      /* 디스플레이 파일 기술자 */
    int                 ts_read;    /* 터치 읽은 결과 저장 */
    
    /**
     * touch_event 구조체 선언 & 초기화
     */
    TOUCH_EVENT(te);
    
    /**
     * 터치스크린 열기!
     */
    ts_fd = open(TS_FD_PATH, TS_OPEN_OPTION);
    ASSERTDO(ts_fd != -1, print_error("open() error with %s.\n", TS_FD_PATH); return);
    
    /**
     * 디스플레이 열기!
     */
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return);
    
    /**
     * 디스플레이 사용 준비!
     */
    disp_map(dp_fd);
    
    /**
     * 그림판 실체화!
     */
    struct paint *mypaint = paint_create();
    
    /**
     * 무한루프!!!!!
     */
    while (1) {
        ts_read = touch_read(ts_fd, &te);
        
        if (te.touch_state == TOUCH_STATE_BEGIN) {
            paint_touch_start(mypaint, te.x, te.y);
        }
        else if (te.touch_state == TOUCH_STATE_DRAG) {
            paint_touch_drag(mypaint, te.x, te.y);
        }
        else if (te.touch_state == TOUCH_STATE_DONE) {
            paint_touch_end(mypaint, te.x, te.y);
        }
    }
    
    /* 닿을수없는곳 */
}
