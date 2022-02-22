#ifndef NINETAILSX_RESOURCES_H
#define NINETAILSX_RESOURCES_H
#include <nxcore/helpers.h>
#include <nxcore/math.h>
#include <nxcore/resource/dibitmap.h>

typedef struct texture
{
	v2 uvCoords;
	v2i dims;
	dibitmap* source;
} texture;

/**
 * Converts 0.0f-1.0f UV coordinates to absolute pixel position.
 */
inline v2i
GetAbsoluteCoordsFromTextureUV(texture* tex)
{
	// Truncation should place us at the right position.
	v2i _coords;
	_coords.x = (i32)(tex->uvCoords.u * (r32)tex->source->dims.width);
	_coords.y = (i32)(tex->uvCoords.v * (r32)tex->source->dims.height);
	return _coords;
}

/**
 * Sets the UV coordinates of a texture with given absolute coordinates.
 */
inline void
SetTextureUVCoordsFromAbsolute(texture* tex, v2i coords)
{
	// We need to cast to float to get the proper values.
	tex->uvCoords.u = (r32)coords.x / (r32)tex->source->dims.width;
	tex->uvCoords.v = (r32)coords.y / (r32)tex->source->dims.height;
}

/**
 * Creates a texture from a dibitmap source. Defining a sourceTexOffset from a value
 * within the bounds of the texture's width/height defines the starting point UV coordinates
 * where the texture begins to draw upon. Additionally, sourceTexDims determines how big
 * these bounds are--IE, an offset of x16, y16 with a dim size of w16, h16 will begin drawing
 * 16 pixels up from the origin, 16 pixels right, drawing an area of 16x16.
 * 
 * TODO:
 * 			We need to ensure that texture offset is within bounds.
 * 			We also need to ensure that the texture dims are within bounds
 * 			in respect to the offset.
 */
internal texture
CreateTextureFromBitmap(dibitmap* source, v2i sourceTexOffset, v2i sourceTexDims)
{
	texture _tex;
	_tex.source = source;
	_tex.dims = sourceTexDims;
	_tex.uvCoords.u = (r32)sourceTexOffset.x / (r32)source->dims.width;
	_tex.uvCoords.v = (r32)sourceTexOffset.y / (r32)source->dims.height;
	return _tex;
}

#endif