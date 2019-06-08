#ifndef display_h
#define display_h

#include <stdint.h>

#define FDPATH "/dev/fb2"

#define DISP_WIDTH 320
#define DISP_HEIGHT 240
#define PIXEL_SIZE 2 /* bytes */

#define PIXEL(R, G, B) (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))

unsigned short 		*disp_map(int fd);
void 			disp_unmap(unsigned short *mem);

int 			disp_draw_point(unsigned short *mem, int x, int y, unsigned short color);
int 			disp_draw_rect(unsigned short *mem, int x, int y, int width, int height, unsigned short color);

#endif /* display_h */
