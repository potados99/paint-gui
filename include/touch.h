#ifndef input_h
#define input_h

enum touch_state {
    STATE_NONE          = 0x00,
    STATE_TOUCH_DOWN    = 0x01,
    STATE_TOUCH_UP      = 0x02
};

class touch_event {
public:
	int             x = 0;
	int             y = 0;
    
	int             pressure = 0;
    touch_state     state = touch_state::STATE_NONE;
};

/**
 * Immutable.
 */
class touch_correction {
private:
    int x_min_;
    int x_max_;
    
    int y_min_;
    int y_max_;
    
public:
    touch_correction(int x_min, int x_max, int y_min, int y_max):
    x_min_(x_min),
    x_max_(x_max),
    y_min_(y_min),
    y_max_(y_max) {}
    
    int x_min() { return x_min_; }
    int x_max() { return x_max_; }

    int y_min() { return y_min_; }
    int y_max() { return y_max_; }
};

class touch {
private:
	int 			    fd_;
	touch_correction 	correction_;

    int _touch_read(int fd, touch_event *event, touch_correction *correction);

public:
	touch(const char *event_fd_path);
    ~touch();
    
    int touch_read(touch_event *event);
};

#endif /* input_h */
