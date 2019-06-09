
#ifndef display_h
#define display_h

#include "metric.h"
#include "color.h"

class display {
private:
    int             width_;
    int             height_;
    
    unsigned short  *mem_;
    
    
    int map(int fd);
    int unmap();
    
    int _draw_point(int x, int y, color c);
    int _draw_line(int x0, int y0, int x1, int y1, color c);
    int _draw_rect(int x, int y, int width, int height, color c);
    int _commit(int x, int y, int width, int height);
    
    int _points_to_point_and_size(int x0, int y0, int x1, int y1, int *x, int *y, int *width, int *height);

public:
    int draw_point(point p, color c);
    
    int draw_line(point p0, point p1, color c);
    
    int draw_rect(point p0, point p1, color c);
    int drwa_rect(point p, size s, color c);
    
    int clear();
    int clear(color c);
    
    int commit();
    int commit(point p0, point p1);
    int commit(point p, size s);
};

#endif /* display_h */
