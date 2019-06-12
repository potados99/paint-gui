
#include "metric.h"
#include "debug.h"
#include "list.h"

#include <string.h>
#include <stdlib.h>

/**
 * TODO: 구현하세욧.
 */

void points_add(struct list_head *points_head, int x, int y) {
    NULL_CHECK("points_add()", points_head);
    
    POINT_NODE_ALLOC(new_node);
    
    new_node->x = x;
    new_node->y = y;
    new_node->list.prev = &new_node->list;
    new_node->list.next = &new_node->list;
    
    list_add_tail(&new_node->list, points_head);
}

void points_free(struct list_head *points_head) {
    NULL_CHECK("points_free()", points_head);
    
    struct point_node *cur;
    struct point_node *save;
    
    list_for_each_entry_safe(cur, save, points_head, list) {
        free(cur);
    }
}
