//
//  display.cpp
//  paint-gui
//
//  Created by POTADOS on 09/06/2019.
//  Copyright © 2019 potados. All rights reserved.
//

#include "display.h"
#include "color.h"
#include "debug.h"
#include "macros.h"

#include <algorithm>

/**
 * Local usage.
 */
#define POINT_CHECK(FUNCTION_NAME, POINT_NAME)                                          \
do {                                                                                    \
ASSERTDO(POINT_NAME.x() >= 0,                                                           \
print_info(FUNCTION_NAME ": " #POINT_NAME ".x(%d) under zero.\n", POINT_NAME.x());      \
return -1);                                                                             \
ASSERTDO(POINT_NAME.y() >= 0,                                                           \
print_info(FUNCTION_NAME ": " #POINT_NAME ".y(%d) under zero.\n", POINT_NAME.y());      \
return -1);                                                                             \
} while(0)

#define SIZE_CHECK(FUNCTION_NAME, SIZE_NAME)                                            \
do {                                                                                    \
ASSERTDO(SIZE_NAME.width() >= 0,                                                        \
print_info(FUNCTION_NAME ": "#SIZE_NAME ".x(%d) under zero.\n", SIZE_NAME.width());     \
return -1;);                                                                            \
ASSERTDO(SIZE_NAME.height() >= 0,                                                       \
print_info(FUNCTION_NAME ": "#SIZE_NAME ".y(%d) under zero.\n", SIZE_NAME.height());    \
return -1;);                                                                            \
} while(0)


/******************************
 * Public
 ******************************/

int display::draw_point(point p, color c) {
    POINT_CHECK("display::draw_point(point, color", p);

    return _draw_point(p.x(), p.y(), c);
}

int display::draw_line(point p0, point p1, color c) {
    POINT_CHECK("display::draw_line(point, point, color", p0);
    POINT_CHECK("display::draw_line(point, point, color)", p1);
    
    return _draw_line(p0.x(), p0.y(), p1.x(), p1.y(), c);
}

int display::draw_rect(point p0, point p1, color c) {
    POINT_CHECK("display::draw_rect(point, point, color)", p0);
    POINT_CHECK("display::draw_rect(point, point, color)", p1);
    
    int x;
    int y;
    int width;
    int height;

    _points_to_point_and_size(p0.x(), p0.y(), p1.x(), p1.y(), &x, &y, &width, &height);
    
    return _draw_rect(x, y, width, height, c);
}

int display::drwa_rect(point p, size s, color c) {
    POINT_CHECK("display::draw_rect(point, size, color)", p);
    SIZE_CHECK("display::draw_rect(point, size, color)", s);
    
    return _draw_rect(p.x(), p.y(), s.width(), s.height(), c);
}

int display::clear() {
    return clear(color(0, 0, 0));
}
int display::clear(color c) {
    return _draw_rect(0, 0, width_, height_, c);
}

int display::commit() {
    return _commit(0, 0, width_, height_);
}
int display::commit(point p0, point p1) {
    POINT_CHECK("display::commit(point, point)", p0);
    POINT_CHECK("display::commit(point, point)", p1);

    return 0;
}
int display::commit(point p, size s) {
    POINT_CHECK("display::draw_rect(point, size, color)", p);
    SIZE_CHECK("display::draw_rect(point, size, color)", s);

    return 0;
}


/******************************
 * Private
 ******************************/

int display::_draw_point(int x, int y, color c) {
    return 0;

}
int display::_draw_line(int x0, int y0, int x1, int y1, color c) {
    return 0;

}
int display::_draw_rect(int x, int y, int width, int height, color c) {
    return 0;

}
int display::_commit(int x, int y, int width, int height) {
    return 0;
}



int display::_points_to_point_and_size(int x0, int y0, int x1, int y1, int *x, int *y, int *width, int *height) {
    /**
     * x0 and y0 must be smaller than x1 and y1.
     */
    if (x0 > x1) {
        SWAP(x0, x1);
    }
    if (y0 > y1) {
        SWAP(y0, y1);
    }
    
    *x = x0;
    *y = y0;
    
    *width = x1 - x0;
    *height = y1 - y0;
    
    return 0;
}
