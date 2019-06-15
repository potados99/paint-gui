
#ifndef ui_h
#define ui_h

#include "machine_specific.h"
#include "metric.h"
#include "color.h"

/**
 * UI 요소들의 위치와 크기, 색상입니다.
 */

#define UI_BUTTON_LINE_LOCATION     POINT(6, 16)
#define UI_BUTTON_RECT_LOCATION     POINT(6, 46)
#define UI_BUTTON_OVAL_LOCATION     POINT(6, 76)
#define UI_BUTTON_FDRAW_LOCATION    POINT(6, 106)
#define UI_BUTTON_SELECT_LOCATION   POINT(6, 136)
#define UI_BUTTON_ERASE_LOCATION    POINT(6, 166)
#define UI_BUTTON_CLEAR_LOCATION    POINT(6, 196)
#define UI_BUTTON_PEN_LOCATION      POINT(270, 167)
#define UI_BUTTON_FILL_LOCATION     POINT(270, 196)

#define UI_BUTTON_ITEM_SIZE_BIG     SIZE(53, 26)
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


extern unsigned char UI_IMAGE[DP_WIDTH * DP_HEIGHT];

#endif /* ui_h */
