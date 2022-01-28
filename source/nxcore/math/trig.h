#ifndef NINETAILSX_TRIG_H
#define NINETAILSX_TRIG_H
#include <nxcore/helpers.h>
#include <nxcore/primitives.h>
#include <math.h>

/**
 * Real 32-bit implementation of the trig-functions.
 */

internal r32
sin_r32(r32 rad)
{
	r32 _value = sinf(rad);
	return _value;
}

internal r32
cos_r32(r32 rad)
{
	r32 _value = cosf(rad);
	return _value;
}

internal r32
tan_r32(r32 rad)
{
	r32 _value = tanf(rad);
	return _value;
}

#endif