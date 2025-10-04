#pragma once
extern "C" {
#include <ntddk.h>
#include <ntstrsafe.h>
}

#ifndef ULONG_MAX
#define ULONG_MAX 0xFFFFFFFFUL
#endif

typedef unsigned __int64 size_t;
typedef signed __int32 int32_t;

namespace crt {

    inline size_t strlen(const char* s) {
        const char* p = s;
        while (*p) ++p;
        return (p - s);
    }

    inline int strcmp(const char* a, const char* b) {
        while (*a && *a == *b) { ++a; ++b; }
        return *a - *b;
    }

    inline void* memcpy(void* dest, const void* src, size_t len) {
        RtlCopyMemory(dest, src, len);
        return dest;
    }

    inline int tolower(int c) {
        return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
    }

    inline char* strtolower(char* str) {
        for (char* p = str; *p; ++p) *p = static_cast<char>(tolower(*p));
        return str;
    }

}
