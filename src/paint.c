
#include "paint.h"
#include "touch.h"
#include "debug.h"
#include "shape.h"
#include "machine_specific.h"
#include "display.h"
#include "ui.h"
#include "macros.h"

#include <unistd.h> /* for test */

/*************************  [ 이 소스파일에서만 쓰이는 인라인함수들 (시작) ] *************************/

/**
 * 한 점이 캔버스 영역에 속하는지 검사합니다.
 */
static inline bool _point_in_canvas(struct paint *context, int x, int y) {
    return (IN_RANGE(x, context->canvas_x, context->canvas_x + context->canvas_width - 1) &&
            IN_RANGE(y, context->canvas_y, context->canvas_y + context->canvas_height - 1));
}

/**
 * paint 구조체에 적절한 초기값을 대입합니다.
 */
static inline void _init(struct paint *context) {
    context->canvas_x = X(UI_CANVAS_LOCATION);
    context->canvas_y = Y(UI_CANVAS_LOCATION);
    context->canvas_width = WIDTH(UI_CANVAS_SIZE);
    context->canvas_height = HEIGHT(UI_CANVAS_SIZE);

    context->draw_mode = MODE_LINE;
    context->fill = false;
    context->draw_color = PAINT_DEFAULT_DRAW_COLOR;
    context->canvas_color = PAINT_DEFAULT_BACK_COLOR;
    
    context->touch_state = TOUCH_STATE_DONE;
    context->current_action = ACTION_NONE;
    
    LIST_HEAD_REINIT(context->shapes);
}

/**
 * 한 점과 폭, 높이로 이루어진 범위 내의 도형들을 새로 그립니다.
 */
static inline void _redraw_area(struct paint *context, int x, int y, int width, int height) {
    ENSURE_SIZE_POSITIVE(x, y, width, height);
    
    /**
     * 영역을 지우고 (배경색으로)
     */
    disp_draw_rect_fill(x, y, width, height, context->canvas_color);
    
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
    disp_commit_partial(x, y, width, height);
}

/**
 * 두 점으로 이루어진 범위 내의 도형들을 새로 그립니다.
 */
static inline void _redraw_areap(struct paint *context, int x0, int y0, int x1, int y1) {
    ENSURE_POINTS_ORDERED(x0, y0, x1, y1);
    
    /**
     * 영역을 지우고 (배경색으로)
     */
    disp_draw_rectp_fill(x0, y0, x1, y1, context->canvas_color);
    
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

/**
 * 도형을 평행이동하고 새로 그립니다.
 */
static inline void _move_shape_and_redraw(struct paint *context, struct shape *shape, int delta_x, int delta_y) {
    
    SHAPE_EXPORT_AREA_TO_TWO_POINTS(shape, before_x0, before_y0, before_x1, before_y1);
    shape_move(shape, delta_x, delta_y);
    SHAPE_EXPORT_AREA_TO_TWO_POINTS(shape, after_x0, after_y0, after_x1, after_y1);
    
    _redraw_areap(context,
                  MIN(before_x0, after_x0),
                  MIN(before_y0, after_y0),
                  MAX(before_x1, after_x1),
                  MAX(before_y1, after_y1));

    return;
}

/**
 * 도형의 크기를 바꾸고 새로 그립니다.
 */
static inline void _transform_shape_and_redraw(struct paint *context, struct shape *shape, int delta_width, int delta_height) {
    
    SHAPE_EXPORT_AREA_TO_TWO_POINTS(shape, before_x0, before_y0, before_x1, before_y1);
    shape_transform(shape, delta_width, delta_height);
    SHAPE_EXPORT_AREA_TO_TWO_POINTS(shape, after_x0, after_y0, after_x1, after_y1);
    
    _redraw_areap(context,
                  MIN(before_x0, after_x0),
                  MIN(before_y0, after_y0),
                  MAX(before_x1, after_x1),
                  MAX(before_y1, after_y1));
    
    return;
}

/**
 * 지정된 좌표에 존재하는 도형을 선택합니다.
 */
static inline struct shape *_pick_shape(struct paint *context, int x, int y) {
    struct shape *cur;

    list_for_each_entry_reverse(cur, &context->shapes, list) {
        if (shape_point_in_shape_area(cur, x, y)) {
            return cur;
        }
    }
    
    return NULL;
}

/**
 * ui의 배경색을 칠하고 그 위에 윤곽과 글씨를 씁니다.
 */
static inline void _draw_ui_background(unsigned short back_color, unsigned short ui_color) {
    register int offset = 0;
    
    disp_set_direct(true);

    do {
        disp_draw_point(offset % DP_WIDTH,
                        offset / DP_WIDTH,
                        GET_BIT8(UI_IMAGE, offset) ? back_color : ui_color);
   	} while (++offset < DP_MEM_SIZE);
    
    disp_set_direct(false);
}

/**
 * 캔버스를 그립니다.
 */
static inline void _draw_ui_canvas(unsigned short canvas_color) {
    disp_set_direct(true);
    
    disp_draw_rect_fill(X(UI_CANVAS_LOCATION), 
						Y(UI_CANVAS_LOCATION), 
						WIDTH(UI_CANVAS_SIZE), 
						HEIGHT(UI_CANVAS_SIZE), 
						canvas_color);
    
    disp_set_direct(false);
}

/**
 * 색상 선택 팔레트를 그립니다.
 */
static inline void _draw_ui_color_palette(void) {
    disp_set_direct(true);
    
    disp_draw_rect_fill(X(UI_PALETTE_C0_LOCATION),
                        Y(UI_PALETTE_C0_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_WHITE);
    
    disp_draw_rect_fill(X(UI_PALETTE_C1_LOCATION),
                        Y(UI_PALETTE_C1_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_MAGENTA);
    
    disp_draw_rect_fill(X(UI_PALETTE_C2_LOCATION),
                        Y(UI_PALETTE_C2_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_RED);
    
    disp_draw_rect_fill(X(UI_PALETTE_C3_LOCATION),
                        Y(UI_PALETTE_C3_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_GREEN);
    
    disp_draw_rect_fill(X(UI_PALETTE_C4_LOCATION),
                        Y(UI_PALETTE_C4_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_YELLOW);
    
    disp_draw_rect_fill(X(UI_PALETTE_C5_LOCATION),
                        Y(UI_PALETTE_C5_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_BLUE);
    
    disp_draw_rect_fill(X(UI_PALETTE_C6_LOCATION),
                        Y(UI_PALETTE_C6_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_CYAN);
    
    disp_draw_rect_fill(X(UI_PALETTE_C7_LOCATION),
                        Y(UI_PALETTE_C7_LOCATION),
                        WIDTH(UI_PALETTE_ITEM_SIZE),
                        HEIGHT(UI_PALETTE_ITEM_SIZE),
                        COLOR_BLACK);

    disp_set_direct(false);
}

static inline void _draw_ui(void) {
	/**
	 * 헤헷
	 */

	disp_set_direct(true);
	disp_draw_whole(UI_DEFAULT_BACK_COLOR);
	disp_set_direct(false);

	usleep(300000);

    _draw_ui_background(UI_DEFAULT_BACK_COLOR, UI_DEFAULT_TEXT_COLOR);

	usleep(300000);
    
    _draw_ui_canvas(UI_DEFAULT_CANVAS_COLOR);

	usleep(300000);

    _draw_ui_color_palette();
}

static inline int _pick_ui_element(int x, int y) {

    if (POINT_IN_AREA(UI_BUTTON_LINE_LOCATION, UI_BUTTON_ITEM_SIZE_BIG, x, y)) {
        // LINE
        return UI_BTN_LINE;
    }
    else if (POINT_IN_AREA(UI_BUTTON_RECT_LOCATION, UI_BUTTON_ITEM_SIZE_BIG, x, y)) {
        // RECTANGLE
        return UI_BTN_RECT;
    }
    else if (POINT_IN_AREA(UI_BUTTON_OVAL_LOCATION, UI_BUTTON_ITEM_SIZE_BIG, x, y)) {
        // OVAL
        return UI_BTN_OVAL;
    }
    else if (POINT_IN_AREA(UI_BUTTON_FDRAW_LOCATION, UI_BUTTON_ITEM_SIZE_BIG, x, y)) {
        // FREE DRAW
        return UI_BTN_FDRAW;
    }
    else if (POINT_IN_AREA(UI_BUTTON_SELECT_LOCATION, UI_BUTTON_ITEM_SIZE_BIG, x, y)) {
        // SELECT
        return UI_BTN_SELECT;
    }
    else if (POINT_IN_AREA(UI_BUTTON_ERASE_LOCATION, UI_BUTTON_ITEM_SIZE_BIG, x, y)) {
        // ERASE
        return UI_BTN_ERASE;
    }
    else if (POINT_IN_AREA(UI_BUTTON_CLEAR_LOCATION, UI_BUTTON_ITEM_SIZE_BIG, x, y)) {
        // CLEAR
        return UI_BTN_CLEAR;
    }
    else if (POINT_IN_AREA(UI_BUTTON_PEN_LOCATION, UI_BUTTON_ITEM_SIZE_SMALL, x, y)) {
        // PEN
        return UI_BTN_PEN;
    }
    else if (POINT_IN_AREA(UI_BUTTON_FILL_LOCATION, UI_BUTTON_ITEM_SIZE_SMALL, x, y)) {
        // FILL
        return UI_BTN_FILL;
    }

    else if (POINT_IN_AREA(UI_PALETTE_C0_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 0 (white)
        return UI_BTN_C0;
    }
    else if (POINT_IN_AREA(UI_PALETTE_C1_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 1 (magenta)
        return UI_BTN_C1;
    }
    else if (POINT_IN_AREA(UI_PALETTE_C2_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 2 (red)
        return UI_BTN_C2;
    }
    else if (POINT_IN_AREA(UI_PALETTE_C3_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 3 (green)
        return UI_BTN_C3;
    }
    else if (POINT_IN_AREA(UI_PALETTE_C4_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 4 (yellow)
        return UI_BTN_C4;
    }
    else if (POINT_IN_AREA(UI_PALETTE_C5_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 5 (blue)
        return UI_BTN_C5;
    }
    else if (POINT_IN_AREA(UI_PALETTE_C6_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 6 (cyan)
        return UI_BTN_C6;
    }
    else if (POINT_IN_AREA(UI_PALETTE_C7_LOCATION, UI_PALETTE_ITEM_SIZE, x, y)) {
        // COLOR 7 (black)
        return UI_BTN_C7;
    }
  
    return UI_BTN_NONE;
}

/*************************  [ 이 소스파일에서만 쓰이는 인라인함수들 (끝)] *************************/


struct paint *paint_create(void) {
    PAINT_ALLOC(new_paint);
    
    _init(new_paint);
    
    _draw_ui();
    
    return new_paint;
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
    
    if (context->touch_started_from_canvas) {
        /**
         * 캔버스에서 새로운 무언가를 합니다.
         */
        
        print_info("paint_touch_start(): touch inside canvas. current mode is %d.\n", context->mode);
        
        switch (context->draw_mode) {
            case MODE_LINE:
                break;
            case MODE_RECT:
                break;
            case MODE_OVAL:
                break;
            case MODE_FDRAW:
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
    }
    else {
        /**
         * UI 버튼을 처리합니다.
         */
        switch (_pick_ui_element(x, y)) {
            case UI_BTN_LINE:
                context->draw_mode = MODE_LINE;
                print_info("Set draw mode to line.\n");
                break;
            case UI_BTN_RECT:
                context->draw_mode = MODE_RECT;
                print_info("Set draw mode to rect.\n");
                break;
            case UI_BTN_OVAL:
                context->draw_mode = MODE_OVAL;
                print_info("Set draw mode to oval.\n");
                break;
            case UI_BTN_FDRAW:
                context->draw_mode = MODE_FDRAW;
                print_info("Set draw mode to free draw.\n");
                break;
            case UI_BTN_SELECT:
                context->draw_mode = MODE_SELECT;
                print_info("Set draw mode to select.\n");
                break;
            case UI_BTN_ERASE:
                context->draw_mode = MODE_ERASE;
                print_info("Set draw mode to erase.\n");
                break;
            case UI_BTN_CLEAR:
                context->draw_mode = MODE_CLEAR;
                print_info("Set draw mode to clear.\n");
                break;
                
            case UI_BTN_PEN:
                context->fill = false;
                print_info("Set fill mode to pen(non-fill).\n");
                break;
            case UI_BTN_FILL:
                context->fill = true;
                print_info("Set fill mode to fill.\n");
                break;
                
            case UI_BTN_C0:
                context->draw_color = UI_PALETTE_C0_COLOR;
                print_info("Set draw color to c0.\n");
                break;
            case UI_BTN_C1:
                context->draw_color = UI_PALETTE_C1_COLOR;
                print_info("Set draw color to c1.\n");
                break;
            case UI_BTN_C2:
                context->draw_color = UI_PALETTE_C2_COLOR;
                print_info("Set draw color to c2.\n");
                break;
            case UI_BTN_C3:
                context->draw_color = UI_PALETTE_C3_COLOR;
                print_info("Set draw color to c3.\n");
                break;
            case UI_BTN_C4:
                context->draw_color = UI_PALETTE_C4_COLOR;
                print_info("Set draw color to c4.\n");
                break;
            case UI_BTN_C5:
                context->draw_color = UI_PALETTE_C5_COLOR;
                print_info("Set draw color to c5.\n");
                break;
            case UI_BTN_C6:
                context->draw_color = UI_PALETTE_C6_COLOR;
                print_info("Set draw color to c6.\n");
                break;
            case UI_BTN_C7:
                context->draw_color = UI_PALETTE_C7_COLOR;
                print_info("Set draw color to c7.\n");
                break;

            default:
                /**
                 * 아무것도 선택하지 않았어요,,, 허공을 찔렀네요.
                 * 무시!
                 */
                return;
            }
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
    _draw_ui();
    return;
    
    /**
     * 도형 생성
     */
    
    struct shape *line = shape_create(ST_LINEP, 90, 200, 290, 90, COLOR(255, 0, 0));
    shapes_list_add(&context->shapes, line);
    
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
    
    _redraw_areap(context, 0, 0, 319, 239);
    
    /**
     * 이동, 크기변환 테스트
     */
    int init_x = 160;
    int init_y = 120;
    
    struct shape *selected = _pick_shape(context, init_x, init_y);
    
    for (int i = 0; i < 50; ++i) {
        usleep(20000);
        
        _move_shape_and_redraw(context, selected, 2, 1);
    }
    
    for (int i = 0; i < 100; ++i) {
        usleep(20000);
        
        _transform_shape_and_redraw(context, rect, -1, -1);
    }
    
    for (int i = 0; i < 200; ++i) {
        usleep(20000);
        
        _transform_shape_and_redraw(context, rect, 1, 1);
    }
    
    for (int i = 0; i < 30; ++i) {
        usleep(20000);
        
        _transform_shape_and_redraw(context, rect, -2, -1);
    }
     
    for (int i = 0; i < 120; ++i) {
        usleep(20000);
        
        _transform_shape_and_redraw(context, line, 0, -1);
    }
    
}

