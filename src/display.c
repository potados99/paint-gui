#include "display.h"
#include "metric.h"
#include "machine_specific.h"
#include "debug.h"
#include "macros.h"

#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

/**
 * 이것들은 display.c에 종속되는 전역 변수로 두었습니다.
 * display라는 모듈 또는 객체를 만들어 그곳에 보관할 수도 있지만,
 * 있어봤자 디스플레이는 한개인데...
 */
static unsigned short   *dp_mem;                /* 실제 디스플레이의 파일 기술자에 map될 메모리 주소를 담는 변수. */
static unsigned short 	dp_buf[DP_MEM_SIZE];    /* 디스플레이에 쓰기 전, 변화를 저장하는 버퍼 역할의 변수.*/
static unsigned long 	bitmap[DP_BITMAP_SIZE]; /* 변화가 생긴 지점을 저장하는 메타데이터 역할의 변수. */

static bool             direct;                 /* 직접 쓰기 모드의 활성화 여부를 저장하는 변수. */

/**
 * 당황하지 마십시오!!!
 * 아주 낮은 레벨의 함수에서 쓸 파라미터 확인용 매크로입니다.
 * 이 소스파일에서만 쓰일 예정이어서 이곳에 두었습니다.
 *
 * POINT_CHECK은 x와 y 좌표가 각각 폐구간 [0, 319], [0, 239]에 속하는지 확인합니다.
 * SIZE_CHECK은 width와 height가 각각 폐구간 [0, 320], [0, 240]에 속하는지 확인합니다.
 * 만약에 적절치 않으면 함수 이름과 함께 에러 메시지를 뿜고 리턴해버립니다.
 *
 * 그런데 사실 몇번 뜯어고치면서 이것들도 안쓰게 되었습니다...
 */
#define POINT_CHECK(FUNC_NAME, X, Y)                                                        \
do {                                                                                        \
ASSERTDO(IN_RANGE(X, 0, DP_WIDTH - 1),                                                      \
print_error(FUNC_NAME ": " #X "{%d} out of range.\n", X);                                   \
return);                                                                                    \
ASSERTDO(IN_RANGE(Y, 0, DP_HEIGHT - 1),                                                     \
print_error(FUNC_NAME ": " #Y "{%d} out of range.\n", Y);                                   \
         return);                                                                           \
} while (0)

#define SIZE_CHECK(FUNC_NAME, W, H)                                                         \
do {                                                                                        \
ASSERTDO(IN_RANGE(W, 0, DP_WIDTH),                                                          \
print_error(FUNC_NAME ": " #W "{%d} out of range.\n", W);                                   \
return);                                                                                    \
ASSERTDO(IN_RANGE(H, 0, DP_HEIGHT),                                                         \
print_error(FUNC_NAME ": " #H "{%d} out of range.\n", H);                                   \
return);                                                                                    \
} while (0)



/********************************************************************************************/
/**
 * 여기에 속하는 함수 또는 매크로들은 1초에 적어도 수십번씩 실행될 것들이라
 * 퍼포먼스가 매우매우 중요합니다...
 */
#define GET_BIT(PTR, OFFSET)                        \
((*(PTR + (OFFSET / 32))) & (1 << (OFFSET % 32)))

#define SET_BIT(PTR, OFFSET)                        \
do {                                                \
*(PTR + (OFFSET / 32)) |= (1 << (OFFSET % 32));     \
} while (0)

#define UNSET_BIT(PTR, OFFSET)                      \
do {                                                \
*(PTR + (OFFSET / 32)) &= ~(1 << (OFFSET % 32));    \
} while (0)

static inline void _apply(short x, short y, short width, short height) {
    /**
     * Points can be negative, but size cannot.
     */
    SIZE_CHECK("_apply", width, height);
  
    /**
     * Cur area to fit in screen.
     */
    if (x >= DP_WIDTH || y >= DP_HEIGHT) {
        /**
         * Meaningless.
         */
        return;
    }
    if (x + width < 0 || y + height < 0) {
        /**
         * Meaningless, also.
         */
        return;
    }
    
    /**
     * Bring start point into screen.
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
     * Cut excess area at the right and the bottom.
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
	register short 	n_line = 0;

	do {
		if (GET_BIT(bitmap, offset)) {

            print_trace("_apply(): write to display memory at offset{%d max} %d.\n", offset_max, offset);

			*(dp_mem + offset) = *(dp_buf + offset);
		
            UNSET_BIT(bitmap, offset);
		}


		if (++n_line >= width) {
			n_line = 0;
			offset += (DP_WIDTH - width);
		} 
	
		++offset;
	

	} while (offset <= offset_max);

}

static inline void _modify(int offset,  unsigned short color) {
    ASSERTDO(IN_RANGE(offset, 0, DP_MEM_SIZE - 1), print_info("_modify: offset{%d} out of range.\n", offset); return);

    print_trace("_modify(): modify pixel at offset %d to %d.\n", offset, color);

    if (direct) {
        *(dp_mem + offset) = color;
    }
    else {
        *(dp_buf + offset) = color;
        SET_BIT(bitmap, offset);
    }
}

static inline void _line_low(short x0, short y0, short x1, short y1, unsigned short color) {
    short dx = x1 - x0;
    short dy = y1 - y0;
    short yi = 1;
    
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    
    short D = 2*dy - dx;
    short y = y0;
    
    for (short x = x0; x <= x1; ++x) {
        _modify(x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            y += yi;
            D -= 2*dx;
        }
        
        D += 2*dy;
    }
}

static inline void _line_high(short x0, short y0, short x1, short y1, unsigned short color) {
    short dx = x1 - x0;
    short dy = y1 - y0;
    short xi = 1;
    
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    
    short D = 2*dx - dy;
    short x = x0;
    
    for (short y = y0; y <= y1; ++y) {
        _modify(x + (DP_WIDTH * y), color);
        
        if (D > 0) {
            x += xi;
            D -= 2*dy;
        }
        
        D += 2*dx;
    }
}
/********************************************************************************************/


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

void disp_draw_point(short x, short y, unsigned short color) {
    print_trace("disp_draw_point(): draw point at (%d, %d).\n", x, y);

	_modify(x + (y * DP_WIDTH), color);
}

void disp_draw_line(short x0, short y0 , short x1, short y1, unsigned short color) {
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

void disp_draw_rect(short x, short y, short width, short height, unsigned short color) {
    print_trace("disp_draw_rect(): draw rect at point(%d, %d) with size(%d, %d).\n", x, y, width, height);

	const int 	    offset_max = (x + width - 1) + (DP_WIDTH * (y + height - 1));
	register int 	offset = x + (DP_WIDTH * y);
	register short 	n_line = 0;

	do {
		_modify(offset, color);
	
		if (++n_line >= width) {
			n_line = 0;
			offset += (DP_WIDTH - width);
		} 
		
		++offset;
		
	} while (offset < offset_max + 1);
}

void disp_draw_rectp(short x0, short y0, short x1, short y1, unsigned short color) {
	disp_draw_rect(MIN(x0, x1), MIN(y0, y1), ABS(x1 - x0), ABS(y1 - y0), color);
}

void disp_draw_whole(unsigned short color) {
    return disp_draw_rect(0, 0, DP_WIDTH, DP_HEIGHT, color);
}

void disp_draw_shape(struct shape *shape);

void disp_commit() {
    print_trace("disp_commit(): commit all changes.\n");

    _apply(0, 0, DP_WIDTH, DP_HEIGHT);
}

void disp_commit_partial(short x, short y, short width, short height) {
    print_trace("disp_commit_partial(): commit changes partially, at point(%d, %d), size(%d, %d).\n", x, y, width, height);

    _apply(x, y, width, height);
}

void disp_commit_partialp(short x0, short y0, short x1, short y1) {
	disp_commit_partial(MIN(x0, x1), MIN(y0, y1), ABS(x1 - x0), ABS(y1 - y0));
}

void disp_cancel() {
    print_trace("disp_cancel(): remove changes from bitmap.\n");
    
    memset(bitmap, 0, DP_BITMAP_SIZEB);
}

void disp_clear() {
    print_trace("disp_clear(): clear screen to black.\n");

    memset(dp_mem, 0, DP_MEM_SIZEB);
}
