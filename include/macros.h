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

#define SWAP(X, Y)                                                                      \
do {                                                                                    \
    __typeof__ (X) _temp = X;                                                           \
    X = Y;                                                                              \
    Y = _temp;                                                                          \
} while(0)

#define ENSURE_RIGHT_BIGGER(LEFT, RIGHT)                                                \
do {                                                                                    \
    if (LEFT > RIGHT) {                                                                 \
        SWAP(LEFT, RIGHT);                                                              \
    }                                                                                   \
} while (0)                                                                             \

/**
 * 8비트 정수용 비트단위 읽기/쓰기 매크로입니다!
 */
#define GET_BIT8(PTR, OFFSET)                                                           \
((*(PTR + ((OFFSET) / 8))) & (1 << (7 - ((OFFSET) % 8))))

#define SET_BIT8(PTR, OFFSET)                                                           \
do {                                                                                    \
*(PTR + ((OFFSET) / 8)) |= (1 << (7 - ((OFFSET) % 8)));                                 \
} while (0)

#define UNSET_BIT8(PTR, OFFSET)                                                         \
do {                                                                                    \
*(PTR + ((OFFSET) / 8)) &= ~(1 << (7 - ((OFFSET) % 8)));                                \
} while (0)

/**
 * 32비트 정수용 비트단위 읽기/쓰기 매크로입니다!
 */
#define GET_BIT32(PTR, OFFSET)                                                          \
((*(PTR + ((OFFSET) / 32))) & (1 << (31 - ((OFFSET) % 32))))

#define SET_BIT32(PTR, OFFSET)                                                          \
do {                                                                                    \
*(PTR + ((OFFSET) / 32)) |= (1 << (31 - ((OFFSET) % 32)));                              \
} while (0)

#define UNSET_BIT32(PTR, OFFSET)                                                        \
do {                                                                                    \
*(PTR + ((OFFSET) / 32)) &= ~(1 << (31 - ((OFFSET) % 32)));                             \
} while (0)

/**
 * http://netstorage.iar.com/SuppDB/Public/SUPPORT/000419/AN-G-002.pdf
 */
static inline unsigned int root(unsigned int x) {
    unsigned int a;
    unsigned int b;
    
    b = x;
    a = x = 0x3f;
    
    x = b/x;
    a = x = (x+a) >> 1;
    
    x = b/x;
    a = x = (x+a) >> 1;
    
    x = b/x;
    a = x = (x+a) >> 1;

    return x;
}

#endif /* macros_h */
