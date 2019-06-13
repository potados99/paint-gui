/**
 * paint.h
 * 그림판 구실을 하게 만들어줄 함수들입니다.
 */
#ifndef paint_h
#define paint_h

#include <stdbool.h>

#include "metric.h"
#include "debug.h"

#define MODE_LINE           1
#define MODE_RECT           2
#define MODE_OVAL           3
#define MODE_SELECT         4
#define MODE_ERASE          5
#define MODE_CLEAR          6

#define ACTION_NONE         0
#define ACTION_DRAW         1
#define ACTION_SELECT       2


#define PAINT(NAME)         \
struct paint NAME = {0}

struct paint {
  
    /**
     * 그림판의 기본적인 맥락(context)입니당.
     * 그리기 모드, 채우기 여부, 색상입니다.
     */
    unsigned char   mode;
    bool            fill;
    unsigned short  color;
    
    /**
     * 그림판 전체 크기 중 캔버스의 위치와 크기를 담아둡니다.
     */
    int             canvas_x;
    int             canvas_y;
    int             canvas_width;
    int             canvas_height;
    
    /**
     * 현재 입력 상태를 저장합니다.
     */
    unsigned char   touch_state;
    
    unsigned char   current_action;
    bool            touch_started_from_canvas;
    
    int             touch_start_x;
    int             touch_start_y;
    
    int             last_x;
    int             last_y;

    
};


/**
 * 터치에 반응하는 함수들입니다.
 * 사용자는 이것만 호출하면 됩니다.
 * 나머지는 얘네가 다~ 알아서~~ ^~^
 */
void paint_touch_start(struct paint *context, int x, int y);
void paint_touch_drag(struct paint *context, int x, int y);
void paint_touch_end(struct paint *context, int x, int y);



#endif /* paint_h */
