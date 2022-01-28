#ifndef NINETAILSX_PRIMITIVES_H
#define NINETAILSX_PRIMITIVES_H

/**
 * We need to rely on the standard library for guaranteed type sizing.
 */
#include <stdint.h>

/**
 * Typedef'd signed integer definitions.
 */
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;


/**
 * Typedef'd unsigned integer definitions.
 */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/**
 * Typedef'd real number definitions.
 */
typedef float r32;
typedef double r64;

/**
 * Typedef'd boolean definitions. They are useful for struct-packing as the
 * normal bool definition is usually just an unsigned 8-bit value.
 */
typedef u32 b32;
typedef u64 b64;

#endif
