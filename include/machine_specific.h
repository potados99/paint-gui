#ifndef machine_specific_h
#define machine_specific_h

#define TS_FD_PATH      "/dev/input/event1"
#define DP_FD_PATH      "/dev/fb2"

#define TS_OPEN_OPTION  O_RDONLY
#define DP_OPEN_OPTION  O_RDWR

#define DP_WIDTH        320
#define DP_HEIGHT       240

#define PIXEL_SIZE      2

#define DP_MEM_SIZE		(DP_WIDTH * DP_HEIGHT)
#define DP_MEM_SIZEB	(DP_MEM_SIZE * PIXEL_SIZE)
#define DP_BITMAP_SIZE	(DP_MEM_SIZE / 32)
#define DP_BITMAP_SIZEB	(DP_BITMAP_SIZE * 4)

#define TS_WDITH        4096 /* 2^12 */
#define TS_HEIGHT       4096

#define TS_X_MIN        310
#define TS_X_MAX        3900
#define TS_Y_MIN        150
#define TS_Y_MAX        3900

#endif /* machine_specific_h */
