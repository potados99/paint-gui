
#ifndef color_h
#define color_h

/**
 * Immutable.
 */
class color {
private:
    unsigned short r_;
    unsigned short g_;
    unsigned short b_;
    
public:
    static color black;
    static color white;
    static color red;
    static color green;
    static color blue;
    static color yellow;
    static color cyan;
    static color maganta;
    
    color(unsigned short r, unsigned short g, unsigned short b): r_(r), g_(g), b_(b) {}
    
    unsigned short r() { return r_; }
    unsigned short g() { return g_; }
    unsigned short b() { return b_; }
    
    unsigned short to_short() { return (((r_ >> 3) << 11) | ((g_ >> 2) << 5) | (b_ >> 3)); }
};

#endif /* color_h */
