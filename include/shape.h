
#ifndef shape_h
#define shape_h

#include "color.h"

class shape {
public:
    enum type {
        RECT = 1
        /**
         * Supporting only rect for shape of visual_element for now...
         */
    };
    
private:
    type type_;
    color color_;
    
public:
    shape(type type, color color): type_(type), color_(color) {}
    
    type type() { return type_; }
    color color() { return color_; }
};

#endif /* shape_h */
