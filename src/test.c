#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "touch.h"
#include "display.h"
#include "machine_specific.h"
#include "debug.h"
#include "shape.h"
#include "test.h"
#include "metric.h"
#include "paint.h"
#include "color.h"

int draw_read_test() {
    int                 ts_fd; /* 터치스크린 파일 기술자 */
    int                 dp_fd; /* 디스플레이 파일 기술자 */
    int                 ts_read; /* 터치 읽은 결과 저장 */
    int                 touched; /* 터치 상태 저장 */
    int                 last_x = 0; /* 이전 터치 x좌표 저장 */
    int                 last_y = 0; /* 이전 터치 y좌표 저장 */
    unsigned long       line_num = 0;
    
    /**
     * touch_event 구조체 선언 & 초기화
     */
    TOUCH_EVENT(te);
    
    /**
     * 터치스크린 열기!
     */
    ts_fd = open(TS_FD_PATH, TS_OPEN_OPTION);
    ASSERTDO(ts_fd != -1, print_error("open() error with %s.\n", TS_FD_PATH); return -1);
    
    /**
     * 디스플레이 열기!
     */
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    /**
     * mmap 해줍니다. 결과는 display.c에 전역변수로 들어가있습니다.
     */
    disp_map(dp_fd);
    
    /**
     * 화면 전체를 흰색으로 칠합니다.
     */
    disp_draw_whole(COLOR(255, 255, 255));
    disp_commit();
    
    /**
     * 도형들을 만들어줍니다.
     */
    LIST_HEAD(shapes_head);
    
    struct shape *rect = shape_create(ST_RECT_FILL, 140, 110, 50, 50, COLOR(0, 255, 0));
    shapes_list_add(&shapes_head, rect);
    
    struct shape *fdraw = shape_create(ST_FREEP, 0, 0, 0, 0, COLOR(0, 0, 255));
    shape_add_point(fdraw, 100, 100);
	shape_add_point(fdraw, 102, 106);
	shape_add_point(fdraw, 106, 112);
	shape_add_point(fdraw, 112, 119);
	shape_add_point(fdraw, 120, 125);
	shape_add_point(fdraw, 130, 131);
    shape_add_point(fdraw, 142, 137);
	shape_add_point(fdraw, 156, 142);
	shape_add_point(fdraw, 172, 150);
	shapes_list_add(&shapes_head, fdraw);
    
    /**
     * 만든 도형들을 그려줍니다.
     */
    struct shape *cur;
    
    list_for_each_entry(cur, &shapes_head, list) {
        disp_draw_2d_shape(cur);
    }
    disp_commit();
    
    for (int i = 0; i < 100; ++i) {
        usleep(20000);
 
		/**
		 * 해당 구역을 지워줍니다.
		 */
		disp_draw_rectp_fill(fdraw->value[0] + fdraw->offset[0], 
							 fdraw->value[1] + fdraw->offset[1], 
							 fdraw->value[2] + fdraw->offset[0], 
							 fdraw->value[3] + fdraw->offset[1], 
							 COLOR(255, 255, 255));
        
		/**
		 * 도형에 offset을 추가하고 싹다 새로 그립니다.
		 */
		fdraw->offset[0]++;
 
        list_for_each_entry(cur, &shapes_head, list) {
            disp_draw_2d_shape(cur);
        }
        
        /**
         * 바뀐 부분만 적용합니다.
         */
        disp_commit_partialp(fdraw->value[0] + fdraw->offset[0] - 1,
                             fdraw->value[1],
                             fdraw->value[2] + fdraw->offset[0],
                             fdraw->value[3]);
    }
    
    /**
     * 적용되지 않은 변화를 모두 지웁니다.
     */
    disp_cancel();
    
    
    /**
     * 터치를 읽어서 각 점과 점 사이에 직선을 그려줍니다.
     */
    while (1) {
        
        ts_read = touch_read(ts_fd, &te);
        
        if (te.touch_state == TOUCH_STATE_BEGIN) {
            /**
             * 이제 막 터치가 시작될 때.
             * 이전 터치 좌표를 현재 터치 시작 좌표로 덮어씁니다.
             */
            last_x = te.x;
            last_y = te.y;
            touched = 1;
        }
        else if (te.touch_state == TOUCH_STATE_DONE) {
            /**
             * 터치가 막 끝났을 때.
             */
            touched = 0;
        }
        
        /**
         * 적당히 출력해줍니다.
         */
        if (te.touch_state == TOUCH_STATE_BEGIN) printf("\n============================= TOUCH START =============================\n");
        printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
        if (te.touch_state == TOUCH_STATE_DONE) printf("============================= TOUCH FINISH =============================\n\n");
        
        /**
         * 이전 점과 현재 점 사이에 직선을 그려주고, 그 구간의 변화를 적용합니다.
         */
        disp_draw_linep(last_x, last_y, te.x, te.y, COLOR(0, 0, 0));
        disp_commit_partialp(MIN(last_x, te.x) - 1, MIN(last_y, te.y) - 1, MAX(last_x, te.x) + 1, MAX(last_y, te.y) + 1);
        
        /**
         * 다음 루프로 넘어가기 전에 현재 점을 이전 점에 대입합니다.
         */
        last_x = te.x;
        last_y = te.y;
    }
    
    
    
    
    /* 도달 불가능한 영역! */
    
    // disp_unmap();
    
    // close(dp_fd);
    // close(ts_fd);
    
}

int list_test() {
    printf("list_test() start.\n");
    
    LIST_HEAD(points_head);
    
    points_add(&points_head, 1, 2);
    points_add(&points_head, 2, 3);
    points_add(&points_head, 3, 4);
    points_add(&points_head, 4, 5);
    points_add(&points_head, 5, 6);
    
    struct point_node *cur;
    list_for_each_entry(cur, &points_head, list) {
        printf("point(%d, %d)\n", cur->x, cur->y);
    }

    points_free(&points_head);
    
    printf("list_test() done.\n\n");

    return 0;
}

int shape_creation_test(void) {
    /**
     * 이것만 있으면 모든 shape의 정보를 털 수 있다!!
     */
    LIST_HEAD(shapes_head);
    
    /**
     * 사각형!
     */
    struct shape *rect = shape_create(ST_RECT, 0, 0, 50, 50, COLOR(255, 255, 255));
    shapes_list_add(&shapes_head, rect);
    
    /**
     * 자유그리기!
     */
    struct shape *fdraw = shape_create(ST_FREEP, 0, 0, 0, 0, COLOR(0, 0, 50));
    shape_add_point(fdraw, 100, 100);
    shape_add_point(fdraw, 125, 125);
    shape_add_point(fdraw, 150, 150);
    shapes_list_add(&shapes_head, fdraw);
    
    /**
     * 순회하며 출력하기!
     */
    struct shape *cur_shape;
    list_for_each_entry(cur_shape, &shapes_head, list) {
        printf("============ SHAPE START ============\n");
        
        printf("Shape type: %d\n",
               cur_shape->type);
        
        printf("Shape values: (%d, %d, %d, %d)\n",
               cur_shape->value[0],
               cur_shape->value[1],
               cur_shape->value[2],
               cur_shape->value[3]);
        
        printf("Shape offset: (%d, %d)\n",
               cur_shape->offset[0],
               cur_shape->offset[1]);
        
        printf("Shape color: 0x%04x\n",
               cur_shape->color);
        
        printf("Shape zindex: %d\n",
               cur_shape->zindex);
        
        /**
         * 마냑에 fdraw_points가 유효하다면은,,,
         */
        if (cur_shape->fdraw_points.next != NULL) {
            printf("Shape points: ");
            
            struct point_node *cur_point;
            list_for_each_entry(cur_point, &cur_shape->fdraw_points, list) {
                printf("(%d, %d) ",
                       cur_point->x,
                       cur_point->y);
            }
            
            printf("\n");
        }
        
        printf("============ SHAPE END ============\n\n");
    }
    
    /**
     * 삭제까지!
     */
    struct shape *cur;
    struct shape *save;
    list_for_each_entry_safe(cur, save, &shapes_head, list) {
        shape_delete(cur);
    }
    
    return 0;
}

int paint_usecase_test() {
    int                 ts_fd; /* 터치스크린 파일 기술자 */
    int                 dp_fd; /* 디스플레이 파일 기술자 */
    
    /**
     * 터치스크린 열기!
     */
    ts_fd = open(TS_FD_PATH, TS_OPEN_OPTION);
    ASSERTDO(ts_fd != -1, print_error("open() error with %s.\n", TS_FD_PATH); return -1);
    
    /**
     * 디스플레이 열기!
     */
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    /**
     * mmap 해줍니다. 결과는 display.c에 전역변수로 들어가있습니다.
     */
    disp_map(dp_fd);
    
    /**
     * 화면 전체를 흰색으로 칠합니다.
     */
    disp_draw_whole(COLOR(255, 255, 255));
    disp_commit();
    
    struct paint *paint = paint_create();
    paint_test(paint);
    
    return 0;
}

int area_test(void) {
    /*
    struct shape *s = shape_create(ST_RECT, 6, 6, -5, -5, 0);
    printf("Created shape with value[4] = { 6, 6, -5, -5 }.\n\n");
    */
    struct shape *s = shape_create(ST_RECT, 5, 5, 0, 0, 0);
    printf("Created shape with value[4] = { 5, 5, 0, 0 }.\n\n");
    
    SHAPE_EXPORT_AREA_TO_TWO_POINTS(s, x0, y0, x1, y1);
    printf("Exportd that area to two points:\n");
    printf("p0(%d, %d) and p1(%d, %d)\n\n", x0, y0, x1, y1);

    SHAPE_EXPORT_AREA_TO_POINT_AND_SIZE(s, x, y, width, height);
    printf("Exportd that area to point and size:\n");
    printf("p(%d, %d) and s(%d, %d)\n\n", x, y, width, height);
    
    return 0;
}

int button_test(void) {
    int                 ts_fd; /* 터치스크린 파일 기술자 */
    int                 dp_fd; /* 디스플레이 파일 기술자 */
    int                 ts_read; /* 터치 읽은 결과 저장 */
    
    /**
     * touch_event 구조체 선언 & 초기화
     */
    TOUCH_EVENT(te);
    
    /**
     * 터치스크린 열기!
     */
    ts_fd = open(TS_FD_PATH, TS_OPEN_OPTION);
    ASSERTDO(ts_fd != -1, print_error("open() error with %s.\n", TS_FD_PATH); return -1);
    
    /**
     * 디스플레이 열기!
     */
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    /**
     * mmap 해줍니다. 결과는 display.c에 전역변수로 들어가있습니다.
     */
    disp_map(dp_fd);
    

    struct paint *mypaint = paint_create();

    while (1) {
        ts_read = touch_read(ts_fd, &te);
        
        if (te.touch_state == TOUCH_STATE_BEGIN) {
            paint_touch_start(mypaint, te.x, te.y);
        }
        else if (te.touch_state == TOUCH_STATE_DONE) {

        }
        else if (te.touch_state == TOUCH_STATE_DONE) {
            
        }
    }
}
