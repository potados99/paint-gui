
#include "paint.h"
#include "touch.h"

/********************************************************************************************/

/**
 * 유틸리티 함수들입니다!!
 */
static inline bool point_in_canvas(struct paint *context, int x, int y) {
    return (IN_RANGE(x, context->canvas_x, context->canvas_x + context->canvas_width - 1) &&
            IN_RANGE(y, context->canvas_y, context->canvas_y + context->canvas_height - 1));
}

/********************************************************************************************/



void paint_touch_start(struct paint *context, int x, int y) {
    NULL_CHECK("paint_touch_start()", context);
    print_trace("paint_touch_start(): touch start at (%d, %d)\n", x, y);

    context->touch_state = TOUCH_STATE_BEGIN;
    
    /**
     * 터치가 시작될 때, 이 터치가 캔버스(그림이 그려지는 부분) 안으로부터 시작된 터치인지 확인하는 flag를
     * 업데이트해줍니다!
     */
    context->touch_started_from_canvas = point_in_canvas(context, x, y);
    
    switch (context->mode) {
        case MODE_LINE:
            break;
        case MODE_RECT:
            break;
        case MODE_OVAL:
            break;
        case MODE_SELECT:
            break;
        case MODE_ERASE:
            break;
        case MODE_CLEAR:
            break;
        default:
            print_error("paint_touch_start(): mode not selecteed.\n");
            return;
    }
    
    
    
    
    
    context->last_x = x;
    context->last_y = y;
}

void paint_touch_drag(struct paint *context, int x, int y) {
    NULL_CHECK("paint_touch_drag()", context);
    print_trace("paint_touch_drag(): touch drag at (%d, %d)\n", x, y);
    
    context->touch_state = TOUCH_STATE_DRAG;
    
    if (!context->touch_started_from_canvas) {
        /**
         * 이 프로그램에서, 캔버스 밖에서 발생한 터치를 시작점으로 하는 드래그는
         * 필요가 없습니다. 따라서 무시합니다!
         */
        print_trace("paint_touch_drag(): ignoring drag.\n");
        return;
    }

    context->last_x = x;
    context->last_y = y;
}

void paint_touch_end(struct paint *context, int x, int y) {
    NULL_CHECK("paint_touch_end()", context);
    print_trace("paint_touch_end(): touch end at (%d, %d)\n", x, y);

    context->touch_state = TOUCH_STATE_DONE;
    
    /**
     * 터치가 끝났으면 어쨌든 그리는 중은 아닌 것이므로 false로 set해줍니다. 확실하게!!
     */
    context->current_action = ACTION_NONE;
    
    context->last_x = x;
    context->last_y = y;
}

