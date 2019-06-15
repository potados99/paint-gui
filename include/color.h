
#ifndef color_h
#define color_h

/**
 * unsigned int 타입에 적절한 한 픽셀을 만드는 매크로입니다.
 */
#define COLOR(R, G, B)  \
(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))

#define COLOR_BLACK     COLOR(0, 0, 0)
#define COLOR_RED       COLOR(255, 0, 0)
#define COLOR_GREEN     COLOR(0, 255, 0)
#define COLOR_BLUE      COLOR(0, 0, 255)
#define COLOR_YELLOW    COLOR(255, 255, 0)
#define COLOR_CYAN      COLOR(0, 255, 255)
#define COLOR_MAGENTA   COLOR(255, 0, 255)
#define COLOR_WHITE     COLOR(255, 255, 255)


#endif /* color_h */
