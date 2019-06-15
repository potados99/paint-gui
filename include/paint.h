/**
 * paint.h
 * 그림판 구실을 하게 만들어줄 함수들입니다.
 */
#ifndef paint_h
#define paint_h

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "metric.h"
#include "debug.h"

#define MODE_LINE           0x11
#define MODE_RECT           0x12
#define MODE_OVAL           0x13
#define MODE_FDRAW          0x14
#define MODE_SELECT         0x05
#define MODE_ERASE          0x06

#define ACTION_NONE         0
#define ACTION_DRAW         1
#define ACTION_SELECT       2

#define PAINT(NAME)         \
struct paint NAME = {0}

#define PAINT_ALLOC(NAME)   \
struct paint *NAME = (struct paint *)malloc(sizeof(struct paint));  \
memset(NAME, 0, sizeof(struct paint));

struct paint {
    /**
     * 그림판 전체 크기 중 캔버스의 위치와 크기를 담아둡니다.
     */
    int                 canvas_x;
    int                 canvas_y;
    int                 canvas_width;
    int                 canvas_height;
    
    /**
     * 그림판의 기본적인 맥락(context)입니당.
     * 그리기 모드, 채우기 여부, 색상입니다.
     */
    unsigned char       draw_mode;
    bool                fill;
    unsigned short      draw_color;
    unsigned short      canvas_color;
    
    /**
     * 캔버스에 그려진 도형들을 저장할 list head입니다
     */
    struct list_head shapes;
    
    /**
     * 현재 입력 상태를 저장합니다.
     */
    unsigned char   touch_state;
    
    unsigned char   current_action;
    bool            touch_started_from_canvas;
    
    struct shape    *selected_shape;
    
    int             touch_start_x;
    int             touch_start_y;
    
    int             last_x;
    int             last_y;
};

/**
 * paint 구조체를 새로 만들어서 돌려줍니다.
 * 할당도 초기화도 다해주는 착한녀석..^^ 그냥 갖다 쓰기만 하면 돼요..ㅎㅎ
 */
struct paint *paint_create(void);

/**
 * 터치에 반응하는 함수들입니다.
 * 사용자는 이것만 호출하면 됩니다.
 * 나머지는 얘네가 다~ 알아서~~ ^~^
 */
void paint_touch_start(struct paint *context, int x, int y);
void paint_touch_drag(struct paint *context, int x, int y);
void paint_touch_end(struct paint *context, int x, int y);

void paint_test(struct paint *context);

#endif /* paint_h */
