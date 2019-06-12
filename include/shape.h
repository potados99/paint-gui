/**
 *
 *
 */

#ifndef shape_h
#define shape_h

#include "metric.h"

/**
 * Shape 종류 (ST: Shape Type).
 */
#define ST_LINE         1
#define ST_RECT         2
#define ST_RECT_FILL    3
#define ST_RECTP        4
#define ST_RECTP_FILL   5
#define ST_OVAL         6
#define ST_OVAL_FILL    7
#define ST_FDRAW        8

/**
 * unsigned int 타입에 적절한 한 픽셀을 만드는 매크로입니다.
 */
#define COLOR(R, G, B)  \
(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))


/**
 * zindex는 shape를 만들 때마다 계속해서 늘어나야 합니다.
 * 그래서 전역 static 변수로 만들어서 계속 늘려줍니다.
 */
static unsigned int zindex = 0;

/**
 * 새로운 shape 구조체를 만드는 매크로입니다. 0으로 초기화 해줍니다.
 */
#define SHAPE(NAME)     \
struct shape NAME = {0}

/**
 * 이 프로그램에 필요한 모든 도형을 표현할 수 있는 구조체입니다.
 * 새로운 shape를 만들 때에는 SHAPE(NAME) 매크로를 사용해주세용.
 */
struct shape {
    /**
     * shape의 종류.
     */
    unsigned char   type;

    /**
     * 모든 shape에 대해 공통인 값들.
     */
    int             value[4];
    
    /**
     * 도형을 평행이동하면, 원래의 좌표를 건드리지 않고 이 offset을 수정합니다.
     * offset은 음수일 수도, 양수일 수도 있습니다!
     */
    int             offset[2];
    
    /**
     * 도형의 높낮이(z축)를 저장합니다. 숫자가 높을수록 사용자와 가깝습니다.
     */
    unsigned int    zindex;
    
    /**
     * 예..그냥 한 픽셀 컬러입니다.
     */
    unsigned int    color;
    
    /**
     * Free draw를 표현하기 위한 점들을 담은 연결리스트입니다.
     * metric.h의 struct point_node와 struct points 참조하세용.
     */
    struct points   fdraw_points;
};

void shape_move(struct shape *shape, int delta_x, int delta_y);

#endif /* shape_h */
