
#include "paint.h"
#include "touch.h"
#include "debug.h"
#include "shape.h"
#include "machine_specific.h"
#include "display.h"

/********************************************************************************************/

/**
 * 유틸리티 함수들입니다!!
 */
static inline bool _point_in_canvas(struct paint *context, int x, int y) {
    return (IN_RANGE(x, context->canvas_x, context->canvas_x + context->canvas_width - 1) &&
            IN_RANGE(y, context->canvas_y, context->canvas_y + context->canvas_height - 1));
}

static inline void _init(struct paint *context) {
    context->mode = MODE_LINE;
    context->fill = false;
    context->draw_color = PAINT_DEFAULT_DRAW_COLOR;
    context->back_color = PAINT_DEFAULT_BACK_COLOR;
    
    context->touch_state = TOUCH_STATE_DONE;
    context->current_action = ACTION_NONE;
    
    LIST_HEAD_REINIT(context->shapes);
}

static inline void _redraw_area(struct paint *context, int x0, int y0, int x1, int y1) {
    /**
     * 영역을 지우고 (배경색으로)
     */
    disp_draw_rectp(x0, y0, x1, y1, context->back_color);
    
    /**
     * 새로 그리고
     */
    struct shape *cur;
    list_for_each_entry(cur, &context->shapes, list) {
        disp_draw_2d_shape(cur);
    }
    
    /** 
     * 그 변화를 commit합니다.
     */
    disp_commit_partialp(x0, y0, x1, y1);
}

static inline void _move_shape_and_redraw(struct paint *context, struct shape *shape, int delta_x, int delta_y) {

    SHAPE_VALUES_TO_TWO_POINTS(shape, redraw_x0, redraw_y0, redraw_x1, redraw_y1);

    shape_move(shape, delta_x, delta_y);
    
    _redraw_area(context, redraw_x0, redraw_y0, redraw_x1, redraw_y1);

    return;
}

static inline void _transform_shape_and_redraw(struct paint *context, struct shape *shape, int delta_width, int delta_height) {
    
    SHAPE_VALUES_TO_TWO_POINTS(shape, redraw_x0, redraw_y0, redraw_x1, redraw_y1);

    shape_transform(shape, delta_width, delta_height);
    
    _redraw_area(context, redraw_x0, redraw_y0, redraw_x1, redraw_y1);
    
    return;
}

static inline struct shape *_pick_shape(struct paint *context, int x, int y) {
    struct shape *cur;

    list_for_each_entry_reverse(cur, &context->shapes, list) {
        if (shape_point_in_shape_area(cur, x, y)) {
            return cur;
        }
    }
    
    return NULL;
}
/********************************************************************************************/

struct paint *paint_create(int x, int y, int width, int height) {
    PAINT_ALLOC(new_paint);
    
    new_paint->canvas_x = x;
    new_paint->canvas_y = y;
    new_paint->canvas_width = width;
    new_paint->canvas_height = height;
    
    _init(new_paint);
    
    return new_paint;
}

struct paint *paint_createp(int x0, int y0, int x1, int y1) {
    return paint_create(MIN(x0, x1), MIN(y0, y1), ABS(x1 - x0) + 1, ABS(y1 - y0) + 1);
}

void paint_touch_start(struct paint *context, int x, int y) {
    NULL_CHECK("paint_touch_start()", context);
    print_trace("paint_touch_start(): touch start at (%d, %d)\n", x, y);

    context->touch_state = TOUCH_STATE_BEGIN;
    
    /**
     * 터치가 시작될 때, 이 터치가 캔버스(그림이 그려지는 부분) 안으로부터 시작된 터치인지 확인하는 flag를
     * 업데이트해줍니다!
     */
    context->touch_started_from_canvas = _point_in_canvas(context, x, y);
    
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

void paint_test(struct paint *context) {
    struct shape *rect = shape_create(ST_RECT_FILL, 140, 110, 50, 50, COLOR(0, 255, 0));
    shapes_list_add(&context->shapes, rect);
    
    struct shape *fdraw = shape_create(ST_FREEP, 0, 0, 0, 0, COLOR(0, 0, 255));
    shape_add_point(fdraw, 100, 100);
    shape_add_point(fdraw, 102, 106);
    shape_add_point(fdraw, 106, 112);
    shape_add_point(fdraw, 112, 119);
    shape_add_point(fdraw, 120, 125);
    shape_add_point(fdraw, 130, 131);
    shape_add_point(fdraw, 142, 137);
    shape_add_point(fdraw, 156, 142);
    shape_add_point(fdraw, 172, 150);
    shapes_list_add(&context->shapes, fdraw);
    
    _redraw_area(context, 0, 0, 319, 239);
    
    int init_x = 160;
    int init_y = 120;
    
    struct shape *selected = _pick_shape(context, init_x, init_y);
    
    for (int i = 0; i < 50; ++i) {
        usleep(20000);
        
        _move_shape_and_redraw(context, selected, 1, 1);
    }
}

