/**
 * metric.h
 * 점이나 크기와 같은 규격들을 정의해놓았습니다.
 */

#ifndef metric_h
#define metric_h

#include "macros.h"
#include "list.h"

#include <stdlib.h>
#include <string.h>

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
#define DELTA_X(P0, P1)         ABS(X(P1) - X(P0))
#define DELTA_Y(P0, P1)         ABS(Y(P1) - Y(P0))

/**
 * 두 점을 가지고, 가로 길이 차이와 세로 길이 차이를 구합니다.
 */
#define DELTA_WIDTH(P0, P1)     ABS(WIDTH(P1) - WIDTH(P0))
#define DELTA_HEIGHT(P0, P1)    ABS(HEIGHT(P1) - HEIGHT(P0))

#define POINT_IN_AREA(LOCATION, SIZE, _X, _Y)       \
(IN_RANGE(_X, X(LOCATION), (X(LOCATION) + WIDTH(SIZE) - 1))) && (IN_RANGE(_Y, Y(LOCATION), (Y(LOCATION) + HEIGHT(SIZE) - 1)))

/**
 * 한 점의 X, Y 좌표가 각각 [0, X_MAX], [0, Y_MAX]에 속하는지 검사합니다.
 */
#define POINT_CHECK(FUNC_NAME, X, Y, X_MAX, Y_MAX)                                          \
do {                                                                                        \
ASSERTDO(IN_RANGE(X, 0, X_MAX),                                                             \
print_error(FUNC_NAME ": " #X "{%d} out of range.\n", X);                                   \
return);                                                                                    \
ASSERTDO(IN_RANGE(Y, 0, Y_MAX),                                                             \
print_error(FUNC_NAME ": " #Y "{%d} out of range.\n", Y);                                   \
return);                                                                                    \
} while (0)

/**
 * 한 크기의 길이와 높이가 각각 [0, W_MAX], [0, H_MAX]에 속하는지 검사합니다.
 */
#define SIZE_CHECK(FUNC_NAME, W, H, W_MAX, H_MAX)                                           \
do {                                                                                        \
ASSERTDO(IN_RANGE(W, 0, W_MAX),                                                             \
print_error(FUNC_NAME ": " #W "{%d} out of range.\n", W);                                   \
return);                                                                                    \
ASSERTDO(IN_RANGE(H, 0, H_MAX),                                                             \
print_error(FUNC_NAME ": " #H "{%d} out of range.\n", H);                                   \
return);                                                                                    \
} while (0)

/**
 * 폭과 높이 모두 양수임이 확실하게 해줍니다.
 */
#define ENSURE_SIZE_POSITIVE(X, Y, WIDTH, HEIGHT)   \
do {                                                \
    if (WIDTH < 0) {                                \
        X += WIDTH + 1;                             \
        WIDTH = -WIDTH;                             \
    }                                               \
    if (HEIGHT < 0) {                               \
        Y += HEIGHT + 1;                            \
        HEIGHT = -HEIGHT;                           \
    }                                               \
} while (0)

/**
 * (x0, y0)이 좌상단, (x1, y1)이 우상단임을 확실하게 해줍니다.
 */
#define ENSURE_POINTS_ORDERED(X0, Y0, X1, Y1)       \
do {                                                \
    ENSURE_RIGHT_BIGGER(X0, X1);                    \
    ENSURE_RIGHT_BIGGER(Y0, Y1);                    \
} while (0)

/**
 * 정수 X가 START <= X <= END인지 검사합니다.
 */
#define IN_RANGE(X, START, END) ((X >= (START)) && (X <= (END)))

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
