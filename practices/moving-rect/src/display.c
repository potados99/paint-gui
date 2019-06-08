#include "display.h"

#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int disp_buf[DISP_HEIGHT * DISP_WIDTH] = {-1, };
long long last_time = 0;

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

static inline void _flush(unsigned short *mem) {

}

void _draw(unsigned short *mem, int offset,  unsigned short color) {
	*(disp_buf + offset) = color;


	long long cur_time = _cur_millis();

	if (cur_time - last_time > 30) {
		for (int i = 0; i < DISP_HEIGHT * DISP_WIDTH; ++i) {
			if (*(disp_buf + i) != -1) {
				*(mem + i) = *(disp_buf + i);
				*(disp_buf + i) = -1;
			}
		}

		last_time = cur_time;
	}
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
		}
	}

	return 0;
}
