/**
 *
 *
 */

#ifndef shape_h
#define shape_h

#include "metric.h"
#include "list.h"

#include <stdlib.h>
#include <string.h>

/**
 * Shape 종류 (ST: Shape Type).
 * 이 프로그램에서, 도형의 위치와 범위는 시작점과 끝점 두개로 표현합니다.
 */
#define ST_LINE         1   /* 두 점으로 표현 */
#define ST_RECT         2   /* 한 점과 길이+높이로 표현 */
#define ST_RECT_FILL    3   /* 한 점과 길이+높이로 표현 */
#define ST_RECTP        4   /* 두 점으로 표현*/
#define ST_RECTP_FILL   5   /* 두 점으로 표현*/
#define ST_OVAL         6   /* 한 점과 길이+높이로 표현 */
#define ST_OVAL_FILL    7   /* 한 점과 길이+높이로 표현 */
#define ST_OVALP        8   /* 두 점으로 표현*/
#define ST_OVALP_FILL   9   /* 두 점으로 표현*/
#define ST_FDRAW        10  /* 두 점으로 표현 */

/**
 * unsigned int 타입에 적절한 한 픽셀을 만드는 매크로입니다.
 */
#define COLOR(R, G, B)  \
(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))

/**
 * zindex는 shape를 만들 때마다 계속해서 늘어나야 합니다.
 * 그래서 전역 static 변수로 만들어서 계속 늘려줍니다.
 */
static unsigned int zindex_count = 0;

/**
 * 새로운 shape 구조체를 만드는 매크로입니다. 0으로 초기화 해줍니다.
 */
#define SHAPE(NAME)                                                     \
struct shape NAME = {0};                                                \
do {                                                                    \
NAME.zindex = zindex_count++;                                           \
} while (0)

/**
 * 새로운 shape 구조체를 힙에다 만들고 초기화합니다.
 */
#define SHAPE_ALLOC(NAME)                                               \
struct shape *NAME = (struct shape *)malloc(sizeof(struct shape));      \
do {                                                                    \
memset(NAME, 0, sizeof(struct shape));                                  \
NAME->zindex = zindex_count++;                                           \
} while (0)

/*
#define SHAPE_FREE(PTR)                                                 \
do {                                                                    \
if (PTR->fdraw_points.head != NULL) {                                   \
points_free(&PTR->fdraw_points);                                        \
}                                                                       \
free(PTR);                                                              \
} while (0)
*/

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
     * 도형의 높낮이(z축)를 저장합니다. 숫자가 높을수록 사용자와 가깝습니다.
     * 65535를 설마 넘겠어...
     */
    unsigned short      zindex;
    
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
 * shape를 평행이동합니다. shape 구조체 내의 offset 필드에 추가됩니다.
 */
void shape_move(struct shape *shape, int delta_x, int delta_y);

/**
 * 자유 그리기 도형에 새로운 점을 추가해줍니다.
 */
void shape_add_point(struct shape *shape, int x, int y);

#endif /* shape_h */
