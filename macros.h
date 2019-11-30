#ifndef MACROS_HEADER_FILE
#define MACROS_HEADER_FILE
#define new(type) (type *) calloc(1, sizeof(type))
#define NOTE             \
    siteLine = __LINE__; \
    siteFile = __FILE__;
#define ERROR(...)                                                        \
    { fprintf(stderr, "\033[31;1;4m[%s:%d]\033[0m:\t", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                                         \
    putc('\n', stderr);                                                   \
    error(1); }
#define LOG(...)                   \
    if(logVerbosity > 0) {          \
        log_lable(__FILE__, __LINE__);  \
        fprintf(stderr, __VA_ARGS__);   \
        putc('\n', stderr);             \
        fflush(stderr);                 \
    }
#define LOG2(...)                      \
    if (logVerbosity > 2) {            \
        log_lable(__FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);  \
        putc('\n', stderr);            \
        fflush(stderr);                \
    }
#define LOG3(...)                      \
    if (logVerbosity > 3) {            \
        log_lable(__FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);  \
        putc('\n', stderr);            \
        fflush(stderr);                \
    }
#define PAUSE_IF(condition) if(condition) { pause() }
#endif
