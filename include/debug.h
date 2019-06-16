/**
 * debug.h
 * 디버그를 위한 매크로들을 모아놓았습니다.
 */

#ifndef debug_h
#define debug_h

#include <stdio.h>

/**
 * X가 참일 것이라 가정한 뒤, 아니면 WHAT을 실행합니다.
 */
#define ASSERTDO(X, WHAT)       \
do {                            \
if (!(X)) {                     \
WHAT;                           \
}                               \
} while(0)

/**
 * 이 프로그램에 사용된 디버깅 시스템에서, 로그는 종 세 종류입니다.
 * 첫 번째는 print_trace, 가장 하찮고 별로 중요하지도 않은 것입니다.
 * 주로 프로그램의 흐름을 파악할 때 쓰입니다.
 * 두 번째는 print_info, 프로그래머가 꼭 알아야 하는 정보를 출력합니다.
 * 세 번째는 print_error, 이건 생기면 안됩니다. 반드시 알아야 합니다.
 *
 * VERBOSE 레벨에 따른 로그의 출력 여부를 정의해놓았습니다.
 * 만약 VERBOSE가 2이면 (제일 시끄러움) print_error, print_info, print_trace 모두 출력되며,
 * 1이면 print_error, print_info만, 0이면 print_error만 출력됩니다.
 * print_error는 가장 중요하기 때문에 그냥 잘 출력됩니다.
 *
 * 이 VERBOSE 전처리기 변수는 Makefile에 의해 전달됩니다. Makefile 손보세용.
 */
#if (VERBOSE > 1)
#define print_trace(_fmt, ...)                                                              \
fprintf(stdout, "[TRACE] "_fmt, ##__VA_ARGS__)
#else
#define print_trace(_fmt, ...) NULL
#endif

#if (VERBOSE > 0)
#define print_info(_fmt, ...)                                                               \
fprintf(stdout, "[INFO] " _fmt, ##__VA_ARGS__)
#else
#define print_info(_fmt, ...) NULL
#endif

#define print_error(_fmt, ...)                                                              \
fprintf(stderr, "[ERROR] " _fmt, ##__VA_ARGS__)


/**
 * X가 NULL인지 검사합니다.
 */
#define NULL_CHECK(FUNC_NAME, X)                                                            \
ASSERTDO((X) != NULL, print_error(FUNC_NAME ": " #X " is NULL.\n"); return)

/**
 * NULL_CHECK를 한 다음에 값을 반환합니다.
 */
#define NULL_CHECK_RET(FUNC_NAME, X, RET)                                                   \
ASSERTDO((X) != NULL, print_error(FUNC_NAME ": " #X " is NULL.\n"); return RET)

#endif /* debug_h */
