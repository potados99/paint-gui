#ifndef machine_specific_h
#define machine_specific_h

#define TS_FD_PATH      "/dev/input/event1"
#define DP_FD_PATH      "/dev/fb2"

#define TS_OPEN_OPTION  O_RDONLY
#define DP_OPEN_OPTION  O_RDWR

#define DP_WIDTH        320
#define DP_HEIGHT       240

#define PIXEL_SIZE      2
#define BITMAP_SIZE     2400
#define DP_MEM_SIZE     153600

#define TS_WDITH        4096 /* 2^12 */
#define TS_HEIGHT       4096

#define TS_X_MIN        310
#define TS_X_MAX        3900
#define TS_Y_MIN        150
#define TS_Y_MAX        3900

#endif /* machine_specific_h */
