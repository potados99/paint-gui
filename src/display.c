#include "display.h"
#include "metric.h"
#include "machine_specific.h"
#include "debug.h"

#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

static unsigned short   *dp_mem;
//static unsigned short 	*dp_buf;
//static unsigned long 	*bitmap;

static unsigned short 	dp_buf[DP_MEM_SIZE];
static unsigned long 	bitmap[DP_BITMAP_SIZE];

static bool             direct;

/**
 * Assertion macros.
 * Local use.
 */
#define POINT_CHECK(FUNC_NAME, POINT_NAME)                                                  \
do {                                                                                        \
ASSERTDO(IN_RANGE(X(POINT_NAME), 0, DP_WIDTH - 1),                                          \
print_error(FUNC_NAME ": X(" #POINT_NAME "){%d} out of range.\n", X(POINT_NAME));           \
return);                                                                                    \
ASSERTDO(IN_RANGE(Y(POINT_NAME), 0, DP_HEIGHT - 1),                                         \
print_error(FUNC_NAME ": Y(" #POINT_NAME "){%d} out of range.\n", Y(POINT_NAME));           \
         return);                                                                           \
} while (0)

#define SIZE_CHECK(FUNC_NAME, SIZE_NAME)                                                    \
do {                                                                                        \
ASSERTDO(IN_RANGE(WIDTH(SIZE_NAME), 0, DP_WIDTH),                                           \
print_error(FUNC_NAME ": WIDTH(" #SIZE_NAME "){%d} out of range.\n", WIDTH(SIZE_NAME));     \
return);                                                                                    \
ASSERTDO(IN_RANGE(HEIGHT(SIZE_NAME), 0, DP_HEIGHT),                                         \
print_error(FUNC_NAME ": HEIGHT(" #SIZE_NAME "){%d} out of range.\n", HEIGHT(SIZE_NAME));   \
return);                                                                                    \
} while (0)



////////////////////////// VERY VERY PERFORMANCE SENSITIVE //////////////////////////

#define GET_BIT(PTR, OFFSET)                        \
((*(PTR + (OFFSET / 32))) & (1 << (OFFSET % 32)))

#define SET_BIT(PTR, OFFSET)                        \
do {                                                \
*(PTR + (OFFSET / 32)) |= (1 << (OFFSET % 32));     \
} while (0)

#define UNSET_BIT(PTR, OFFSET)                      \
do {                                                \
*(PTR + (OFFSET / 32)) &= ~(1 << (OFFSET % 32));    \
} while (0)

static inline void _apply(int point, int size) {
#ifndef UNSAFE
    ASSERTDO(dp_mem != NULL, print_error("_apply: mem cannot be null.\n"); return);
    POINT_CHECK("_apply", point);
    SIZE_CHECK("_apply", size);
#endif
  
    short x, y, width, height;
    
    x = X(point);
    y = Y(point);
    width = WIDTH(size);
    height = HEIGHT(size);
	
	print_trace("apply changes at x: %d, y: %d, width: %d, height: %d.\n", x, y, width, height);
    
	const int	 	offset_max = (x + width - 1) + (DP_WIDTH * (y + height - 1));
	register int 	offset = x + (DP_WIDTH * y);
	register short 	n_line = 0;

	do {
		if (GET_BIT(bitmap, offset)) {

			print_trace("write to display memory at offset{%d max} %d.\n", offset_max, offset);

			*(dp_mem + offset) = *(dp_buf + offset);
		
            UNSET_BIT(bitmap, offset);
		}


		if (++n_line >= width) {
			n_line = 0;
			offset += (DP_WIDTH - width);
		} 
	
		++offset;
	

	} while (offset <= offset_max);

}

static inline void _modify(int offset,  unsigned short color) {
#ifndef UNSAFE
    ASSERTDO(dp_mem != NULL, print_error("_modify: mem cannot be null.\n"); return);
    ASSERTDO(IN_RANGE(offset, 0, DP_WIDTH * DP_HEIGHT - 1), print_error("_modify: offset{%d} out of range.\n", offset); return);
#endif

	print_trace("modify pixel at offset %d to %d.\n", offset, color);

    if (direct) {
        *(dp_mem + offset) = color;
    }
    else {
        *(dp_buf + offset) = color;
        SET_BIT(bitmap, offset);
    }
}

static inline void _line_low(int p0, int p1, unsigned short color) {
    short x0, y0, x1, y1;
    
    x0 = X(p0);
    y0 = Y(p0);
    x1 = X(p1);
    y1 = Y(p1);
    
    short dx = x1 - x0;
    short dy = y1 - y0;
    short yi = 1;
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    
    short D = 2*dy - dx;
    short y = y0;
    
    for (short x = x0; x <= x1; ++x) {
        _modify(x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            y += yi;
            D -= 2*dx;
        }
        
        D += 2*dy;
    }
}

static inline void _line_high(int p0, int p1, unsigned short color) {
    short x0, y0, x1, y1;
    
    x0 = X(p0);
    y0 = Y(p0);
    x1 = X(p1);
    y1 = Y(p1);
    
    short dx = x1 - x0;
    short dy = y1 - y0;
    short xi = 1;
    
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    
    short D = 2*dx - dy;
    short x = x0;
    
    for (short y = y0; y <= y1; ++y) {
        _modify(x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            x += xi;
            D -= 2*dy;
        }
        
        D += 2*dx;
    }
}

////////////////////////// VERY VERY PERFORMANCE SENSITIVE //////////////////////////


void disp_map(int fd) {
	dp_mem = (unsigned short *)mmap(NULL, DP_MEM_SIZEB, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERTDO(dp_mem != MAP_FAILED, print_error("disp_map(): mmap() failed.\n"); return);

	//dp_buf = (unsigned short *)malloc(sizeof(unsigned long) * DP_WIDTH * DP_HEIGHT + 1);
    //ASSERTDO(dp_buf != NULL, print_error("disp_map(): malloc() failed.\n"); return);

	//bitmap = (unsigned long *)malloc(sizeof(unsigned long) * BITMAP_SIZE + 1);
    //ASSERTDO(bitmap != NULL, print_error("disp_map(): malloc() failed.\n"); return);
}

void disp_unmap() {
	munmap(dp_mem, DP_WIDTH * DP_HEIGHT * PIXEL_SIZE);

	//free(dp_buf);
	//free(bitmap);
}

void disp_set_direct(bool value) {
    direct = value;
}

void disp_draw_point(int point, unsigned short color) {
	_modify(X(point) + (Y(point) * DP_WIDTH), color);
}

void disp_draw_line(int p0, int p1, unsigned short color) {

	if (DELTA_HEIGHT(p0, p1) < DELTA_WIDTH(p0, p1)) {
		if (X(p0) > X(p1)) {
            _line_low(p1, p0, color);
		}
		else {
            _line_low(p0, p1, color);
		}
	}
	else {
		if (Y(p0) > Y(p1)) {
            _line_high(p1, p0, color);
		}
		else {
            _line_high(p0, p1, color);
		}
	}
}

void disp_draw_rect(int point, int size, unsigned short color) {
    short x, y, width, height;
    
    x = X(point);
    y = Y(point);
    
    width = WIDTH(size);
    height = HEIGHT(size);
 
	print_trace("draw rect at x: %d, y: %d, width: %d, height: %d.\n", x, y, width, height);
   
	const int 	    offset_max = (x + width - 1) + (DP_WIDTH * (y + height - 1));
	register int 	offset = x + (DP_WIDTH * y);
	register short 	n_line = 0;

	do {
		_modify(offset, color);
	
		if (++n_line >= width) {
			n_line = 0;
			offset += (DP_WIDTH - width);
		} 
		
		++offset;
		
	} while (offset < offset_max + 1);
}

void disp_draw_whole(unsigned short color) {
    return disp_draw_rect(0, SIZE(DP_WIDTH, DP_HEIGHT), color);
}

void disp_commit() {
    _apply(0, SIZE(DP_WIDTH, DP_HEIGHT));
}

void disp_commit_partial(int point, int size) {
    _apply(point, size);
}

void disp_cancel() {
	memset(dp_buf, 0, DP_BITMAP_SIZEB);
}

void disp_clear() {
    memset(dp_mem, 0, DP_MEM_SIZEB);
}
