
#ifndef macros_h
#define macros_h

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

#define SWAP(X, Y)                  \
do {                                \
__typeof__ (X) _temp = X;           \
X = Y;                              \
Y = _temp;                          \
} while(0)

#endif /* macros_h */
