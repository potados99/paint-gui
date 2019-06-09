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

static unsigned short 	*disp_buf;
static unsigned long 	*bitmap;

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
((*(PTR + (offset / 32))) & (1 << (offset % 32)))

#define SET_BIT(PTR, OFFSET)                        \
do {                                                \
*(PTR + (offset / 32)) |= (1 << (offset % 32));     \
} while (0)

#define UNSET_BIT(PTR, OFFSET)                      \
do {                                                \
*(PTR + (offset / 32)) &= ~(1 << (offset % 32));    \
} while (0)

static inline void _apply(unsigned short *mem, int point, int size) {
#ifndef UNSAFE
    ASSERTDO(mem != NULL, print_error("_apply: mem cannot be null.\n"); return);
    POINT_CHECK("_apply", point);
    SIZE_CHECK("_apply", size);
#endif
    
    short x, y, width, height;
    
    x = X(point);
    y = Y(point);
    width = WIDTH(size);
    height = WIDTH(size);
    
	const short 	offset_max = (x + width - 1) + (DP_WIDTH * (y + height - 1));

	register short 	offset = x + (DP_WIDTH * y);
	register short 	n_line = 0;

	do {
		if (GET_BIT(bitmap, offset)) {
			*(mem + offset) = *(disp_buf + offset);
		
            UNSET_BIT(bitmap, offset);
		}


		if (++n_line >= width) {
			n_line = 0;
			offset += (DP_WIDTH - width);
		} 
	
		++offset;
	

	} while (offset <= offset_max);

}

static inline void _modify(unsigned short *mem, int offset,  unsigned short color) {
#ifndef UNSAFE
    ASSERTDO(mem != NULL, print_error("_modify: mem cannot be null.\n"); return);
    ASSERTDO(IN_RANGE(offset, 0, DP_WIDTH * DP_HEIGHT - 1), print_error("_modify: offset out of range.\n"); return);
#endif

	*(disp_buf + offset) = color;
    SET_BIT(disp_buf, offset);
}

static inline int _line_low(unsigned short *mem, int p0, int p1, unsigned short color) {
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
        _modify(mem, x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            y += yi;
            D -= 2*dx;
        }
        
        D += 2*dy;
    }
    
    return 0;
}

static inline int _line_high(unsigned short *mem, int p0, int p1, unsigned short color) {
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
        _modify(mem, x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            x += xi;
            D -= 2*dy;
        }
        
        D += 2*dx;
    }
    
    return 0;
}

////////////////////////// VERY VERY PERFORMANCE SENSITIVE //////////////////////////


unsigned short *disp_map(int fd) {
	unsigned short *mem = (unsigned short *)mmap(NULL, DP_WIDTH * DP_HEIGHT * PIXEL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERTDO(mem != MAP_FAILED, print_error("disp_map(): mmap() failed.\n"); return NULL);

	disp_buf = (unsigned short *)malloc(sizeof(unsigned long) * DP_WIDTH * DP_HEIGHT + 1);
    ASSERTDO(disp_buf != NULL, print_error("disp_map(): malloc() failed.\n"); return NULL);

	bitmap = (unsigned long *)malloc(sizeof(unsigned long) * BITMAP_SIZE + 1);
    ASSERTDO(bitmap != NULL, print_error("disp_map(): malloc() failed.\n"); return NULL);

	return mem;
}

void disp_unmap(unsigned short *mem) {
	munmap(mem, DP_WIDTH * DP_HEIGHT * PIXEL_SIZE);

	free(disp_buf);
	free(bitmap);
}

int disp_draw_point(unsigned short *mem, int point, unsigned short color) {
	_modify(mem, X(point) + (Y(point) * DP_WIDTH), color);
	
	return 0;
}

int disp_draw_line(unsigned short *mem, int p0, int p1, unsigned short color) {

	if (DELTA_HEIGHT(p0, p1) < DELTA_WIDTH(p0, p1)) {
		if (X(p0) > X(p1)) {
			return _line_low(mem, p1, p0, color);
		}
		else {
			return _line_low(mem, p0, p1, color);
		}
	}
	else {
		if (Y(p0) > Y(p1)) {
			return _line_high(mem, p1, p0, color);
		}
		else {
			return _line_high(mem, p0, p1, color);
		}
	}
	
}

int disp_draw_rect(unsigned short *mem, int point, int size, unsigned short color) {
    short x, y, width, height;
    
    x = X(point);
    y = Y(point);
    
    width = WIDTH(size);
    height = WIDTH(size);
    
	const int 	    offset_max = (x + width - 1) + (DP_WIDTH * (y + height - 1));
	register int 	offset = x + (DP_WIDTH * y);
	
	register int 	n_line = 0;

	do {
		_modify(mem, offset, color);
	
		if (++n_line >= width) {
			n_line = 0;
			offset += (DP_WIDTH - width);
		} 
		
		++offset;
		
	} while (offset < offset_max + 1);

	return 0;
}

int disp_draw_whole(unsigned short *mem, unsigned short color) {
    return disp_draw_rect(mem, 0, SIZE(DP_WIDTH, DP_HEIGHT), color);
}

void disp_commit(unsigned short *mem) {
    _apply(mem, 0, SIZE(DP_WIDTH, DP_HEIGHT));
}

void disp_commit_partial(unsigned short *mem, int point, int size) {
    _apply(mem, point, size);
}

void disp_clear(unsigned short *mem) {
    memset(mem, 0, DP_MEM_SIZE);
}
