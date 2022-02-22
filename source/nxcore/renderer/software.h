#ifndef NINETAILSX_SOFTWARE_H
#define NINETAILSX_SOFTWARE_H
#include <nxcore/helpers.h>
#include <nxcore/renderer.h>

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
 * DrawRect
 * 			Draws a rectangle of a given size at a given position using a given hex-encoded color.
 * 			This color follows the the 0xXXRRGGBB format.
 */
internal void
DrawRect(renderer* Renderer, i32 x, i32 y, i32 width, i32 height, u32 color)
{

	// This is meant to preserve the state of the input variables for debugging purposes.
	i32 RectX = x;
	i32 RectY = y;
	i32 RectWidth = width;
	i32 RectHeight = height;

	/**
	 * Okay, so drawing rectangles isn't as easy as it seems. First, we are confined to the image
	 * buffer provided by the renderer. Since the x,y coordinates of the rectangle we are drawing
	 * can be any arbitrary value (and potentially be outside the range of the image buffer), we need
	 * to consider "out of bounds" drawing. First, let's look at the y-position.
	 * 
	 * Since the top-left corner is considered the origin, if the y-position is negative and we add
	 * the height, we will "know" if it is within bounds if the value is positive, negative if not.
	 * We don't need to draw anything if it is negative so we can just return.
	 */

	if (y+RectHeight < 0) return;

	// If y is negative, but still within bounds, then we can just shrink the height
	// by the y offset to adjust height such that it is within bounds and then set the y to 0.
	else if (y < 0)
	{
		RectHeight -= absolute_i32(y); // Value is negative, but for the sake understandability...
		RectY = 0;
	}

	// What about over-bounds y? Well, same deal. First, we see if the y+height is large than
	// the height of the bitmap region then we shrink it to fit.
	else if (y+RectHeight >= Renderer->WindowDimensions.height)
	{
		// Now we know that the rect is out of bounds, but how out of bounds is it?
		i32 Overbounds = (y+RectHeight) - Renderer->WindowDimensions.height;
		if (Overbounds >= RectWidth) return; // No point in drawing if it's completely out of bounds!

		// Now shrink the rectange by the overbounds.
		RectHeight -= Overbounds;
	}

	/**
	 * Now comes the question of the the x-bounds. Interestingly, if this isn't handled properly,
	 * it won't throw any errors but it will draw weird. The solution is the movement of the x-coord
	 * in the case that x is out of bounds left (but still drawable) and shrinking, and simply shrink
	 * if out of bounds on the right.
	 */
	if (x+RectWidth < 0) return; // Once again, if it's not in bounds, no need to draw.

	else if (x < 0)
	{
		RectWidth -= absolute_i32(x);
		RectX = 0;
	}

	/**
	 * In the case that the rect is out of the right bounds, we can just shrink the x.
	 */
	if (x+RectWidth >= Renderer->WindowDimensions.width)
	{
		i32 Overbounds = (x+RectWidth) - Renderer->WindowDimensions.width;
		if (Overbounds >= RectWidth) return; // If it's not even within bounds, exit.

		// Shrink.
		RectWidth -= Overbounds;
	}

	/**
	 * We need to grab the starting point of the draw location. First, we need to
	 * navigate down y-rows by incremementing the image base pointer over y*width.
	 * This will place us on the correct row. Then we just need to navigate to the right
	 * based on the x location and that will place at the exact starting location.
	 */
	u32* Loc = (u32*)Renderer->Image + (Renderer->WindowDimensions.width*RectY) + RectX;
	for (i32 Row = 0; Row < RectHeight; ++Row)
	{
		/**
		 * The pixel pitch places the pointer at the start of each row. Since we advance
		 * by the width of the buffer, we guaranteed to hit the next starting position
		 * one row down.
		 */
		u32* Pitch = Loc + (Row*Renderer->WindowDimensions.width);
		for (i32 Col = 0; Col < RectWidth; ++Col)
		{
			u32* Pixel = Pitch + Col;
			*Pixel++ = color;
		}
	}

}

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
 * Draws a bitmap to the screen at a given position. The bitmapWidth and bitmapHeight *must*
 * be the exact size of the bitmap as it is required for proper pitch calculations.
 */
internal void
DrawBitmap(renderer* Renderer, void* bitmap, i32 x, i32 y, i32 bitmapWidth, i32 bitmapHeight)
{

	// Check within bounds, exit if it isn't.
	if (!IsWithinBitmapBounds(Renderer->WindowDimensions.width, Renderer->WindowDimensions.height,
		x, y, bitmapWidth, bitmapHeight)) return;


	// We need to preserve parameter values.
	u32* sourceBitmap = (u32*)bitmap;
	i32 sourceWidth = bitmapWidth;
	i32 sourceHeight = bitmapHeight;
	
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
		sourceBitmap += (bitmapWidth * offsetY); // Offsetting down y-rows
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
		u32* sourcePitch = sourceBitmap + (row*bitmapWidth);
		for (i32 col = 0; col < sourceWidth; ++col)
		{
			*destPitch++ = *sourcePitch++;
		}
	}

}

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