

#ifndef drawable_h
#define drawable_h

#include "metric.h"
#include "shape.h"

class drawable {
    virtual point   get_location() = 0;
    virtual size    get_size() = 0;
    virtual shape   get_shape() = 0;
    virtual bool    get_visible() = 0;
    virtual int     get_zindex() = 0;
};



#endif /* drawable_h */
