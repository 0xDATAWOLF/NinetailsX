#ifndef NINETAILSX_COLORS_H
#define NINETAILSX_COLORS_H
#include <nxcore/helpers.h>
#include <nxcore/math.h>

/**
 * Returns a 32-bit unsigned integer which represents the color of a pixel.
 * This can be directly used with DIBitmaps to format pixel color. The individual
 * colors operate on a scale of 0.0f to 1.0f.
 * 
 * TODO:
 * 			We may be able to speed this up using SSE instructions.
 * 
 * NOTE:
 * 			The RGB values may be incorrect depending on the platform.
 */
inline u32
CreateDIBPixel(r32 a, r32 r, r32 g, r32 b)
{

	u32 _pixel = 0x0;
	u32 _pixel_a = (u8)(clamp_r32(a, 0.0f, 1.0f)*255) << 24;
	u32 _pixel_r = (u8)(clamp_r32(r, 0.0f, 1.0f)*255) << 16;
	u32 _pixel_g = (u8)(clamp_r32(g, 0.0f, 1.0f)*255) << 8;
	u32 _pixel_b = (u8)(clamp_r32(b, 0.0f, 1.0f)*255) << 0;
	_pixel |= _pixel_a | _pixel_r | _pixel_g | _pixel_b;
	return _pixel;

}

/**
 * v4 alias of CreateDIBPixel(r32,r32,r32,r32).
 */
inline u32
CreateDIBPixel(v4 color)
{
	return CreateDIBPixel(color.a, color.r, color.g, color.b);
}

#endif