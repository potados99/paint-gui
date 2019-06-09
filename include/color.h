
#ifndef color_h
#define color_h

/**
 * Immutable.
 */
class color {
private:
    unsigned char r_;
    unsigned char g_;
    unsigned char b_;
    
public:
    color(unsigned char r, unsigned char g, unsigned char b): r_(r), g_(g), b_(b) {}
    
    unsigned char r() { return r_; }
    unsigned char g() { return g_; }
    unsigned char b() { return b_; }
};

#endif /* color_h */
