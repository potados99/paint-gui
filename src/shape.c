
#include "shape.h"
#include "debug.h"

#include <stdlib.h>

void shape_move(struct shape *shape, int delta_x, int delta_y) {
    NULL_CHECK("shape_move()", shape);
    
    shape->offset[0] += delta_x;
    shape->offset[1] += delta_y;
}
