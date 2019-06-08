#include "display.h"

#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

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

int disp_draw_point(unsigned short *mem, int x, int y, unsigned short color) {
	int offset = (y * DISP_WIDTH) + x;
	*(mem + offset) = color;

	return 0;
}

int disp_draw_rect(unsigned short *mem, int x, int y, int width, int height, unsigned short color) {
	int offset = (y * DISP_WIDTH) + x;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			*(mem + offset + (DISP_WIDTH * i) + j) = color;
		}
	}

	return 0;
}
