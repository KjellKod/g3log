#ifndef EXECINFO_MOCK_H
#define EXECINFO_MOCK_H

// Mock backtrace() — returns 0 frames captured
static inline int backtrace(void **buffer, int size) {
    (void)buffer; // suppress unused warnings
    (void)size;
    return 0;
}

// Mock backtrace_symbols() — returns NULL
static inline char **backtrace_symbols(void *const *buffer, int size) {
    (void)buffer;
    (void)size;
    return 0;
}

// Mock backtrace_symbols_fd() — does nothing
static inline void backtrace_symbols_fd(void *const *buffer, int size, int fd) {
    (void)buffer;
    (void)size;
    (void)fd;
}

#endif // EXECINFO_MOCK_H
