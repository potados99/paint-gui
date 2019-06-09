#ifndef display_h
#define display_h

#include "macros.h"

#define X(POINT)                UPPER16(POINT)
#define Y(POINT)                LOWER16(POINT)

#define WIDTH(SIZE)             UPPER16(SIZE)
#define HEIGHT(SIZE)            LOWER16(SIZE)

#define POINT(X, Y)             COMBINE16(X, Y)
#define SIZE(W, H)              COMBINE16(W, H)

#define DELTA_WIDTH(P0, P1)     ABS(X(P1) - X(P0))
#define DELTA_HEIGHT(P0, P1)    ABS(Y(P1) - Y(P0))

#define IN_RANGE(X, START, END) ((X >= START) && (X <= END))


#define PIXEL(R, G, B) (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))

void            disp_map(int fd);
void 			disp_unmap(void);

int 			disp_draw_point(int point, unsigned short color);
int			    disp_draw_line(int p0, int p1, unsigned short color);
int 			disp_draw_rect(int point, int size, unsigned short color);
int             disp_draw_whole(unsigned short color);

void            disp_commit(void);
void            disp_commit_partial(int point, int size);

void            disp_clear(void);

#endif /* display_h */
