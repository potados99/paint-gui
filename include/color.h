
#ifndef color_h
#define color_h

/**
 * Immutable.
 */
class color16 {
private:
    unsigned short r_;
    unsigned short g_;
    unsigned short b_;
    
public:
    static color16 black;
    static color16 white;
    static color16 red;
    static color16 green;
    static color16 blue;
    static color16 yellow;
    static color16 cyan;
    static color16 maganta;
    
    color16(unsigned short r, unsigned short g, unsigned short b): r_(r), g_(g), b_(b) {}
    
    unsigned short r() { return r_; }
    unsigned short g() { return g_; }
    unsigned short b() { return b_; }
    
    unsigned short to_short() { return (((r_ >> 3) << 11) | ((g_ >> 2) << 5) | (b_ >> 3)); }
};

#endif /* color_h */
