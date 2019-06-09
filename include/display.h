
#ifndef display_h
#define display_h

#include "metric.h"
#include "color.h"

class display {
private:
    int             width_;
    int             height_;
    
    int             fd_;
    unsigned short  *mem_;
    unsigned short  *buf_;
    unsigned long   *bitmap_;
    
    int map();
    int unmap();
    
    /**
     * No error or exception handling in functions starting with underscroe.
     */
    int _draw_point(int x, int y, color16 c);
    int _draw_line(int x0, int y0, int x1, int y1, color16 c);
    int _draw_line_low(int x0, int y0, int x1, int y1, color16 c);
    int _draw_line_high(int x0, int y0, int x1, int y1, color16 c);
    int _draw_rect(int x, int y, int width, int height, color16 c);
    int _commit(int x, int y, int width, int height);
    
    int _points_to_point_and_size(int x0, int y0, int x1, int y1, int *x, int *y, int *width, int *height);

    void _write(int offset, color16 c);
    void _apply(int x, int y, int width, int height);
    void _flush();

    int _get_bit(int offset);
    void _set_bit(int offset, int value);
    
public:
    display(const char *dp_fd_path);
    ~display();
    
    int draw_point(point p, color16 c);
    
    int draw_line(point p0, point p1, color16 c);
    
    int draw_rect(point p0, point p1, color16 c);
    int draw_rect(point p, size s, color16 c);
    
    int clear();
    int clear(color16 c);
    
    int commit();
    int commit(point p0, point p1);
    int commit(point p, size s);
};

#endif /* display_h */
