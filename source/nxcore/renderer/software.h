#ifndef NINETAILSX_SOFTWARE_H
#define NINETAILSX_SOFTWARE_H
#include <nxcore/helpers.h>
#include <nxcore/renderer/dibitmap.h>
/**
 * TODO:
 * 			I'd like to learn how to draw an arbitrary line based on two coordinates.
 * 			This is a good place to start as it is fast and somewhat simple to do:
 * 	
 * Bresenham's Line Algorithm
 * 			https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 */
internal void
__DrawLine() { /** IMPLEMENT LATER */ return; };

/**
 * Determines if one bitmap exists within the boundary of another bitmap with the origin
 * placed at the left-lower corner.
 */
inline b32
IsWithinBitmapBounds(i32 parentWidth, i32 parentHeight, i32 x, i32 y, i32 subWidth, i32 subHeight)
{
	b32 _within_bounds = true;

	if (x+subWidth < 0) _within_bounds = false;
	if (x >= parentWidth) _within_bounds = false;
	if (y+subHeight < 0) _within_bounds = false;
	if (y >= parentHeight) _within_bounds = false;

	return _within_bounds;
}



/**
 * Draws a rectangle of a given size and color to a bitmap.
 * 
 * This improved version:
 * Utilizes the simpler code of the other draw functions and no longer uses the
 * renderer struct as a dependancy (because these draw functions operate on bitmaps,
 * we don't really care *what* bitmap it is we are drawing to, only that it is a
 * properly formatted bitmap).
 */
internal void
DrawRect(dibitmap* bitmap, v2i rectPos, v2i rectDims, u32 color)
{

	// Check within bounds for drawing, exit if it isn't.
	if (!IsWithinBitmapBounds(bitmap->dims.width, bitmap->dims.height,
		rectPos.x, rectPos.y, rectDims.width, rectDims.height)) return;

	// Boundary adjustments on the x-boundary.
	if (rectPos.x < 0)
	{
		rectDims.width -= absolute_i32(rectPos.x);
		rectPos.x = 0;
	}
	else if (rectPos.x+rectDims.width > bitmap->dims.width)
	{
		rectDims.width -= ((rectPos.x+rectDims.width) - bitmap->dims.width);
	}

	// Boundary adjustments on the y-boundary.
	if (rectPos.y < 0)
	{
		rectDims.height -= absolute_i32(rectPos.y);
		rectPos.y = 0;
	}
	else if (rectPos.y+rectDims.height > bitmap->dims.height)
	{
		rectDims.height -= ((rectPos.y+rectDims.height) - bitmap->dims.height);
	}

	// Drawing to the bitmap.
	u32* offsetStart = (u32*)bitmap->buffer + (bitmap->dims.width*rectPos.y) + rectPos.x;
	for (i32 Row = 0; Row < rectDims.height; ++Row)
	{
		u32* Pitch = offsetStart + (Row*bitmap->dims.width);
		for (i32 Col = 0; Col < rectDims.width; ++Col)
		{
			u32* Pixel = Pitch + Col;
			*Pixel = color;
		}
	}

}

/**
 * Draws a bitmap to the screen at a given position. The bitmapWidth and bitmapHeight *must*
 * be the exact size of the bitmap as it is required for proper pitch calculations.
 */
internal void
DrawBitmap(dibitmap* dest, dibitmap* source, v2i position)
{

	// Check within bounds, exit if it isn't.
	if (!IsWithinBitmapBounds(dest->dims.width, dest->dims.height,
		position.x, position.y, source->dims.width, source->dims.height)) return;


	// We need to preserve parameter values.
	u32* sourceBitmap = (u32*)source->buffer;
	i32 sourceWidth = source->dims.width;
	i32 sourceHeight = source->dims.height;
	
	// If we are slightly out of bounds, we need to clip the source and offset the source bitmap.
	if (position.x < 0)
	{
		i32 offsetX = absolute_i32(position.x);
		sourceWidth -= offsetX;
		sourceBitmap += offsetX; // Offsetting forward
		position.x = 0; // Placing back in bounds
	}

	// We do not need to offset the source bitmap, but we do need to shrink the width to fit
	// against the right edge.
	else if (position.x+sourceWidth >= dest->dims.width)
	{
		sourceWidth -= ((position.x+sourceWidth) - dest->dims.width);
	}

	// Applying what we did to the x-coordinates with they y-coordinates above, except now we need
	// to jump down rows.
	if (position.y < 0)
	{
		i32 offsetY = absolute_i32(position.y);
		sourceHeight -= offsetY;
		sourceBitmap += (source->dims.width * offsetY); // Offsetting down y-rows
		position.y = 0; // Placing back in bounds
	}

	// Shrink to fit at y.
	else if (position.y+sourceHeight >= dest->dims.height)
	{
		sourceHeight -= ((position.y+sourceHeight) - dest->dims.height);
	}

	// Now we can copy to the buffer.
	u32* destBitmap = (u32*)dest->buffer + ((dest->dims.width*position.y) + position.x);
	for (i32 row = 0; row < sourceHeight; ++row)
	{
		u32* destPitch = destBitmap + (row*dest->dims.width);
		u32* sourcePitch = sourceBitmap + (row*source->dims.width);
		for (i32 col = 0; col < sourceWidth; ++col)
		{
			*destPitch++ = *sourcePitch++;
		}
	}

}
#if 0
/**
 * Draws a texture to the screen.
 */
internal void
DrawTexture(renderer* Renderer, texture* tex, i32 x, i32 y)
{

	// Draw the bitmap.
	//DrawBitmap(Renderer, bitmapSource, x, y, tex->dims.width, tex->dims.height);


	// Check within bounds, exit if it isn't.
	if (!IsWithinBitmapBounds(Renderer->WindowDimensions.width, Renderer->WindowDimensions.height,
		x, y, tex->dims.width, tex->dims.height)) return;


	// We need to preserve parameter values.
	v2i uvOffset = GetAbsoluteCoordsFromTextureUV(tex);
	u32* sourceBitmap = (u32*)tex->source->buffer + 
		(tex->source->dims.width*uvOffset.y) + (uvOffset.x);
	i32 sourceWidth = tex->dims.width;
	i32 sourceHeight = tex->dims.height;
	
	// If we are slightly out of bounds, we need to clip the source and offset the source bitmap.
	if (x < 0)
	{
		i32 offsetX = absolute_i32(x);
		sourceWidth -= offsetX;
		sourceBitmap += offsetX; // Offsetting forward
		x = 0; // Placing back in bounds
	}

	// We do not need to offset the source bitmap, but we do need to shrink the width to fit
	// against the right edge.
	else if (x+sourceWidth >= Renderer->WindowDimensions.width)
	{
		sourceWidth -= ((x+sourceWidth) - Renderer->WindowDimensions.width);
	}

	// Applying what we did to the x-coordinates with they y-coordinates above, except now we need
	// to jump down rows.
	if (y < 0)
	{
		i32 offsetY = absolute_i32(y);
		sourceHeight -= offsetY;
		sourceBitmap += (tex->source->dims.width * offsetY); // Offsetting down y-rows
		y = 0; // Placing back in bounds
	}

	// Shrink to fit at y.
	else if (y+sourceHeight >= Renderer->WindowDimensions.height)
	{
		sourceHeight -= ((y+sourceHeight) - Renderer->WindowDimensions.height);
	}

	// Now we can copy to the buffer.
	u32* destBitmap = (u32*)Renderer->Image + ((Renderer->WindowDimensions.width*y) + x);
	for (i32 row = 0; row < sourceHeight; ++row)
	{
		u32* destPitch = destBitmap + (row*Renderer->WindowDimensions.width);
		u32* sourcePitch = sourceBitmap + (row*tex->source->dims.width);
		for (i32 col = 0; col < sourceWidth; ++col)
		{
			*destPitch++ = *sourcePitch++;
		}
	}

}
internal void
DrawTexture(dibitmap* bitmap, texture* tex, i32 x, i32 y)
{

	// Draw the bitmap.
	//DrawBitmap(Renderer, bitmapSource, x, y, tex->dims.width, tex->dims.height);


	// Check within bounds, exit if it isn't.
	if (!IsWithinBitmapBounds(bitmap->dims.width, bitmap->dims.height,
		x, y, tex->dims.width, tex->dims.height)) return;


	// We need to preserve parameter values.
	v2i uvOffset = GetAbsoluteCoordsFromTextureUV(tex);
	u32* sourceBitmap = (u32*)tex->source->buffer + 
		(tex->source->dims.width*uvOffset.y) + (uvOffset.x);
	i32 sourceWidth = tex->dims.width;
	i32 sourceHeight = tex->dims.height;
	
	// If we are slightly out of bounds, we need to clip the source and offset the source bitmap.
	if (x < 0)
	{
		i32 offsetX = absolute_i32(x);
		sourceWidth -= offsetX;
		sourceBitmap += offsetX; // Offsetting forward
		x = 0; // Placing back in bounds
	}

	// We do not need to offset the source bitmap, but we do need to shrink the width to fit
	// against the right edge.
	else if (x+sourceWidth >= Renderer->WindowDimensions.width)
	{
		sourceWidth -= ((x+sourceWidth) - Renderer->WindowDimensions.width);
	}

	// Applying what we did to the x-coordinates with they y-coordinates above, except now we need
	// to jump down rows.
	if (y < 0)
	{
		i32 offsetY = absolute_i32(y);
		sourceHeight -= offsetY;
		sourceBitmap += (tex->source->dims.width * offsetY); // Offsetting down y-rows
		y = 0; // Placing back in bounds
	}

	// Shrink to fit at y.
	else if (y+sourceHeight >= Renderer->WindowDimensions.height)
	{
		sourceHeight -= ((y+sourceHeight) - Renderer->WindowDimensions.height);
	}

	// Now we can copy to the buffer.
	u32* destBitmap = (u32*)Renderer->Image + ((Renderer->WindowDimensions.width*y) + x);
	for (i32 row = 0; row < sourceHeight; ++row)
	{
		u32* destPitch = destBitmap + (row*Renderer->WindowDimensions.width);
		u32* sourcePitch = sourceBitmap + (row*tex->source->dims.width);
		for (i32 col = 0; col < sourceWidth; ++col)
		{
			*destPitch++ = *sourcePitch++;
		}
	}

}
#endif


/**
 * Creates a bitmap layer, returns the data back as a dibitmap. This layer is formatted
 * with the renderer's dimensions such that it can be drawn on.
 * 
 * This pushes on the top of the bt-monotonic memory arena.
 */
internal dibitmap
CreateBitmapLayer(btmonotonic_memory_arena* arena, v2i dims)
{

	// We need to allocate space for the buffer (pixel size * dims) and the header
	// structure. This is needed to properly define the bitmap.
	u32 buffsize = (sizeof(u32) * (u32)dims.x * (u32)dims.y) + sizeof(bitmap_header);
	void* buffer = BTMonotonicArenaPushTopSize(arena, buffsize);

	dibitmap bitmap = {};
	bitmap.dims = dims;
	bitmap.buffer = (u8*)buffer + sizeof(bitmap_header); // Where the actual pixel data is stored.
	bitmap.header = (bitmap_header*)buffer; // Cast the head of the bitmap buffer to bitmap_header.

	// Fill out the file header.
	bitmap.header->fileHeader.dataOffset = sizeof(bitmap_header);
	bitmap.header->fileHeader.signature = 'MB'; // Little endian...?
	bitmap.header->fileHeader.fileSize = buffsize;
	bitmap.header->fileHeader._reserved = 0x9F011FFF; // We can set this to whatever we want, so sign it!

	// Fill out the info header.
	bitmap.header->infoHeader.bitmask_alpha = 	0xFF000000;
	bitmap.header->infoHeader.bitmask_red = 	0x00FF0000;
	bitmap.header->infoHeader.bitmask_green = 	0x0000FF00;
	bitmap.header->infoHeader.bitmask_blue = 	0x000000FF;
	bitmap.header->infoHeader.bpp = 32; // Always!
	bitmap.header->infoHeader.compression = 0; // No compression, BI_RGB
	bitmap.header->infoHeader.height = dims.height;
	bitmap.header->infoHeader.width = dims.width;
	bitmap.header->infoHeader.size = sizeof(bitmap_info_header_v5); // Size of the infoheader.
	bitmap.header->infoHeader.imageSize = (sizeof(u32) * (u32)dims.x * (u32)dims.y); // Size of pixel buffer.
	bitmap.header->infoHeader.planes = 1; // Always 1.
	bitmap.header->infoHeader.colorsUsed = 0; // We aren't using a color pallete.

	return bitmap;

}

#endif