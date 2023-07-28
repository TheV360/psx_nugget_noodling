#pragma once

#include <stdbool.h> // -> bool, true false
#include <stddef.h>  // -> size_t, ptrdiff_t, NULL

// #include <stdint.h>  // -> uint8_t, etc.
// stdint doesn't actually exist in nugget+psyq?? there's a stub deep in the
// folders but it seems to just hand off to my host system's libraries?
// i think it's better to just do this and have complete control over things

#include <sys/types.h> // -> u_char, etc.

// don't tell anyone this is how i live.

typedef __UINT8_TYPE__  u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

typedef __INT8_TYPE__  s8;
typedef __INT16_TYPE__ s16;
typedef __INT32_TYPE__ s32;
typedef __INT64_TYPE__ s64;

typedef __SIZE_TYPE__    usize;
typedef __PTRDIFF_TYPE__ ssize;
