/**
 * display.h
 * 디스플레이를 다루기 위한 함수들을 잘 모아놓았습니다.
 * 모듈이나 객체의 형태로 제공되지는 않습니다.
 * 따라서 디스플레이 한 개만 쓸 수 있어요.. 두개 쓸 일이 없기야 하지만은...
 */

#ifndef display_h
#define display_h

#include "macros.h"
#include "shape.h"
#include <stdbool.h>

/**
 * 범위 검사하는 매크로입니다.
 */
#define IN_RANGE(X, START, END) ((X >= START) && (X <= END))

void            disp_map(int fd);
void 			disp_unmap(void);

void            disp_set_direct(bool value);

void 			disp_draw_point(short x, short y, unsigned short color);

void			disp_draw_line(short x0, short y0 , short x1, short y1, unsigned short color);

void            disp_draw_rect(short x, short y, short width, short height, unsigned short color);
void 			disp_draw_rect_fill(short x, short y, short width, short height, unsigned short color);

void 			disp_draw_rectp(short x0, short y0, short x1, short y1, unsigned short color);
void            disp_draw_rectp_fill(short x0, short y0, short x1, short y1, unsigned short color);

void            disp_draw_whole(unsigned short color);

void            disp_draw_shape(struct shape *shape);

void            disp_commit(void);
void            disp_commit_partial(short x, short y, short width, short height);
void            disp_commit_partialp(short x0, short y0, short x1, short y1);
void			disp_cancel(void);

void            disp_clear(void);

#endif /* display_h */
