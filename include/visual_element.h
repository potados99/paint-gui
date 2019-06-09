
#ifndef visual_element_h
#define visual_element_h

#include "object.h"
#include "drawable.h"
#include "metric.h"
#include "shape.h"

class visual_element: public object, public drawable {
private:
    static int  element_count;

    point       location_;
    size        size_;
    shape       shape_;
    bool        visible_;
    int         zindex_;
    
public:
    visual_element(point location, size size, shape shape): location_(location), size_(size), shape_(shape), visible_(true), zindex_(element_count++) {}
    
    /**
     * from drawable
     */
    virtual point   get_location() { return location_; }
    virtual size    get_size() { return size_; }
    virtual shape   get_shape() { return shape_; }
    virtual bool    get_visible() { return visible_; }
    virtual int     get_zindex() { return zindex_; }
    
    
};

#endif /* visual_element_h */
