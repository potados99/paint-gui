/**
 * ui.h
 * 프로그램에서 사용할 ui 요소들에 대한 정보를 모아두었습니다.
 */

#ifndef ui_h
#define ui_h

#include "machine_specific.h"
#include "metric.h"
#include "color.h"

/**
 * UI 요소(버튼으로 쓰임)들의 위치와 크기, 색상입니다.
 */
#define UI_BUTTON_LINE_LOCATION     POINT(5, 15)
#define UI_BUTTON_RECT_LOCATION     POINT(5, 45)
#define UI_BUTTON_OVAL_LOCATION     POINT(5, 75)
#define UI_BUTTON_FDRAW_LOCATION    POINT(5, 105)
#define UI_BUTTON_SELECT_LOCATION   POINT(5, 135)
#define UI_BUTTON_ERASE_LOCATION    POINT(5, 165)
#define UI_BUTTON_CLEAR_LOCATION    POINT(5, 195)
#define UI_BUTTON_PEN_LOCATION      POINT(270, 167)
#define UI_BUTTON_FILL_LOCATION     POINT(270, 196)

#define UI_BUTTON_ITEM_SIZE_BIG     SIZE(54, 27)
#define UI_BUTTON_ITEM_SIZE_SMALL   SIZE(38, 26)

#define UI_PALETTE_C0_LOCATION      POINT(264, 16)
#define UI_PALETTE_C1_LOCATION      POINT(291, 16)
#define UI_PALETTE_C2_LOCATION      POINT(264, 44)
#define UI_PALETTE_C3_LOCATION      POINT(291, 44)
#define UI_PALETTE_C4_LOCATION      POINT(264, 72)
#define UI_PALETTE_C5_LOCATION      POINT(291, 72)
#define UI_PALETTE_C6_LOCATION      POINT(264, 100)
#define UI_PALETTE_C7_LOCATION      POINT(291, 100)

#define UI_PALETTE_ITEM_SIZE        SIZE(23, 23)

#define UI_PALETTE_C0_COLOR         COLOR_WHITE
#define UI_PALETTE_C1_COLOR         COLOR_MAGENTA
#define UI_PALETTE_C2_COLOR         COLOR_RED
#define UI_PALETTE_C3_COLOR         COLOR_GREEN
#define UI_PALETTE_C4_COLOR         COLOR_YELLOW
#define UI_PALETTE_C5_COLOR         COLOR_BLUE
#define UI_PALETTE_C6_COLOR         COLOR_CYAN
#define UI_PALETTE_C7_COLOR         COLOR_BLACK

#define UI_CANVAS_LOCATION			POINT(65, 4)
#define UI_CANVAS_SIZE				SIZE(192, 232)

/**
 * 버튼 식별자입니다.
 */
#define UI_GROUP(ID)                ((ID) >> 4)

#define UI_BTN_NONE                 0x00

#define UI_BTN_LINE                 0x01
#define UI_BTN_RECT                 0x02
#define UI_BTN_OVAL                 0x03
#define UI_BTN_FDRAW                0x04
#define UI_BTN_SELECT               0x05
#define UI_BTN_ERASE                0x06
#define UI_BTN_CLEAR                0x07
#define UI_BTN_PEN                  0x18
#define UI_BTN_FILL                 0x19

#define UI_BTN_C0                   0x21
#define UI_BTN_C1                   0x22
#define UI_BTN_C2                   0x23
#define UI_BTN_C3                   0x24
#define UI_BTN_C4                   0x25
#define UI_BTN_C5                   0x26
#define UI_BTN_C6                   0x27
#define UI_BTN_C7                   0x28

/**
 * 버튼의 타입을 이루는 요소들입니다.
 * 버튼의 타입은 마크 가능 여부와 색상 보유 여부를 나타냅니다.
 * 예시: type이 BT_MARKABLE | BT_COLORABLE이면 해당 버튼은
 * 선택되었음으로 마크할 수도 있고 색상을 가져 색칠할 수 있습니다.
 */
#define BT_MARKABLE                 0x01
#define BT_COLORABLE                0x02

#define UI_NUMBER_OF_BUTTONS        17

struct button {
    int             type;
    
    int             x;
    int             y;
    
    int             width;
    int             height;
    
    unsigned short  color;
    
    int             id;
};

/**
 * ui 비트맵 이미지입니다.
 */
extern const unsigned char ui_image[DP_WIDTH * DP_HEIGHT];

/**
 * 프로그램에서 사용할 버튼들의 컬렉션입니다.
 */
extern const struct button ui_buttons[UI_NUMBER_OF_BUTTONS];

/**
 * 주어진 값(좌표 또는 id)으로 버튼을 찾습니다. 없으면 NULL!
 */
const struct button *ui_find_button_by_coordinate(int x, int y);
const struct button *ui_find_button_by_id(int id);

#endif /* ui_h */

