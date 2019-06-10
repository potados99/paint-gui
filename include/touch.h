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
#define STATE_NONE 		    0x00
#define STATE_TOUCH_DOWN 	0x01
#define STATE_TOUCH_UP 		0x02

/**
 * 터치를 읽은 결과를 저장하는 구조체입니다.
 * 이는 또한 이전 터치 결과를 조금 저장하여 불연속적인 입력을 무시하는 필터링 기능도 합니다.
 */
struct touch_event {
	unsigned short x;                   /* [0, 320] */
	unsigned short y;                   /* [0, 240] */
	unsigned char pressure;             /* [0, 255] */
	unsigned char touch_state;          /* [0, 2] */
    
    unsigned short last_distance;       /* [0, 570] */
};

/**
 * 터치 입력을 읽어서 touch_event 구조체에 값을 저장합니다.
 * 여기서는 blocking read에 맞게 설계되었습니다.
 */
int touch_read(int fd, struct touch_event *event);


#endif /* touch_h */
