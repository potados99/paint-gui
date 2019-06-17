/*******************************************************************
 * 2019년 1학기 임베디드시스템 기말 팀 과제
 *
 * 프로젝트 이름 :     paint-gui
 * 프로젝트 시작 :     2019.06.03
 * 프로젝트 종료 :     2019.06.17

 * 프로그램 구조 :
 * 프로그램은 크게 입출력과 처리 부분으로 나뉘어집니다.
 * 사용자로부터 터치 입력을 받아서 처리한 뒤 디스플레이로 출력하는 흐름을 가집니다.
 * 함수 호출 구조는 다음과 같습니다.
 *
 *  main()
 *      -> do_it()                                  (at routine.h)
 *          -> touch_read()                         (at touch.h)
 *              -> paint_touch_***()                (at paint.h)
 *                  -> ...                          (at paint.c)
 *                      -> disp_draw_***()          (at display.h)
 *                      -> disp_commit()            (at display.h)
 *
 * README.md에 자세한 설명이 실려있습니다.
 *******************************************************************/

#include "routine.h" /* do_it() */

int main(int argc, const char *argv[]) {
    do_it();

    return 0;
}

