/**
 * Point, size, area.
 */

#ifndef metric_h
#define metric_h

/**
 * Immutable.
 */
class point {
private:
    int x_;
    int y_;
    
public:
    point(int x, int y): x_(x), y_(y) {}
    
    int x() { return x_; }
    int y() { return y_; }
};

/**
 * Immutable.
 */
class size {
private:
    int width_;
    int height_;
    
public:
    size(int width, int height): width_(width), height_(height) {}
    
    int width() { return width_; }
    int height() { return height_; }
};





#endif /* metric_h */
