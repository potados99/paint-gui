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
 * unsigned short 타입에 적절한 한 픽셀을 만드는 매크로입니다.
 */
#define COLOR(R, G, B) (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))

struct shape {
    /**
     * shape의 종류.
     */
    unsigned char   type;

    /**
     * 모든 shape에 대해 공통인 값들.
     */
    short           values[4];
    unsigned char   zindex;
    unsigned short  color;
    
    /**
     * Free draw를 표현하기 위한 점들을 담은 연결리스트.
     * metric.h의 struct point_node와 struct points 참조하세용.
     */
    struct points   fdraw_points;
};

#endif /* shape_h */
