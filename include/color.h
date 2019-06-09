
#ifndef color_h
#define color_h

/**
 * Immutable.
 */
class color {
private:
    int r_;
    int g_;
    int b_;
    
public:
    color(int r, int g, int b): r_(r), g_(g), b_(b) {}
    
    int r() { return r_; }
    int g() { return g_; }
    int b() { return b_; }
};

#endif /* color_h */
