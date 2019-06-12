/**
 * metric.h
 * 점이나 크기와 같은 규격들을 정의해놓았습니다.
 */

#ifndef metric_h
#define metric_h

#include "macros.h"
#include "list.h"

#include <stdlib.h>

/**
 * 32비트 usigned 정수로부터 상위(X) / 하위(Y) 16비트를 꺼내옵니다.
 */
#define X(POINT)                UPPER16(POINT)
#define Y(POINT)                LOWER16(POINT)

/**
 * 32비트 usigned 정수로부터 상위(WIDTH) / 하위(HEIGHT) 16비트를 꺼내옵니다.
 */
#define WIDTH(SIZE)             UPPER16(SIZE)
#define HEIGHT(SIZE)            LOWER16(SIZE)

/**
 * 두 unsigned 16비트 정수를 하나의 unsigned 32비트 정수로 합칩니다.
 * 점과 크기를 위해 모두 사용할 수 있는데, 둘은 사실 같습니당.
 */
#define POINT(X, Y)             COMBINE16(X, Y)
#define SIZE(W, H)              COMBINE16(W, H)

/**
 * 두 점을 가지고, X방향 길이와 Y방향 길이를 구합니다.
 */
#define DELTA_X(P0, P1)     ABS(X(P1) - X(P0))
#define DELTA_Y(P0, P1)    ABS(Y(P1) - Y(P0))

/**
 * 두 점을 가지고, 가로 길이 차이와 세로 길이 차이를 구합니다.
 */
#define DELTA_WIDTH(P0, P1)     ABS(WIDTH(P1) - WIDTH(P0))
#define DELTA_HEIGHT(P0, P1)    ABS(HEIGHT(P1) - HEIGHT(P0))

/**
 * 정수 X가 START <= X <= END인지 검사합니다.
 */
#define IN_RANGE(X, START, END) ((X >= START) && (X <= END))

/**
 * point_node 구조체를 할당해서 초기화해줍니다.
 */
#define POINT_NODE_ALLOC(NAME) \
struct point_node *NAME = (struct point_node *)malloc(sizeof(struct point_node));   \
memset(NAME, 0, sizeof(struct point_node));                                         \

/**
 * 점을 표현하는 연결리스트에서 쓰일 노드.
 */
struct point_node {
    int                 x;
    int                 y;
    
    struct list_head    list;
};

void points_add(struct list_head *points_head, int x, int y);
void points_free(struct list_head *points_head);

#endif /* metric_h */
