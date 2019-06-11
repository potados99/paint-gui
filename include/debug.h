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


/**
 * 한 점의 X, Y 좌표가 각각 [0, X_MAX], [0, Y_MAX]에 속하는지 검사합니다.
 */
#define POINT_CHECK(FUNC_NAME, X, Y, X_MAX, Y_MAX)                                          \
do {                                                                                        \
ASSERTDO(IN_RANGE(X, 0, X_MAX),                                                             \
print_error(FUNC_NAME ": " #X "{%d} out of range.\n", X);                                   \
return);                                                                                    \
ASSERTDO(IN_RANGE(Y, 0, Y_MAX),                                                             \
print_error(FUNC_NAME ": " #Y "{%d} out of range.\n", Y);                                   \
return);                                                                                    \
} while (0)

/**
 * 한 크기의 길이와 높이가 각각 [0, W_MAX], [0, H_MAX]에 속하는지 검사합니다.
 */
#define SIZE_CHECK(FUNC_NAME, W, H, W_MAX, H_MAX)                                           \
do {                                                                                        \
ASSERTDO(IN_RANGE(W, 0, W_MAX),                                                             \
print_error(FUNC_NAME ": " #W "{%d} out of range.\n", W);                                   \
return);                                                                                    \
ASSERTDO(IN_RANGE(H, 0, H_MAX),                                                             \
print_error(FUNC_NAME ": " #H "{%d} out of range.\n", H);                                   \
return);                                                                                    \
} while (0)


#endif /* debug_h */
