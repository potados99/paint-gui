
#ifndef debug_h
#define debug_h


#define ASSERTDO(X, WHAT)       \
do {                            \
if (!(X)) {                     \
WHAT;                           \
}                               \
} while(0)

#if (VERBOSE > 1)
#define print_trace(_fmt, ...) \
fprintf(stdout, "[TRACE] "_fmt, ##__VA_ARGS__)
#else
#define print_trace(_fmt, ...) NULL
#endif

#if (VERBOSE > 0)
#define print_info(_fmt, ...) \
fprintf(stdout, "[INFO] " _fmt, ##__VA_ARGS__)
#else
#define print_info(_fmt, ...) NULL
#endif

#define print_error(_fmt, ...) \
fprintf(stderr, "[ERROR] " _fmt, ##__VA_ARGS__)

#endif /* debug_h */
