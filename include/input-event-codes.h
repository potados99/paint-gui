#ifndef input_event_codes_h
#define input_event_codes_h

/**
 * 이게 없을 수도 있더라구요..
 */

#define EV_SYN                  0x00
#define EV_KEY                  0x01
#define EV_REL                  0x02
#define EV_ABS                  0x03
#define EV_MSC                  0x04
#define EV_SW                   0x05
#define EV_LED                  0x11
#define EV_SND                  0x12
#define EV_REP                  0x14
#define EV_FF                   0x15
#define EV_PWR                  0x16
#define EV_FF_STATUS            0x17
#define EV_MAX                  0x1f
#define EV_CNT               	(EV_MAX+1)

#define BTN_TOUCH               0x14

#define ABS_X                   0x00
#define ABS_Y                   0x01
#define ABS_PRESSURE            0x18

#endif /* input_event_codes_h */
