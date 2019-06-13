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
     * 도형들을 만들고 그려줍니다.
     */
    struct shape sq0;
    sq0.type = ST_RECTP;
    sq0.value[0] = 50;
    sq0.value[1] = 50;
    sq0.value[2] = 150;
    sq0.value[3] = 150;
    sq0.color = 0;
    disp_draw_2d_shape(&sq0);
    disp_commit();
    usleep(400000);
    
    struct shape sq1;
    sq1.type = ST_RECTP;
    sq1.value[0] = 100;
    sq1.value[1] = 70;
    sq1.value[2] = 200;
    sq1.value[3] = 170;
    sq1.color = COLOR(255, 0, 0);
    disp_draw_2d_shape(&sq1);
    disp_commit();
    usleep(400000);
    
    struct shape ln0;
    ln0.type = ST_LINE;
    ln0.value[0] = 80;
    ln0.value[1] = 25;
    ln0.value[2] = 240;
    ln0.value[3] = 210;
    ln0.color = COLOR(0, 128, 0);
    disp_draw_2d_shape(&ln0);
    disp_commit();
    usleep(400000);
    
    struct shape sq2;
    sq2.type = ST_RECTP_FILL;
    sq2.value[0] = 20;
    sq2.value[1] = 120;
    sq2.value[2] = 250;
    sq2.value[3] = 130;
    sq2.color = COLOR(0, 128, 255);
    disp_draw_2d_shape(&sq2);
    disp_commit();
    usleep(400000);
    
    /**
     * 1번 직사각형을 조금 움직여주고 다 새로 그립니다.
     * 하지만 일부만 새로고칩니다.
     */
    for (int i = 0; i < 70; ++i) {
        usleep(100000);
        disp_draw_whole(COLOR(255, 255, 255));
        shape_move(&sq1, 0, +1);
        
        disp_draw_2d_shape(&sq0);
        disp_draw_2d_shape(&sq1);
        disp_draw_2d_shape(&ln0);
        disp_draw_2d_shape(&sq2);
        
        disp_commit_partialp(sq1.value[0], sq1.value[1], sq1.value[2] + sq1.offset[1] - 1, sq1.value[3] + sq1.offset[1]);
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
        
        if (te.touch_state == STATE_TOUCH_DOWN) {
            /**
             * 이제 막 터치가 시작될 때.
             * 이전 터치 좌표를 현재 터치 시작 좌표로 덮어씁니다.
             */
            last_x = te.x;
            last_y = te.y;
            touched = 1;
        }
        else if (te.touch_state == STATE_TOUCH_UP) {
            /**
             * 터치가 막 끝났을 때.
             */
            touched = 0;
        }
        
        /**
         * 적당히 출력해줍니다.
         */
        if (te.touch_state == STATE_TOUCH_DOWN) printf("\n============================= TOUCH START =============================\n");
        printf("[%ld] X: %5d,\tY: %5d,\tPressure: %5d,\tState: %d\n", line_num++, te.x, te.y, te.pressure, te.touch_state);
        if (te.touch_state == STATE_TOUCH_UP) printf("============================= TOUCH FINISH =============================\n\n");
        
        /**
         * 이전 점과 현재 점 사이에 직선을 그려주고, 그 구간의 변화를 적용합니다.
         */
        disp_draw_line(last_x, last_y, te.x, te.y, COLOR(0, 0, 0));
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
    struct shape *fdraw = shape_create(ST_FDRAW, 0, 0, 0, 0, COLOR(0, 0, 50));
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
