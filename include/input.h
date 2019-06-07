#ifndef input_h
#define input_h

#define TS_SIZE 		4096

#define STATE_NONE		0x00
#define STATE_TOUCH_DOWN	0x01
#define STATE_TOUCH_UP		0x02

class touch_event {
public:
	int x = 0;
	int y = 0;
	int pressure = 0;
	int touch_state = 0;
};

class touch_correction {
public:
	int xd_coef_x = 0; /* A */
	int xd_coef_y = 0; /* B */
	int xd_coef_1 = 0; /* C */

	int yd_coef_x = 0; /* D */
	int yd_coef_y = 0; /* E */
	int yd_coef_1 = 0; /* F */
};

class input {
private:
	int 			fd_;
	touch_correction 	correction_;

	int touch_read(int fd, touch_event *event, touch_correction *correction);
public:
	input(const char *event_fd_path);

	void start_loop();
};

#endif /* input_h */
