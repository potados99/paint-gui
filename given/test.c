#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h> /* for ioctl */
#include <sys/mman.h> /* for mmap */
#include <sys/types.h>
#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include <linux/input.h>

#define FBDEVFILE "/dev/fb2"

float a, b, c, d, e, f, k;
unsigned short *pfbdata;
int rpixel;
int offset;
int fd, fbfd;
struct fb_var_screeninfo fbvar;
struct input_event ie;
typedef unsigned char ubyte;

unsigned short makepixel(ubyte r, ubyte g, ubyte b) {
	return (unsigned short)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

typedef struct _Point {
	int x;
	int y;
}Point;

void setCalibration() {
	int i, j, tt, pressure;
	int x[3], y[3], xd[3] = { 50, 150, 300 }, yd[3] = { 100, 50, 200 };
	unsigned short red = makepixel(255, 0, 0);
	for (tt = 0; tt < 3; tt++) {
		for (i = -5; i < 5; i++) {
			offset = (yd[tt] + i) * fbvar.xres + xd[tt];
			*(pfbdata + offset) = red;
			offset = yd[tt] * fbvar.xres + xd[tt] + i;
			*(pfbdata + offset) = red;
		}
		while (1) {
			read(fd, &ie, sizeof(struct input_event));
			if (ie.type == 3) {
				if (ie.code == 0) x[tt] = ie.value;
				else if (ie.code == 1) y[tt] = ie.value;
				else if (ie.code == 24) pressure = ie.value;
				if (pressure == 0)break;
			}
		}
		//printf("%d %d\n%d %d\n", xd[tt], yd[tt], x[tt], y[tt]);
		pressure = -1;
	}
	k = ((x[0] - x[2])*(y[1] - y[2])) - ((x[1] - x[2])*(y[0] - y[2]));
	a = ((xd[0] - xd[2])*(y[1] - y[2])) - ((xd[1] - xd[2])*(y[0] - y[2]));
	b = ((x[0] - x[2])*(xd[1] - xd[2])) - ((xd[0] - xd[2])*(x[1] - x[2]));
	c = (y[0] * ((x[2] * xd[1]) - (x[1] * xd[2]))) + (y[1] * ((x[0] * xd[2]) - (x[2] * xd[0]))) + (y[2] * ((x[1] * xd[0]) - (x[0] * xd[1])));
	d = ((yd[0] - yd[2])*(y[1] - y[2])) - ((yd[1] - yd[2])*(y[0] - y[2]));
	e = ((x[0] - x[2])*(yd[1] - yd[2])) - ((yd[0] - yd[2])*(x[1] - x[2]));
	f = (y[0] * ((x[2] * yd[1]) - (x[1] * yd[2]))) + (y[1] * ((x[0] * yd[2]) - (x[2] * yd[0]))) + (y[2] * ((x[1] * yd[0]) - (x[0] * yd[1])));
	a = a / k; b = b / k; c = c / k;
	d = d / k; e = e / k; f = f / k;
}
void clearLcd() {
	int i, j;
	for (i = 0; i < 240; i++) {
		for (j = 0; j < 320; j++) {
			offset = i * 320 + j;
			*(pfbdata + offset) = 0;
		}
	}
}
void clearObjec(Point start, Point end) {
	int tmp, i, j;
	if (start.x > end.x) {
		tmp = start.x;
		start.x = end.x;
		end.x = tmp;
	}
	if (start.y > end.y) {
		tmp = start.y;
		start.y = end.y;
		end.y = tmp;
	}
	for (i = start.y; i <= end.y; i++) {
		for (j = start.x; j <= end.x; j++) {
			offset = i * 320 + j;
			*(pfbdata + offset) = 0;
		}
	}
}
void makeLineBox(Point start, Point end, unsigned short color){
	int i, j, tmp;
	if (start.x > end.x) {
		tmp = start.x;
		start.x = end.x;
		end.x = tmp;
	}
	if (start.y > end.y) {
		tmp = start.y;
		start.y = end.y;
		end.y = tmp;
	}
	for (j = start.x; j < end.x; j++) {
		offset = start.y * 320 + j;
		*(pfbdata + offset) = color;
		offset = end.y * 320 + j;
		*(pfbdata + offset) = color;
	}
	for (i = start.y; i < end.y; i++) {
		offset = i * 320 + start.x;
		*(pfbdata + offset) = color;
		offset = i * 320 + end.x;
		*(pfbdata + offset) = color;
	}
}
void makeFillBox(Point start, Point end, unsigned short color) {
	int i, j, tmp;
	if (start.x > end.x) {
		tmp = start.x;
		start.x = end.x;
		end.x = tmp;
	}
	if (start.y > end.y) {
		tmp = start.y;
		start.y = end.y;
		end.y = tmp;
	}
	for (i = start.y; i < end.y; i++) {
		for (j = start.x; j < end.x; j++) {
			offset = i * 320 + j;
			*(pfbdata + offset) = color;
		}
	}
}

//'makeLine' doesn't working. It has some problems.
void makeLine(Point start, Point end, unsigned short color) {
	int i, j, tmp;
	float A = 0, B;
	int C;
	if(end.x!=start.x) A = (end.y - start.y) / (end.x - start.x);
	B = start.y- A * start.x;
	if (start.x > end.x) {
		tmp = start.x;
		start.x = end.x;
		end.x = tmp;
	}
	if (start.y > end.y) {
		tmp = start.y;
		start.y = end.y;
		end.y = tmp;
	}
	for (i = start.y; i < end.y; i++) {
		for (j = start.x; j < end.x; j++) {
			C = A * j + B;
			if (i = C) {
				offset = i * 320 + j;
				*(pfbdata + offset) = color;
			}
		}
	}
}
int main(void) {
	int pressure = -1;
	int ret, t, i, j;


	int x1, x2, y1, y2;
	fbfd = open(FBDEVFILE, O_RDWR);

	if (fbfd < 0) {
		perror("fbdev open");
		exit(1);
	}
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	if (ret < 0) {
		perror("fbdev ioctl");
		exit(1);
	}
	if (fbvar.bits_per_pixel != 16) {
		fprintf(stderr, "bpp is not 16\n");
		exit(1);
	}

	pfbdata = (unsigned short *)mmap(0, fbvar.xres*fbvar.yres * 16 / 8, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((unsigned)pfbdata == (unsigned)-1) {
		perror("fbdev mmap");
		exit(1);
	}

	fd = open("/dev/input/event4", O_RDONLY);
	if (fd < 0) return -1;

	printf("start\n");
	clearLcd();
	setCalibration();

	clearLcd();


	Point get, start, end;
	int ptmp = -1;
	unsigned short red = makepixel(255, 0, 0);
	while (1) {
		read(fd, &ie, sizeof(struct input_event));
		if (ie.type == 3) {
			if (ie.code == 0) {
				get.x = ie.value;
			}
			else if (ie.code == 1) {
				get.y = ie.value;
			}
			else if (ie.code == 24) {
				pressure = ie.value;
				if (ptmp == -1) {
					//printf("start\n");
					start.x = a*get.x+b*get.y+c;
					start.y = d*get.x+e*get.y+f;
					end.x = start.x;
					end.y = start.y;
					ptmp = pressure;
				}
				if (pressure == 0) { 
					ptmp = -1;
					//printf("end!\n");
					//printf("(%d, %d)->(%d, %d)\n", start.x, start.y, end.x, end.y);
				}
				else {
					clearObjec(start, end);
					end.x = a * get.x + b * get.y + c;
					end.y = d * get.x + e * get.y + f;
					makeLineBox(start, end, red);
					//printf("(%d, %d)\n", end.x, end.y);
				}
			}
		}
	}

	close(fd);
	close(fbfd);
	return 0;

} // end of main

