
#include "metric.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>

/**
 * TODO: 구현하세욧.
 */

/*
static inline void _init_points_list(struct points *points) {
    
}
*/

void points_add(struct points *points, int x, int y) {
    NULL_CHECK("points_add()", points);
    
    /**
     * 새로이 할당하고 대입하여줍니다.
     */
    POINT_NODE_ALLOC(new_node);
    new_node->x = x;
    new_node->y = y;
    
    /**
     * 만약 points 리스트가 텅텅 비었다면...
     */
    if (points->head == NULL) {
        /**
         * head와 tail에 대입해줍니다.
         */
        points->head = new_node;
        points->tail = new_node;
    }
    else {
        /**
         * 아니라면, 마지막에 붙여줍니다.
         */
        points->tail->next = new_node;
        points->tail = new_node;

    }
}

void points_free(struct points *points) {
    NULL_CHECK("points_free()", points);
    
    if (points->head == NULL) {
        print_info("points_free(): list is empty.\n");
        return;
    }

    FOREACH_POINT(points, p) {
    }
}
