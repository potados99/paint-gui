
#include "shape.h"
#include "debug.h"

#include <stdlib.h>

void shape_move(struct shape *shape, int delta_x, int delta_y) {
    NULL_CHECK("shape_move()", shape);
    
    shape->offset[0] += delta_x;
    shape->offset[1] += delta_y;
}

struct shape *shapes_link(struct list_head *shapes_head, struct shape *shape) {
    NULL_CHECK_RET("shape_move()", shapes_head, NULL);
    NULL_CHECK_RET("shape_move()", shapes_head, NULL);

    SHAPE_ALLOC(new_node);
    
    *new_node = *shape;
    new_node->list.prev = &new_node->list;
    new_node->list.next = &new_node->list;
    
    list_add_tail(&new_node->list, shapes_head);

    return new_node;
}
