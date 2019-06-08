#include "display.h"

#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "metric.h"

unsigned short 	*disp_buf;
unsigned long 	*bitmap;

static inline int _get_bit(int offset) {
	int bitmap_idx = offset / 32;
	int bitmap_bit = offset % 32;

	return bitmap[bitmap_idx] & (1 << bitmap_bit);  //test
}

static inline void _set_bit(int offset, int val) {
	int bitmap_idx = offset / 32;
	int bitmap_bit = offset % 32;

	if (val) {
		bitmap[bitmap_idx] |= (1 << bitmap_bit);             //set
	}
	else {
		bitmap[bitmap_idx] &= ~(1 << bitmap_bit);             //unset
	}
}

static inline void _flush_partial(unsigned short *mem, int x, int y, int width, int height) {

	const int 	offset_max = (x + width - 1) + (DISP_WIDTH * (y + height - 1));

	register int 	offset = x + (DISP_WIDTH * y);
	register int 	n_line = 0;

	do {
		if (_get_bit(offset)) {
			*(mem + offset) = *(disp_buf + offset);
		
			_set_bit(offset, 0);
		}


		if (++n_line >= width) {
			n_line = 0;
			offset += (DISP_WIDTH - width);
		} 
	
		++offset;
	

	} while (offset < offset_max + 1);

}

static inline void _push(unsigned short *mem, int offset,  unsigned short color) {
	if (offset < 0 || offset >= DISP_WIDTH * DISP_HEIGHT) {
		printf("_push: offset out of range: %d\n", offset);
		return;
	}

	*(disp_buf + offset) = color;
	_set_bit(offset, 1);
}



unsigned short *disp_map(int fd) {
	unsigned short *mem = (unsigned short *)mmap(NULL, DISP_WIDTH * DISP_HEIGHT * PIXEL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	if (mem == MAP_FAILED) {
		perror("error while mmap()");
		exit(1);	
	}	

	disp_buf = (unsigned short *)malloc(sizeof(unsigned long) * DISP_WIDTH * DISP_HEIGHT + 1);
	bitmap = (unsigned long *)malloc(sizeof(unsigned long) * 2400 + 1);

	return mem;
}

void disp_unmap(unsigned short *mem) {
	munmap(mem, DISP_WIDTH * DISP_HEIGHT * PIXEL_SIZE);

	free(disp_buf);
	free(bitmap);
}
void disp_draw_done(unsigned short *mem) {
	_flush_partial(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT);
}

void disp_partial_done(unsigned short *mem, int x, int y, int width, int height) {
	_flush_partial(mem, x, y, width, height);
}

int disp_draw_point(unsigned short *mem, int x, int y, unsigned short color) {
	_push(mem, x + (y * DISP_WIDTH), color);
	
	return 0;
}


static inline int _line_low(unsigned short *mem, int x0, int y0, int x1, int y1, unsigned short color) {
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
		_push(mem, x + (DISP_WIDTH * y), color);
		
		if (D > 0) {
			y += yi;
			D -= 2*dx;
		}

		D += 2*dy;
	}

	return 0;
}

static inline int _line_high(unsigned short *mem, int x0, int y0, int x1, int y1, unsigned short color) {
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
		_push(mem, x + (DISP_WIDTH * y), color);
		
		if (D > 0) {
			x += xi;
			D -= 2*dy;
		}

		D += 2*dx;
	}

	return 0;
}

int disp_draw_line(unsigned short *mem, int x0, int y0, int x1, int y1, unsigned short color) {
	if (abs(y1 - y0) < abs(x1 - x0)) {
		if (x0 > x1) {
			return _line_low(mem, x1, y1, x0, y0, color);
		}
		else {
			return _line_low(mem, x0, y0, x1, y1, color);
		}
	}
	else {
		if (y0 > y1) {
			return _line_high(mem, x1, y1, x0, y0, color);
		}
		else {
			return _line_high(mem, x0, y0, x1, y1, color);
		}
	}
	
}

int disp_draw_rect(unsigned short *mem, int x, int y, int width, int height, unsigned short color) {

	const int 	offset_max = (x + width - 1) + (DISP_WIDTH * (y + height - 1));
	register int 	offset = x + (DISP_WIDTH * y);
	
	register int 	n_line = 0;

	do {
		_push(mem, offset, color);
	
		if (++n_line >= width) {
			n_line = 0;
			offset += (DISP_WIDTH - width);
		} 
		
		++offset;
		
	} while (offset < offset_max + 1);

	return 0;
}

int to_point_and_size(int x0, int y0, int x1, int y1, struct point *point, struct size *size) {
	int temp;

	if (x0 > x1) {
		temp = x0;
		x0 = x1;
		x1 = temp;
	}
	if (y0 > y1) {
		temp = y0;
		y0 = y1;
		y1 = temp;
	}

	point->x = x0;
	point->y = y0;

	size->width = x1 - x0 + 1;
	size->height = y1 - y0 + 1;

	return 0;
}
