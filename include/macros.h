/**
 * macros.h
 * 쓸만한 매크로 함수들을 모아놓았습니다.
 * 약간의 수학이 필요했는데, 속도가 중요해서 이렇게 매크로로..만들었습니당.
 * 다른 모듈과 무관한 일반적인 수학 함수들입니다.
 */

#ifndef macros_h
#define macros_h

#define UPPER16(X) (((X) >> 16) & 0xFFFF)
#define LOWER16(X) ((X) & 0xFFFF)

#define COMBINE16(X, Y) (((X) << 16) | ((Y) & 0xFFFF))

#define ABS(X) ((X) < 0 ? -(X) : (X))

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

#define SWAP(X, Y)                                                  \
do {                                                                \
    __typeof__ (X) _temp = X;                                       \
    X = Y;                                                          \
    Y = _temp;                                                      \
} while(0)

#define ENSURE_RIGHT_BIGGER(LEFT, RIGHT)                            \
do {                                                                \
    if (LEFT > RIGHT) {                                             \
        SWAP(LEFT, RIGHT);                                          \
    }                                                               \
} while (0)                                                         \

#define GET_BIT(PTR, OFFSET, BIT_WIDTH)                             \
((*(PTR + (OFFSET / (BIT_WIDTH)))) & (1 << (OFFSET % (BIT_WIDTH))))

#define SET_BIT(PTR, OFFSET, BIT_WIDTH)                             \
do {                                                                \
*(PTR + (OFFSET / (BIT_WIDTH))) |= (1 << (OFFSET % (BIT_WIDTH)));   \
} while (0)

#define UNSET_BIT(PTR, OFFSET, BIT_WIDTH)                           \
do {                                                                \
*(PTR + (OFFSET / (BIT_WIDTH))) &= ~(1 << (OFFSET % (BIT_WIDTH)));  \
} while (0)

#endif /* macros_h */
