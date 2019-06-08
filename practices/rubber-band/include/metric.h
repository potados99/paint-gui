#ifndef metric_h
#define metric_h

struct point {
	int x;
	int y;
};

struct size {
	int width;
	int height;
};

struct area {
	struct point p0;
	struct point p1;
};

#endif /* metric_h */
