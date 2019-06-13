/**
 * touch.h
 * 터치 입력에 관한 함수들을 모아놓았습니다.
 *
 */

#ifndef touch_h
#define touch_h

/**
 * 터치 상태입니다.
 * 막 터치가 시작된 STATE_TOUCH_DOWN, 그대로 유지되는 STATE_NONE,
 * 그리고 터치가 끝난 STATE_TOUCH_UP 이렇게 3개입니당.
 */
#define TOUCH_STATE_BEGIN   1
#define TOUCH_STATE_DRAG    0
#define TOUCH_STATE_DONE    2

#define TOUCH_EVENT(NAME)   \
struct touch_event NAME = {0}

/**
 * 터치를 읽은 결과를 저장하는 구조체입니다.
 */
struct touch_event {
	unsigned int    x;                      /* [0, 320] */
	unsigned int    y;                      /* [0, 240] */
	unsigned char   pressure;               /* [0, 255] */
	unsigned char   touch_state;            /* [0, 2] */    
};

/**
 * 터치 입력을 읽어서 touch_event 구조체에 값을 저장합니다.
 * 여기서는 blocking read에 맞게 설계되었습니다.
 */
int touch_read(int fd, struct touch_event *event);


#endif /* touch_h */
