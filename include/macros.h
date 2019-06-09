#ifndef macros_h
#define macros_h

#define UPPER16(X) (((X) >> 16) & 0xFFFF)
#define LOWER16(X) ((X) & 0xFFFF)

#define COMBINE16(X, Y) (((X) << 16) | ((Y) & 0xFFFF))

#define ABS(X) ((X) < 0 ? -(X) : (X))

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

#define SWAP(X, Y)                  \
do {                                \
__typeof__ (X) _temp = X;           \
X = Y;                              \
Y = _temp;                          \
} while(0)

#endif /* macros_h */
