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
 * 모든(?) 2차원 그리기 함수는 이 규격을 준수합니다!
 */
typedef         void (*draw_2d)(int, int, int, int, unsigned short);

void            disp_map(int fd);
void 			disp_unmap(void);

void            disp_set_direct(bool value);

void 			disp_draw_point(int x, int y, unsigned short color);

void			disp_draw_linep(int x0, int y0 , int x1, int y1, unsigned short color);

void            disp_draw_rect(int x, int y, int width, int height, unsigned short color);
void 			disp_draw_rect_fill(int x, int y, int width, int height, unsigned short color);

void 			disp_draw_rectp(int x0, int y0, int x1, int y1, unsigned short color);
void            disp_draw_rectp_fill(int x0, int y0, int x1, int y1, unsigned short color);

void            disp_draw_whole(unsigned short color);

void            disp_draw_2d_shape(struct shape *shape);

void            disp_commit(void);
void            disp_commit_partial(int x, int y, int width, int height);
void            disp_commit_partialp(int x0, int y0, int x1, int y1);
void			disp_cancel(void);

void            disp_clear(void);

#endif /* display_h */
