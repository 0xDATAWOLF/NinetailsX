#ifndef NINETAILSX_MATH_H
#define NINETAILSX_MATH_H

#define PI32 3.14159265359f
#define PI64 3.14159265358979323846

#include <nxcore/math/trig.h>
#include <nxcore/math/vector.h>

/**
 * Returns the absolute value of an i32. The method used in this function is very elementary and
 * not as fast as some bit-magic methods may be, so therefore *don't* use this in anything
 * for production.
 * 
 * TODO:
 * 			Optimize this if we need to?
 */
inline i32
absolute_i32(i32 Value)
{
	return (Value >= 0 ? Value : Value*(i32)(-1));
}

#endif