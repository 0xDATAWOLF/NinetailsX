#ifndef NINETAILSX_RENDERER_H
#define NINETAILSX_RENDERER_H
#include <nxcore/helpers.h>
#include <nxcore/math.h>
#include <nxcore/renderer/software.h>
#include <nxcore/renderer/colors.h>

typedef struct
{
	dibitmap* source;
	v2i dims;
} texture_t;

inline texture_t
CreateTextureFromBitmap(dibitmap* source)
{
	texture_t _tex;
	_tex.dims = source->dims;
	_tex.source = source;
}

#endif