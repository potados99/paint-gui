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
#include "machine_specific.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

/**
 * Local usage.
 */
#define POINT_CHECK(FUNCTION_NAME, POINT_NAME)                                          \
do {                                                                                    \
ASSERTDO(POINT_NAME.x() >= 0 && POINT_NAME.x() < DP_WIDTH,                              \
print_info(FUNCTION_NAME ": " #POINT_NAME ".x(%d) out of range.\n", POINT_NAME.x());    \
return -1);                                                                             \
ASSERTDO(POINT_NAME.y() >= 0 && POINT_NAME.y() < DP_HEIGHT,                             \
print_info(FUNCTION_NAME ": " #POINT_NAME ".y(%d) out of range.\n", POINT_NAME.y());    \
return -1);                                                                             \
} while(0)

#define SIZE_CHECK(FUNCTION_NAME, SIZE_NAME)                                            \
do {                                                                                    \
ASSERTDO(SIZE_NAME.width() >= 0 && SIZE_NAME.width() <= DP_WIDTH,                       \
print_info(FUNCTION_NAME ": " #SIZE_NAME ".x(%d) out of range.\n", SIZE_NAME.width());  \
return -1;);                                                                            \
ASSERTDO(SIZE_NAME.height() >= 0 && SIZE_NAME.height() <= DP_HEIGHT,                    \
print_info(FUNCTION_NAME ": " #SIZE_NAME ".y(%d) out of range.\n", SIZE_NAME.height()); \
return -1;);                                                                            \
} while(0)


/******************************
 * Public
 ******************************/

display::display(const char *dp_fd_path) {
    width_ = DP_WIDTH;
    height_ = DP_HEIGHT;
    
    /**
     * Open and get fd.
     */
    ASSERTDO((fd_ = open(dp_fd_path, O_RDWR)) != -1,
             print_error("display::display(const char *): open() error with %s.\n", dp_fd_path);
             exit(1));
    
    /**
     * Map fd to memory.
     */
    ASSERTDO(map() == 0,
             print_error("display::display(const char *): map() failed.\n");
             exit(1));
    
    /**
     * Setup buffer and bitmap.
     */
    buf_ = (unsigned short *)malloc(sizeof(unsigned short) * width_ * height_ + 1);
    ASSERTDO(buf_ != NULL, print_error("display::display(const char *): malloc() failed.\n"); exit(1));

    bitmap_ = (unsigned long *)malloc(sizeof(unsigned long) * (width_ * height_ / 32) + 1);
    ASSERTDO(bitmap_ != NULL, print_error("display::display(const char *): malloc() failed.\n"); exit(1));

    print_info("display initialized.\n");
}

display::~display() {
    unmap();
    close(fd_);
    free(buf_);
    free(bitmap_);
}

int display::draw_point(point p, color16 c) {
    POINT_CHECK("display::draw_point(point, color", p);

    return _draw_point(p.x(), p.y(), c);
}

int display::draw_line(point p0, point p1, color16 c) {
    POINT_CHECK("display::draw_line(point, point, color", p0);
    POINT_CHECK("display::draw_line(point, point, color)", p1);
    
    return _draw_line(p0.x(), p0.y(), p1.x(), p1.y(), c);
}

int display::draw_rect(point p0, point p1, color16 c) {
    POINT_CHECK("display::draw_rect(point, point, color)", p0);
    POINT_CHECK("display::draw_rect(point, point, color)", p1);
    
    int x;
    int y;
    int width;
    int height;

    _points_to_point_and_size(p0.x(), p0.y(), p1.x(), p1.y(), &x, &y, &width, &height);
    
    return _draw_rect(x, y, width, height, c);
}

int display::draw_rect(point p, size s, color16 c) {
    POINT_CHECK("display::draw_rect(point, size, color)", p);
    SIZE_CHECK("display::draw_rect(point, size, color)", s);
    
    return _draw_rect(p.x(), p.y(), s.width(), s.height(), c);
}

int display::clear() {
    // return clear(color::black);
    memset(mem_, 0, sizeof(unsigned short) * width_ * height_);
    return 0;
}

int display::clear(color16 c) {
    return _draw_rect(0, 0, width_, height_, c);
}

int display::commit() {
    return _commit(0, 0, width_, height_);
}

int display::commit(point p0, point p1) {
    POINT_CHECK("display::commit(point, point)", p0);
    POINT_CHECK("display::commit(point, point)", p1);

    int x;
    int y;
    int width;
    int height;
    
    _points_to_point_and_size(p0.x(), p0.y(), p1.x(), p1.y(), &x, &y, &width, &height);
    
    return _commit(x, y, width, height);
}

int display::commit(point p, size s) {
    POINT_CHECK("display::draw_rect(point, size, color)", p);
    SIZE_CHECK("display::draw_rect(point, size, color)", s);

    return _commit(p.x(), p.y(), s.width(), s.height());
}


/******************************
 * Private
 ******************************/

int display::map() {
    mem_ = (unsigned short *)mmap(NULL, sizeof(unsigned short) * width_ * height_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    
    ASSERTDO(mem_ != MAP_FAILED,
             perror("error while map()");
             print_error("display::map(): map failed with fd %d.\n", fd_);
             return -1;);
    
    return 0;
}

int display::unmap() {
    return munmap(mem_, width_ * height_ * sizeof(unsigned short));
}

int display::_draw_point(int x, int y, color16 c) {
    _write(x + (y * width_), c);
    
    return 0;
}

int display::_draw_line(int x0, int y0, int x1, int y1, color16 c) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            return _draw_line_low(x1, y1, x0, y0, c);
        }
        else {
            return _draw_line_low(x0, y0, x1, y1, c);
        }
    }
    else {
        if (y0 > y1) {
            return _draw_line_high(x1, y1, x0, y0, c);
        }
        else {
            return _draw_line_high(x0, y0, x1, y1, c);
        }
    }
}

int display::_draw_line_low(int x0, int y0, int x1, int y1, color16 c) {
	int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    
    int D = 2*dy - dx;
    int y = y0;
    
    for (int x = x0; x <= x1; ++x) {
        _write(x + (y * width_), c);

        if (D > 0) {
            y += yi;
            D -= 2*dx;
        }
        
        D += 2*dy;
    }
    
    return 0;
}

int display::_draw_line_high(int x0, int y0, int x1, int y1, color16 c) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    
    int D = 2*dx - dy;
    int x = x0;
    
    for (int y = y0; y <= y1; ++y) {
        _write(x + (y * width_), c);

        if (D > 0) {
            x += xi;
            D -= 2*dy;
        }
        
        D += 2*dx;
    }
    return 0;
}

int display::_draw_rect(int x, int y, int width, int height, color16 c) {
	
	const int 		offset_max = (x + width - 1) + (width_ * (y + height - 1));
	register int 	offset = x + (width_* y);
	
	register int 	n_line = 0;

	do {
        _write(offset, c);
	
		if (++n_line >= width) {
			n_line = 0;
			offset += (width_ - width);
		} 
		
		++offset;
		
	} while (offset <= offset_max);

    return 0;
}

int display::_commit(int x, int y, int width, int height) {   
    _apply(x, y, width, height);

	print_trace("commit changes: x: %d, y: %d, width: %d, height: %d.\n", x, y, width, height);
    
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

void display::_write(int offset, color16 c) {
#ifndef UNSAFE
    ASSERTDO(offset >= 0 && offset < width_ * height_,
             print_error("display::_write(int, color): offset out of range.\n");
             return);
#endif
    
    *(buf_ + offset) = c.to_short();
    _set_bit(offset, 1);

	print_trace("wrote buf[%d] = %d.\n", offset, c.to_short());
}

void display::_apply(int x, int y, int width, int height) {
#ifndef UNSAFE
    ASSERTDO(x >= 0 && x < width_,
             print_error("display::_apply(int, int, int, int): x(%d) out of range.\n", x);
             return);
    ASSERTDO(y >= 0 && y < height_,
             print_error("display::_apply(int, int, int, int): y(%d) out of range.\n", y);
             return);
    ASSERTDO(width >= 0 && width <= width_ * height_,
             print_error("display::_apply(int, int, int, int): width(%d) out of range.\n", width);
             return);
    ASSERTDO(height >= 0 && height <= width_ * height_,
             print_error("display::_apply(int, int, int, int): height(%d) out of range.\n", height);
             return);
#endif
    
    const int   offset_max = (x + width - 1) + (width_ * (y + height - 1));
    
    int         offset = x + (width_ * y);
    int         n_line = 0;
    
    do {
        if (_get_bit(offset)) {
            *(mem_ + offset) = *(buf_ + offset);
            
            _set_bit(offset, 0);
        }
        
        if (++n_line >= width) {
            n_line = 0;
            offset += (width_ - width);
        }
        
        ++offset;
        
    } while (offset <= offset_max);
}

void display::_flush() {
    memset(buf_, 0, sizeof(unsigned short) * width_ * height_);
    memset(bitmap_, 0, sizeof(unsigned long) * (width_ * height_ / 32));
}

int display::_get_bit(int offset) {
    int bitmap_idx = offset / 32;
    int bitmap_bit = offset % 32;
    
    return *(bitmap_ + bitmap_idx) & (1 << bitmap_bit);
}

void display::_set_bit(int offset, int value) {
    int bitmap_idx = offset / 32;
    int bitmap_bit = offset % 32;
    
    if (value) {
        *(bitmap_ + bitmap_idx) |= (1 << bitmap_bit);
    }
    else {
        *(bitmap_ + bitmap_idx) &= ~(1 << bitmap_bit);
    }
}
