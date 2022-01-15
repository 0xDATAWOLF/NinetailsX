#ifndef NINETAILSX_PRIMITIVES_H
#define NINETAILSX_PRIMITIVES_H

/**
 * NOTE:
 * 			In order to establish consistent results across platforms, it's important
 * 			that we keep our primitives to known sizes.
 */
#ifdef NINETAILSX_DEBUG
#include <stdint.h>
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#endif

typedef float r32;
typedef double r64;

typedef u32 b32;
typedef u64 b64;

#endif
