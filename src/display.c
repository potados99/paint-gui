#include "display.h"
#include "machine_specific.h"
#include "debug.h"

#include <sys/mman.h>

/**
 * 이것들은 display.c에 종속되는 전역 변수로 두었습니다.
 * display라는 모듈 또는 객체를 만들어 그곳에 보관할 수도 있지만,
 * 있어봤자 디스플레이는 한개인데...
 */
static unsigned short   *dp_mem = NULL;         /* 실제 디스플레이의 파일 기술자에 map될 메모리 주소를 담는 변수. */
static unsigned short 	dp_buf[DP_MEM_SIZE];    /* 디스플레이에 쓰기 전, 변화를 저장하는 버퍼 역할의 변수.*/
static unsigned long 	bitmap[DP_BITMAP_SIZE]; /* 변화가 생긴 지점을 저장하는 메타데이터 역할의 변수. */

static bool             direct;                 /* 직접 쓰기 모드의 활성화 여부를 저장하는 변수. */


/*************************  [ 이 소스파일에서만 쓰이는 인라인함수들 (시작)] *************************/
/**
 * 이 친구들은 퍼포먼스가 중요한 친구들입니다.
 */

/**
 * 임시 버퍼에 쓰인 변경사항을 실제 디스플레이에 적용합니다.
 */
static inline void _apply(int x, int y, int width, int height) {
    /**
     * 점은 음수일 수 있어도, 크기는 그러면 안됩니다.
     */
    ENSURE_SIZE_POSITIVE(x, y, width, height);
    
    /**
     * 영역이 화면 안에 들어가도록 넘치는 부분을 자릅니다.
     */
    if (x >= DP_WIDTH || y >= DP_HEIGHT) {
        /**
         * 의미없는 실행..리턴해줍니다.
         */
        return;
    }
    if (x + width < 0 || y + height < 0) {
        /**
         * 얘도 의미없습니다...
         */
        return;
    }
    
    /**
     * 시작점이 화면 안에 위치하도록 잘라줌니다.
     */
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    
    /**
     * 너비와 높이가 화면 밖으로 넘치지 않도록 잘라줍니다.
     */
    if (x + width >= DP_WIDTH) {
        width = DP_WIDTH - x;
    }
    if (y + height >= DP_HEIGHT) {
        height = DP_HEIGHT - y;
    }
    
    print_trace("_apply(): apply changes at x: %d, y: %d, width: %d, height: %d.\n", x, y, width, height);
    
    const int	 	offset_max = (x + width - 1) + (DP_WIDTH * (y + height - 1));
    register int 	offset = x + (DP_WIDTH * y);
    register int 	n_line = 0;
    
    do {
        if (GET_BIT32(bitmap, offset)) {
            
            print_trace("_apply(): write to display memory at offset %d. (until %d).\n", offset, offset_max);
            
            *(dp_mem + offset) = *(dp_buf + offset);
            
            UNSET_BIT32(bitmap, offset);
        }
        
        
        if (++n_line >= width) {
            n_line = 0;
            offset += (DP_WIDTH - width);
        }
        
        ++offset;
        
        
    } while (offset <= offset_max);
    
}

/**
 * 변경사항을 임시 버퍼에 씁니다.
 * 바로 디스플레이로 가지는 않습니다.
 */
static inline void _modify(int offset,  unsigned short color) {
    ASSERTDO(IN_RANGE(offset, 0, DP_MEM_SIZE - 1), print_trace("_modify(): offset{%d} out of range.\n", offset); return);
    ASSERTDO(dp_mem != NULL, print_error("_modify(): dp_mem is null. call disp_map before use!\n"); return);
    
    print_trace("_modify(): modify pixel at offset %d to %d.\n", offset, color);
    
    if (direct) {
        *(dp_mem + offset) = color;
    }
    else {
        *(dp_buf + offset) = color;
        SET_BIT32(bitmap, offset);
    }
}

/**
 * 선에 기울기에 따라 두 함수를 사용합니다.
 */
static inline void _line_low(int x0, int y0, int x1, int y1, unsigned short color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    
    int D = 2*dy - dx;
    int y = y0;
    
    for (int x = x0; x <= x1; ++x) {
        _modify(x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            y += yi;
            D -= 2*dx;
        }
        
        D += 2*dy;
    }
}
static inline void _line_high(int x0, int y0, int x1, int y1, unsigned short color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    
    int D = 2*dx - dy;
    int x = x0;
    
    for (int y = y0; y <= y1; ++y) {
        _modify(x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            x += xi;
            D -= 2*dy;
        }
        
        D += 2*dx;
    }
}

/**
 * (center_x, center_y)에 장축과 단축이 각각 a, b인 타원을 그립니다.
 */
static inline void _oval(int a, int b, int center_x, int center_y, bool fill, unsigned color) {
    int aa = a * a; /* a의 제곱 */
    int bb = b * b; /* b의 제곱 */
    
    int xk;    /* 현재 x값 */
    int yk; /* 현재 y값 */
    
    int dx; /* 현재 점(xk, yk)에서 x의 변화량 */
    int dy; /* 현재 점(xk, yk)에서 y의 변화량 */
    
    int d1; /* 1 구역 판별식의 값 */
    int d2; /* 2 구역 판별식의 값 */
    
    /**
     * x 독립변수 구간 (1 구역).
     * 1사분면 x 독립변수 구간에서 +x방향으로 진행할 때, 현재 점(xk, yk)의 다음 점으로 가능한 후보는
     * (xk + 1, yk) 또는 (xk + 1, yk - 1) 둘 밖에 없으며, 두 점의 중간점은
     * (xk + 1, yk - 0.5)이다. 따라서 1 구역에서 판별식은 d1(x, y) = F(x + 1, y - 0.5)이다.
     */
    
    xk = 0;
    yk = b;
    
    dx = 2 * bb * xk;
    dy = 2 * aa * yk;
    
    /**
     * 1 구역에서 쓸 판별식의 초기값.
     * d1(0, b)의 값, 즉 F(1, b - 0.5)이다.
     */
    d1 = bb - (b * aa) + (0.25 * aa);
    
    while (dx < dy) {
        /**
         * 그리자
         */
        if (fill) {
            disp_draw_linep(xk + center_x,
                            yk + center_y,
                            xk + center_x,
                            -yk + center_y, color);
            
            disp_draw_linep(-xk + center_x,
                            yk + center_y,
                            -xk + center_x,
                            -yk + center_y, color);
            
        }
        else {
            disp_draw_point(xk + center_x, yk + center_y, color);
            disp_draw_point(-xk + center_x, yk + center_y, color);
            disp_draw_point(xk + center_x, -yk + center_y, color);
            disp_draw_point(-xk + center_x, -yk + center_y, color);
        }
        
        /**
         * x는 독립변수니까 조건없이 그냥 더해줍니다.
         */
        ++xk;
        dx += (2 * bb);
        
        if (d1 < 0) {
            /**
             * 중간점이 타원 안에 있는 경우이므로, 타원 밖과 가까운 (x + 1, y)가 다음 점으로 적절.
             */
            
            /**
             * d1에 dx + bb를 더하는 이유는...
             *
             * 일단 1 구역에서 사용한 판별식은 d1(x, y) = F(0, b - 0.5)입니다.
             * 이번에 사용한 판별식은 d1(xk, yk) = F(xk + 1, yk - 0.5)입니다.
             * 이번에 쓴 판별식의 값이 0보다 작으니 다음 판별식의 값은 d1(xk + 1, yk)입니다.
             * 즉 다음 판별식의 값은 F(xk + 2, yk - 0.5)입니다.
             * F(xk + 2, yk - 0.5)는
             * F(xk + 1, yk - 0.5) + (F(xk + 2, yk - 0.5) - F(xk + 1, yk - 0.5))로도 구할 수 있습니다.
             * 다시 말해서 d1(xk + 1, yk)은 d1(xk, yk) + (d1(xk + 1, yk) - d1(xk, yk))로도 구할 수 있습니다.
             *
             * d1의 함숫값을 새로 계산할 것 없이, d1(x + 1, y) - d1(x, y)에 해당하는 값만 더해주면 된다는 것입니다.
             * 하여 계산한 d1(x + 1, y) - d1(x, y)은 2*bb*x + 2*bb에 해당하고, 이것은 dx + bb에 해당합니다!!!
             *
             * 이 설명은 d1의 함숫값이 0보다 클 때에도 같습니다.
             * 구역 2의 d2에도 같은 설명을 적용할 수 있습니다.
             */
            d1 += (dx + bb);
        }
        else {
            /**
             * 중간점이 타원 밖에 있는 경우이므로, 타원 안과 가까운 (x + 1, y - 1)이 다음 점으로 적절.
             */
            --yk;
            dy -= (2 * aa);
            d1 += (dx - dy + bb);
        }
    }
    
    /**
     * y 독립변수 구간 (2 구역).
     * 1사분면 y 독립변수 구간에서 -y방향으로 진행할 때, 현재 점의 다음 점으로 가능한 후보는
     * (xk, yk - 1) 또는 (xk - 1, yk - 1) 둘 밖에 없으며, 두 점의 중간점은
     * (xk - 0.5, yk - 1)이다. 따라서 2 구역에서 판별식은 d(x, y) = F(x - 0.5, y - 1)이다.
     */
    
    xk = a;
    yk = 0;
    
    dx = 2 * bb * xk;
    dy = 2 * aa * yk;
    
    /**
     * 2 구역에서 쓸 판별식의 초기값.
     * d2(a, 0)의 값, 즉 F(a - 0.5, -1)이다.
     */
    d2 = aa - (a * bb) + (0.25 * bb);
    
    while (dx > dy) {
        /**
         * 그리자
         */
        if (fill) {
            disp_draw_linep(xk + center_x,
                            yk + center_y,
                            -xk + center_x,
                            yk + center_y, color);
            
            disp_draw_linep(xk + center_x,
                            -yk + center_y,
                            -xk + center_x,
                            -yk + center_y, color);
        }
        else {
            disp_draw_point(xk + center_x, yk + center_y, color);
            disp_draw_point(-xk + center_x, yk + center_y, color);
            disp_draw_point(xk + center_x, -yk + center_y, color);
            disp_draw_point(-xk + center_x, -yk + center_y, color);
        }
        
        /**
         * y는 독립변수니까 조건없이 그냥 더해줍니다.
         */
        ++yk;
        dy += (2 * aa);
        
        if (d2 < 0) {
            /**
             * 두 점의 중점이 타원 안에 있으므로, 타원 밖에 가까운 점인 (x, y - 1)을 선택.
             */
            d2 += (dy + aa);
        }
        else {
            /**
             * 두 점의 중점이 타원 밖에 있으므로, 타원 안에 가까운 점인 (x - 1, y - 1)을 선택.
             */
            --xk;
            dx -= (2 * bb);
            d2 += (dy - dx + aa);
        }
    }
    
}

/*************************  [ 이 소스파일에서만 쓰이는 인라인함수들 (끝)] *************************/


void disp_map(int fd) {
    dp_mem = (unsigned short *)mmap(NULL, DP_MEM_SIZEB, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERTDO(dp_mem != MAP_FAILED, print_error("disp_map(): mmap() failed.\n"); return);
}

void disp_unmap() {
    munmap(dp_mem, DP_WIDTH * DP_HEIGHT * PIXEL_SIZE);
}

void disp_set_direct(bool value) {
    direct = value;
}

void disp_draw_point(int x, int y, unsigned short color) {
    print_trace("disp_draw_point(): draw point at (%d, %d).\n", x, y);
    
    _modify(x + (y * DP_WIDTH), color);
}

void disp_draw_linep(int x0, int y0 , int x1, int y1, unsigned short color) {
    print_trace("disp_draw_line(): draw line between p0(%d, %d) and p1(%d, %d).\n", x0, y0 , x1, y1);
    
    if (ABS(y1 - y0) < ABS(x1 - x0)) {
        if (x0 > x1) {
            _line_low(x1, y1, x0, y0, color);
        }
        else {
            _line_low(x0, y0, x1, y1, color);
        }
    }
    else {
        if (y0 > y1) {
            _line_high(x1, y1, x0, y0, color);
        }
        else {
            _line_high(x0, y0, x1, y1, color);
        }
    }
}

void disp_draw_rect(int x, int y, int width, int height, unsigned short color) {
    print_trace("disp_draw_rect(): draw rect at point(%d, %d) with size(%d, %d).\n", x, y, width, height);
    
    ENSURE_SIZE_POSITIVE(x, y, width, height);
    
    disp_draw_rectp(x, y, x + width - 1, y + height - 1, color);
}

void disp_draw_rect_fill(int x, int y, int width, int height, unsigned short color) {
    print_trace("disp_draw_rect_fill(): draw rect at point(%d, %d) with size(%d, %d).\n", x, y, width, height);
    
    ENSURE_SIZE_POSITIVE(x, y, width, height);
    
    const int 	    offset_max = (x + width - 1) + (DP_WIDTH * (y + height - 1));
    register int 	offset = x + (DP_WIDTH * y);
    register int 	n_line = 0;
    
    do {
        _modify(offset, color);
        
        if (++n_line >= width) {
            n_line = 0;
            offset += (DP_WIDTH - width);
        }
        
        ++offset;
        
    } while (offset < offset_max + 1);
}

void disp_draw_rectp(int x0, int y0, int x1, int y1, unsigned short color) {
    print_trace("disp_draw_rectp(): draw rect from point(%d, %d) to (%d, %d).\n", x0, y0, x1, y1);
    
    ENSURE_POINTS_ORDERED(x0, y0, x1, y1);
    
    disp_draw_linep(x0, y0, x1, y0, color); /* 상 */
    disp_draw_linep(x0, y1, x1, y1, color); /* 하 */
    disp_draw_linep(x0, y0, x0, y1, color); /* 좌 */
    disp_draw_linep(x1, y0, x1, y1, color); /* 우 */
}

void disp_draw_rectp_fill(int x0, int y0, int x1, int y1, unsigned short color) {
    print_trace("disp_draw_rectp_fill(): draw rect from point(%d, %d) to (%d, %d).\n", x0, y0, x1, y1);
    
    ENSURE_POINTS_ORDERED(x0, y0, x1, y1);
    
    disp_draw_rect_fill(x0, y0, x1 - x0 + 1, y1 - y0 + 1, color);
}

void disp_draw_oval(int x, int y, int width, int height, unsigned short color) {
    ENSURE_SIZE_POSITIVE(x, y, width, height);
    
    _oval(width/2, height/2, x + (width/2), y + (height/2), false, color);
}

void disp_draw_oval_fill(int x, int y, int width, int height, unsigned short color) {
    ENSURE_SIZE_POSITIVE(x, y, width, height);
    
    _oval(width/2, height/2, x + (width/2), y + (height/2), true, color);
}

void disp_draw_ovalp(int x0, int y0, int x1, int y1, unsigned short color) {
    ENSURE_POINTS_ORDERED(x0, y0, x1, y1);
    
    _oval((x1 - x0)/2, (y1 - y0)/2, x0 + ((x1 - x0)/2), y0 + ((y1 - y0)/2), false, color);
}

void disp_draw_ovalp_fill(int x0, int y0, int x1, int y1, unsigned short color) {
    ENSURE_POINTS_ORDERED(x0, y0, x1, y1);
    
    _oval((x1 - x0)/2, (y1 - y0)/2, x0 + ((x1 - x0)/2), y0 + ((y1 - y0)/2), true, color);
}

void disp_draw_whole(unsigned short color) {
    return disp_draw_rect_fill(0, 0, DP_WIDTH, DP_HEIGHT, color);
}

void disp_draw_2d_shape(struct shape *shape) {
    NULL_CHECK("disp_draw_2d_shape()", shape);
    
    draw_2d draw_function = NULL;
    
    switch (shape->type) {
        /**
         * 함수 mapping 테이블입니다.
         */
        case ST_LINEP:          draw_function = disp_draw_linep; break;
        case ST_RECT:           draw_function = disp_draw_rect; break;
        case ST_RECT_FILL:      draw_function = disp_draw_rect_fill; break;
        case ST_RECTP:          draw_function = disp_draw_rectp; break;
        case ST_RECTP_FILL:     draw_function = disp_draw_rectp_fill; break;
        case ST_OVAL:           draw_function = disp_draw_oval; break;
        case ST_OVAL_FILL:      draw_function = disp_draw_oval_fill; break;
        case ST_OVALP:          draw_function = disp_draw_ovalp; break;
        case ST_OVALP_FILL:     draw_function = disp_draw_ovalp_fill; break;
        case ST_FREEP:          goto free_draw;
            
        default:                print_error("disp_draw_2d_shape(): invalid shape type: %d\n", shape->type); return;
    }
    
    if (SHAPE_BY_TWO_POINTS(shape->type)) {
        if (shape->type == ST_LINEP) {
            /**
             * 선은 두 점이 반드시 좌상단과 우하단을 대표하지 않습니다.
             * 따라서 변환 없이 그냥 넘겨야 합니다.
             */
            draw_function(shape->value[0] + shape->offset[0],
                          shape->value[1] + shape->offset[1],
                          shape->value[2] + shape->offset[0],
                          shape->value[3] + shape->offset[1],
                          shape->color);
        }
        else {
            SHAPE_EXPORT_AREA_TO_TWO_POINTS(shape, x0, y0, x1, y1);
            draw_function(x0, y0, x1, y1, shape->color);
        }
    }
    else {
        SHAPE_EXPORT_AREA_TO_TWO_POINTS(shape, x, y, width, height);
        draw_function(x, y, width, height, shape->color);
    }
    
    return;
    
free_draw:
    /**
     * Freedraw는 void (int, int, int, int, unsigned short) 타입으로 맞추기가 좀 그래서
     * 별도의 함수로 빼지 않고 이곳에 루틴으로 분리해놓았습니당.
     */
    NULL_CHECK("disp_draw_2d_shape()", shape->fdraw_points.next);
    
    if (list_empty(&shape->fdraw_points))
        return;
    
    struct point_node   *cur = list_first_entry(&shape->fdraw_points, struct point_node, list);
    int                 last_x = cur->x + shape->offset[0];
    int                 last_y = cur->y + shape->offset[1];
    
    list_for_each_entry_continue(cur, &shape->fdraw_points, list) {
        disp_draw_linep(last_x, last_y, cur->x + shape->offset[0], cur->y + shape->offset[1], shape->color);
        
        last_x = cur->x + shape->offset[0];
        last_y = cur->y + shape->offset[1];
    }
    
    return;
}

void disp_commit() {
    print_trace("disp_commit(): commit all changes.\n");
    
    _apply(0, 0, DP_WIDTH, DP_HEIGHT);
}

void disp_commit_partial(int x, int y, int width, int height) {
    print_trace("disp_commit_partial(): commit changes partially, at point(%d, %d), size(%d, %d).\n", x, y, width, height);
    
    _apply(x, y, width, height);
}

void disp_commit_partialp(int x0, int y0, int x1, int y1) {
    ENSURE_RIGHT_BIGGER(x0, x1);
    ENSURE_RIGHT_BIGGER(y0, y1);
    
    disp_commit_partial(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
}

void disp_cancel() {
    print_trace("disp_cancel(): remove changes from bitmap.\n");
    
    memset(bitmap, 0, DP_BITMAP_SIZEB);
}

void disp_clear() {
    print_trace("disp_clear(): clear screen to black.\n");
    
    memset(dp_mem, 0, DP_MEM_SIZEB);
    memset(dp_buf, 0, DP_MEM_SIZEB);
    memset(bitmap, 0, DP_BITMAP_SIZEB);
}
