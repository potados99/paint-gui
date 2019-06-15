/**
 * shape.h
 * 그림판에 등장하는 다양한 도형들을 담기 위한 자료구조와 매크로, 함수들을 포함합니다.
 */

#ifndef shape_h
#define shape_h

#include "metric.h"
#include "macros.h"
#include "list.h"
#include "color.h"

#include <stdlib.h>
#include <string.h>

/**
 * Shape 종류 (ST: Shape Type).
 * 이 프로그램에서, 도형의 위치와 범위는 시작점과 끝점 두개로 표현합니다.
 */
#define ST_LINEP        0x11   /* 두 점으로 표현 */
#define ST_RECT         0x02   /* 한 점과 길이+높이로 표현 */
#define ST_RECT_FILL    0x03   /* 한 점과 길이+높이로 표현 */
#define ST_RECTP        0x14   /* 두 점으로 표현*/
#define ST_RECTP_FILL   0x15   /* 두 점으로 표현*/
#define ST_OVAL         0x06   /* 한 점과 길이+높이로 표현 */
#define ST_OVAL_FILL    0x07   /* 한 점과 길이+높이로 표현 */
#define ST_OVALP        0x18   /* 두 점으로 표현*/
#define ST_OVALP_FILL   0x19   /* 두 점으로 표현*/
#define ST_FREEP        0x1A  /* 두 점으로 표현 */

#define SHAPE_BY_TWO_POINTS(TYPE) \
(TYPE & 0x10)

/**
 * shape가 나타내는 offset을 포함한 실제 영역을 두 개의 양 끝점으로 표현해줍니다.
 * 이미 존재하는 함수를 받아서 사용합니다.
 */
#define SHAPE_EXPORT_AREA_TO_TWO_POINTS_REUSE(SHAPE_PTR, X0, Y0, X1, Y1)            \
X0 = SHAPE_PTR->value[0];                                                           \
Y0 = SHAPE_PTR->value[1];                                                           \
X1 = SHAPE_PTR->value[2];                                                           \
Y1 = SHAPE_PTR->value[3];                                                           \
do {                                                                                \
    if (SHAPE_BY_TWO_POINTS(SHAPE_PTR->type)) {                                     \
        ENSURE_POINTS_ORDERED(X0, Y0, X1, Y1);                                      \
        X1 += SHAPE_PTR->offset[0];                                                 \
        Y1 += SHAPE_PTR->offset[1];                                                 \
    }                                                                               \
    else {                                                                          \
        ENSURE_SIZE_POSITIVE(X0, Y0, X1, Y1);                                       \
        X1 += X0 - (X1 > 0);                                                        \
        Y1 += Y0 - (Y1 > 0);                                                        \
    }                                                                               \
    X0 += SHAPE_PTR->offset[0];                                                     \
    Y0 += SHAPE_PTR->offset[1];                                                     \
} while (0)

/**
 * shape가 나타내는 offset을 포함한 실제 영역을 한 개의 점과 width와 height로 표현해줍니다.
 * 이미 존재하는 변수를 받아서 사용합니다.
 */
#define SHAPE_EXPORT_AREA_TO_POINT_AND_SIZE_REUSE(SHAPE_PTR, X, Y, WIDTH, HEIGHT)   \
X = SHAPE_PTR->value[0];                                                            \
Y = SHAPE_PTR->value[1];                                                            \
WIDTH = SHAPE_PTR->value[2];                                                        \
HEIGHT = SHAPE_PTR->value[3];                                                       \
do {                                                                                \
    if (SHAPE_BY_TWO_POINTS(SHAPE_PTR->type)) {                                     \
        ENSURE_POINTS_ORDERED(X, Y, WIDTH, HEIGHT);                                 \
        WIDTH = WIDTH - X + 1;                                                      \
        HEIGHT = HEIGHT - Y + 1;                                                    \
    }                                                                               \
    else {                                                                          \
        ENSURE_SIZE_POSITIVE(X, Y, WIDTH, HEIGHT);                                  \
    }                                                                               \
    X += SHAPE_PTR->offset[0];                                                      \
    Y += SHAPE_PTR->offset[1];                                                      \
} while (0)

/**
 * 위와 같으나 여기서는 변수를 새로 할당해줍니다.
 */
#define SHAPE_EXPORT_AREA_TO_TWO_POINTS(SHAPE_PTR, X0, Y0, X1, Y1)                  \
int X0, Y0, X1, Y1;                                                                 \
SHAPE_EXPORT_AREA_TO_TWO_POINTS_REUSE(SHAPE_PTR, X0, Y0, X1, Y1);

/**
 * 얘도 마찬가지!
 */
#define SHAPE_EXPORT_AREA_TO_POINT_AND_SIZE(SHAPE_PTR, X, Y, WIDTH, HEIGHT)         \
int X, Y, WIDTH, HEIGHT;                                                            \
SHAPE_EXPORT_AREA_TO_POINT_AND_SIZE_REUSE(SHAPE_PTR, X, Y, WIDTH, HEIGHT);


/**
 * 새로운 shape 구조체를 만드는 매크로입니다. 0으로 초기화 해줍니다.
 */
#define SHAPE(NAME)                                                     \
struct shape NAME = {0};                                                \

/**
 * 새로운 shape 구조체를 힙에다 만들고 초기화합니다.
 */
#define SHAPE_ALLOC(NAME)                                               \
struct shape *NAME = (struct shape *)malloc(sizeof(struct shape));      \
memset(NAME, 0, sizeof(struct shape));                                  \

/**
 * 이 프로그램에 필요한 모든 도형을 표현할 수 있는 구조체입니다.
 * 새로운 shape를 만들 때에는 SHAPE(NAME) 매크로를 사용해주세용.
 */
struct shape {
    /**
     * shape의 종류.
     */
    unsigned char       type;

    /**
     * 모든 shape에 대해 공통인 값들.
     * Line은 당연히 두 점의 좌표(x0, y0, x1, y1)로 저장합니다.
     * Rect는 두 점의 좌표로 저장하는게 기본이며, 이때 type은 ST_RECTP입니다.
     * 한 점의 좌표와 길이, 높이로 저장할 수도 있습니다. 이때 type은 ST_RECT입니다.
     * 타원 또한 마찬가지입니다.
     * 직선을 제외한 2차원 평면도형은 모두 시작과 끝점을 기준으로 영역 또는 크기를 다룹니다.
     * Freedraw의 경우는 두 점으로 영역의 시작과 끝을 표현합니다.
     */
    int                 value[4];
    
    /**
     * 도형을 평행이동하면, 원래의 좌표를 건드리지 않고 이 offset을 수정합니다.
     * offset은 음수일 수도, 양수일 수도 있습니다!
     */
    int                 offset[2];
    
    /**
     * 예..그냥 한 픽셀 컬러입니다.
     */
    unsigned short      color;
    
    /**
     * Free draw를 표현하기 위한 점들을 담은 연결리스트입니다.
     * metric.h의 struct point_node와 struct points 참조하세용.
     */
    struct list_head    fdraw_points;
    
    /**
     * shape들의 연결리스트입니다.
     */
    struct list_head list;
};


/**
 * 생성과 할당이 여기서 일어납니다!
 */
struct shape *shape_create(unsigned char type,
                           int v0,
                           int v1,
                           int v2,
                           int v3,
                           unsigned short color);

/**
 * shape를 한방에 싹 지워줍니다.
 */
void shape_delete(struct shape *shape);

/**
 * 힙에 존재하는 shape를 리스트 끝에 이어줍니다.
 */
void shapes_list_add(struct list_head *shapes_head, struct shape *shape);

/**
 * 리스트 맨 끝에 있는 shape를 뽑아줍니다.
 */
struct shape *shapes_list_peek_last(struct list_head *shapes_head);

/**
 * 특정 점이 특정 shape 영역 내에 있는지 판단합니다.
 */
bool shape_point_in_shape_area(struct shape *shape, int x, int y);

/**
 * shape를 평행이동합니다. shape 구조체 내의 offset 필드에 추가됩니다.
 */
void shape_move(struct shape *shape, int delta_x, int delta_y);

/**
 * shape의 크기를 바꿉니다. value 배열에 직접 변경이 가해집니다.
 */
void shape_transform(struct shape *shape, int delta_width, int delta_height);

/**
 * 자유 그리기 도형에 새로운 점을 추가해줍니다.
 */
void shape_add_point(struct shape *shape, int x, int y);

#endif /* shape_h */
