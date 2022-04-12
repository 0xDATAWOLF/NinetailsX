#ifndef NINETAILSX_RENDERER_H
#define NINETAILSX_RENDERER_H
#include <nxcore/helpers.h>
#include <nxcore/math.h>
#include <nxcore/renderer/software.h>
#include <nxcore/renderer/colors.h>
#include <nxcore/renderer/dibitmap.h>

typedef struct
{
	dibitmap source;
} texture_t;

inline texture_t
CreateTexture(dibitmap source)
{

	texture_t _tex;
	_tex.source = source;

	return _tex;

}

#endif