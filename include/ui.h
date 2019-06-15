
#ifndef ui_h
#define ui_h

#include "machine_specific.h"
#include "metric.h"


#define UI_PALETTE_ITEM_SIZE       	SIZE(23, 23)

#define UI_PALETTE_C0_LOCATION      POINT(264, 16)
#define UI_PALETTE_C1_LOCATION      POINT(291, 16)
#define UI_PALETTE_C2_LOCATION      POINT(264, 44)
#define UI_PALETTE_C3_LOCATION      POINT(291, 44)
#define UI_PALETTE_C4_LOCATION      POINT(264, 72)
#define UI_PALETTE_C5_LOCATION      POINT(291, 72)
#define UI_PALETTE_C6_LOCATION      POINT(264, 100)
#define UI_PALETTE_C7_LOCATION      POINT(291, 100)

#define UI_CANVAS_LOCATION			POINT(65, 4)
#define UI_CANVAS_SIZE				SIZE(192, 232)

extern unsigned char UI_IMAGE[DP_WIDTH * DP_HEIGHT];

#endif /* ui_h */
