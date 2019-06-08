#include "display.h"

#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

unsigned short disp_buf[DISP_HEIGHT * DISP_WIDTH] = {0, };
unsigned long bitmap[2400] = {0, };

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

static inline int _get_bit(int offset) {
	int bitmap_idx = offset / 32;
	int bitmap_bit = offset % 32;

	return bitmap[bitmap_idx] & (1 << bitmap_bit);  //test
}

unsigned short *disp_map(int fd) {
	unsigned short *mem = (unsigned short *)mmap(NULL, DISP_WIDTH * DISP_HEIGHT * PIXEL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	if (mem == MAP_FAILED) {
		perror("error while mmap()");
		exit(1);	
	}	

	return mem;
}

void disp_unmap(unsigned short *mem) {
	munmap(mem, DISP_WIDTH * DISP_HEIGHT * PIXEL_SIZE);
}


static inline long long _cur_millis() {
	struct timeval te;
	gettimeofday(&te, NULL);
	long long millis = te.tv_sec * 1000LL + te.tv_usec / 1000;

	return millis;
}

static inline void _flush_partial(unsigned short *mem, int x, int y, int width, int height) {
	int n_line = 0;

	for (register int i = x + (DISP_WIDTH * y); i < (x + width) + (DISP_WIDTH * (y + height)); ++i) {
	
		if (_get_bit(i)) {
			*(mem + i) = *(disp_buf + i);
		
			_set_bit(i, 0);
		}
	

		if (++n_line > width) {
			i += (DISP_WIDTH - width) - 1;
			n_line = 0;
		}
	}
}


//static inline void _partial_flush(unsigned short *mem, 


void _draw(unsigned short *mem, int offset,  unsigned short color) {
	*(disp_buf + offset) = color;
	_set_bit(offset, 1);
	/*
	long long cur_time = _cur_millis();

	if (cur_time - last_time > 30) {
		_flush(mem);
		last_time = cur_time;
	}
	*/
}

void disp_draw_done(unsigned short *mem) {
	_flush_partial(mem, 0, 0, DISP_WIDTH, DISP_HEIGHT);
}

void disp_partial_done(unsigned short *mem, int x, int y, int width, int height) {
	_flush_partial(mem, x, y, width, height);
}

int disp_draw_point(unsigned short *mem, int x, int y, unsigned short color) {
	int offset = (y * DISP_WIDTH) + x;
	_draw(mem, offset, color);

//	*(mem + offset) = color;

	return 0;
}

int disp_draw_rect(unsigned short *mem, int x, int y, int width, int height, unsigned short color) {
	int offset = (y * DISP_WIDTH) + x;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			_draw(mem, offset + (DISP_WIDTH * i) + j, color);
//			 *(mem + offset + (DISP_WIDTH * i) + j) =  color;
		}
	}

	return 0;
}
