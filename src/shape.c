#include "shape.h"
#include "debug.h"

struct shape *shape_create(unsigned char type,
                           int v0,
                           int v1,
                           int v2,
                           int v3,
                           unsigned short color) {
    
    /**
     * 새 shape를 할당하고 초기화합니다.
     * zindex는 여기서 자동으로 이전보다 하나 증가하여 대입됩니다.
     */
    SHAPE_ALLOC(node_allocated);

    /**
     * 처음에는 이정도만 대입해줍니다.
     */
    node_allocated->type = type;
    node_allocated->value[0] = v0;
    node_allocated->value[1] = v1;
    node_allocated->value[2] = v2;
    node_allocated->value[3] = v3;
    node_allocated->color = color;
    
    /**
     * 리스트 노드 관계를 초기화해줍니당.
     */
    LIST_HEAD_REINIT(node_allocated->list);
    
    /**
     * 만약 자유 그리기라면, fdraw_points 리스트도 초기화해줍니다.
     */
    if (type == ST_FREEP)
        LIST_HEAD_REINIT(node_allocated->fdraw_points);
    
    return node_allocated;
}

void shape_delete(struct shape *shape) {
    NULL_CHECK("shape_delete()", shape);

    if (shape->type == ST_FREEP && shape->fdraw_points.next != NULL) {
        points_free(&shape->fdraw_points);
    }
    
    list_del(&shape->list);
    
    free(shape);
}

void shapes_list_add(struct list_head *shapes_head, struct shape *shape) {
    NULL_CHECK("shapes_list_add()", shapes_head);

    list_add_tail(&shape->list, shapes_head);
}

struct shape *shapes_list_peek_last(struct list_head *shapes_head) {
    NULL_CHECK_RET("shapes_list_peek_last()", shapes_head, NULL);
    ASSERTDO(!list_empty(shapes_head), return NULL);
    
    return list_last_entry(shapes_head, struct shape, list);
}

bool shape_point_in_shape_area(struct shape *shape, int x, int y) {
    NULL_CHECK_RET("shape_point_in_shape_area()", shape, false);

    SHAPE_EXPORT_AREA_TO_TWO_POINTS(shape, x0, y0, x1, y1);
    
    return (IN_RANGE(x, x0, x1) && IN_RANGE(y, y0, y1));
}

void shape_move(struct shape *shape, int delta_x, int delta_y) {
    NULL_CHECK("shape_move()", shape);
    
    shape->offset[0] += delta_x;
    shape->offset[1] += delta_y;
}

void shape_transform(struct shape *shape, int delta_width, int delta_height) {
    NULL_CHECK("shape_transform()", shape);

    shape->value[2] += delta_width;
    shape->value[3] += delta_height;
}

void shape_add_point(struct shape *shape, int x, int y) {
    NULL_CHECK("shape_add_point()", shape);

    ASSERTDO(shape->type == ST_FREEP, print_info("shape_add_point(): cannot add point to non-freedraw type shape.\n"); return);
    
    /**
     * 처음 찍히는 점이라면, 이에 맞게 영역을 손봐줍니당.
     */
    if (list_empty(&shape->fdraw_points)) {
        shape->value[0] = shape->value[2] = x;
        shape->value[1] = shape->value[3] = y;
        
    }
    
    /**
     * 점 추가해줍니당.
     */
    points_add(&shape->fdraw_points, x, y);
    
    /**
     * 새로운 점의 좌표가 기존 영역 밖에 있을 때에는
     * 영역을 이에 맞게 넓혀줍니다.
     */
    if (x < shape->value[0]) {
        shape->value[0] = x;
    }
    else if (x > shape->value[2]) {
        shape->value[2] = x;
    }
    
    if (y < shape->value[1]) {
        shape->value[1] = y;
    }
    else if (y > shape->value[3]) {
        shape->value[3] = y;
    }
 
    return;
}
